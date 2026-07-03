#include "audioengine.h"

#include <QAudioDevice>
#include <QMediaDevices>
#include <QAudioFormat>
#include <algorithm>
#include <cmath>
#include <opus.h>

AudioEngine::AudioEngine(QObject *parent)
    : QObject(parent)
{
}

AudioEngine::~AudioEngine()
{
    stopCapture();
    stopPlayback();
    if (m_encoder) opus_encoder_destroy(m_encoder);
    if (m_decoder) opus_decoder_destroy(m_decoder);
}

void AudioEngine::ensureEncoder()
{
    if (m_encoder) return;
    int err = 0;
    m_encoder = opus_encoder_create(kSampleRate, kChannels, OPUS_APPLICATION_VOIP, &err);
    if (err != OPUS_OK) {
        emit audioError(QString("Opus encoder init failed: %1").arg(opus_strerror(err)));
        m_encoder = nullptr;
    }
}

void AudioEngine::ensureDecoder()
{
    if (m_decoder) return;
    int err = 0;
    m_decoder = opus_decoder_create(kSampleRate, kChannels, &err);
    if (err != OPUS_OK) {
        emit audioError(QString("Opus decoder init failed: %1").arg(opus_strerror(err)));
        m_decoder = nullptr;
    }
}

void AudioEngine::startCapture()
{
    if (m_audioSource) return; // already capturing

    ensureEncoder();

    QAudioFormat format;
    format.setSampleRate(kSampleRate);
    format.setChannelCount(kChannels);
    format.setSampleFormat(QAudioFormat::Int16);

    QAudioDevice inputDevice = QMediaDevices::defaultAudioInput();
    if (!inputDevice.isFormatSupported(format)) {
        emit audioError("Default microphone doesn't support 48kHz stereo 16-bit - using closest match");
        format = inputDevice.preferredFormat();
    }

    m_audioSource = std::make_unique<QAudioSource>(inputDevice, format);
    m_captureDevice = m_audioSource->start();
    if (!m_captureDevice) {
        emit audioError("Failed to start microphone capture");
        return;
    }

    connect(m_captureDevice, &QIODevice::readyRead, this, &AudioEngine::onCaptureDataReady);
}

void AudioEngine::stopCapture()
{
    if (m_audioSource) {
        m_audioSource->stop();
        m_audioSource.reset();
        m_captureDevice = nullptr;
    }
    m_captureAccum.clear();
}

void AudioEngine::startPlayback()
{
    if (m_audioSink) return;

    ensureDecoder();

    QAudioFormat format;
    format.setSampleRate(kSampleRate);
    format.setChannelCount(kChannels);
    format.setSampleFormat(QAudioFormat::Int16);

    QAudioDevice outputDevice = QMediaDevices::defaultAudioOutput();
    if (!outputDevice.isFormatSupported(format)) {
        emit audioError("Default speaker doesn't support 48kHz stereo 16-bit - using closest match");
        format = outputDevice.preferredFormat();
    }

    m_audioSink = std::make_unique<QAudioSink>(outputDevice, format);
    m_playbackDevice = m_audioSink->start();
    if (!m_playbackDevice) {
        emit audioError("Failed to start speaker playback");
    }
}

void AudioEngine::stopPlayback()
{
    if (m_audioSink) {
        m_audioSink->stop();
        m_audioSink.reset();
        m_playbackDevice = nullptr;
    }
}

void AudioEngine::setMuted(bool muted)
{
    m_muted = muted;
}

void AudioEngine::setDeafened(bool deafened)
{
    m_deafened = deafened;
}

void AudioEngine::setMicGainDb(double gainDb)
{
    m_micGainLinear = std::pow(10.0, std::clamp(gainDb, -24.0, 24.0) / 20.0);
}

void AudioEngine::setOutputGainDb(double gainDb)
{
    m_outputGainLinear = std::pow(10.0, std::clamp(gainDb, -24.0, 24.0) / 20.0);
}

void AudioEngine::onCaptureDataReady()
{
    if (!m_captureDevice) return;

    m_captureAccum.append(m_captureDevice->readAll());

    while (m_captureAccum.size() >= kFrameBytes) {
        QByteArray frameBytes = m_captureAccum.left(kFrameBytes);
        m_captureAccum.remove(0, kFrameBytes);

        if (m_muted || !m_encoder) {
            continue; // still drain the buffer, just don't encode/send while muted
        }

        auto *samples = reinterpret_cast<int16_t *>(frameBytes.data());

        // Apply mic gain
        if (m_micGainLinear != 1.0) {
            for (int i = 0; i < kFrameSamples * kChannels; ++i) {
                double v = samples[i] * m_micGainLinear;
                samples[i] = static_cast<int16_t>(std::clamp(v, -32768.0, 32767.0));
            }
        }

        // Apply mic-side EQ (shapes how others hear you)
        m_micEq.process(samples, kFrameSamples);

        QByteArray encoded(4000, Qt::Uninitialized); // Opus max frame size guideline
        int bytesWritten = opus_encode(m_encoder, samples, kFrameSamples,
                                        reinterpret_cast<unsigned char *>(encoded.data()), encoded.size());
        if (bytesWritten < 0) {
            emit audioError(QString("Opus encode failed: %1").arg(opus_strerror(bytesWritten)));
            continue;
        }

        encoded.resize(bytesWritten);
        emit encodedFrameReady(encoded);
    }
}

void AudioEngine::feedIncomingOpus(const QByteArray &opusPayload)
{
    if (!m_decoder || !m_playbackDevice) return;
    if (m_deafened) return; // still decode nothing, don't even touch the audio device

    // NOTE: this is a simplified single-stream playback path - if two
    // people speak at once, their decoded audio is written sequentially
    // rather than properly mixed. Good enough to prove the pipeline
    // works end to end; proper per-speaker mixing is a follow-up.
    QVector<int16_t> pcm(kFrameSamples * kChannels);
    int samplesDecoded = opus_decode(
        m_decoder,
        reinterpret_cast<const unsigned char *>(opusPayload.constData()), opusPayload.size(),
        pcm.data(), kFrameSamples, 0);

    if (samplesDecoded < 0) {
        emit audioError(QString("Opus decode failed: %1").arg(opus_strerror(samplesDecoded)));
        return;
    }

    if (m_outputGainLinear != 1.0) {
        for (int i = 0; i < samplesDecoded * kChannels; ++i) {
            double v = pcm[i] * m_outputGainLinear;
            pcm[i] = static_cast<int16_t>(std::clamp(v, -32768.0, 32767.0));
        }
    }

    m_outputEq.process(pcm.data(), samplesDecoded);

    m_playbackDevice->write(reinterpret_cast<const char *>(pcm.constData()),
                             samplesDecoded * kChannels * sizeof(int16_t));
}
