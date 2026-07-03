#pragma once

#include <QAudioSink>
#include <QAudioSource>
#include <QByteArray>
#include <QIODevice>
#include <QObject>
#include <cstdint>
#include <memory>

#include "equalizer.h"

struct OpusEncoder;
struct OpusDecoder;

// Handles the actual audio: capturing from the microphone, encoding to
// Opus, and separately decoding incoming Opus and playing it back through
// the speakers. Deliberately knows nothing about Discord's protocol,
// encryption, or networking - VoiceClient owns that. This class only
// deals in raw PCM <-> Opus, same separation of concerns Abaddon uses
// between its AudioManager and DiscordVoiceClient.
//
// Format: 48kHz, stereo, 16-bit PCM, 20ms frames (960 samples/channel) -
// matches Discord's expected voice format exactly.
class AudioEngine : public QObject
{
    Q_OBJECT
public:
    explicit AudioEngine(QObject *parent = nullptr);
    ~AudioEngine() override;

    void startCapture();
    void stopCapture();

    void startPlayback();
    void stopPlayback();

    void setMuted(bool muted); // stops sending mic audio, capture keeps running
    bool isMuted() const { return m_muted; }

    void setDeafened(bool deafened); // stops playing incoming audio entirely
    bool isDeafened() const { return m_deafened; }

    void setMicGainDb(double gainDb);
    void setOutputGainDb(double gainDb);

    Equalizer &micEqualizer() { return m_micEq; }       // applied before encoding (your voice, as others hear it)
    Equalizer &outputEqualizer() { return m_outputEq; } // applied after decoding (what you hear from others)

    // Feed a decrypted Opus payload (from any remote user) in for decoding
    // and playback. Packets from multiple simultaneous speakers are mixed.
    void feedIncomingOpus(const QByteArray &opusPayload);

signals:
    // One 20ms Opus-encoded frame, ready for VoiceClient to RTP-frame,
    // encrypt, and send over UDP.
    void encodedFrameReady(const QByteArray &opusData);

    void audioError(const QString &reason);

private slots:
    void onCaptureDataReady();

private:
    void ensureEncoder();
    void ensureDecoder();

    std::unique_ptr<QAudioSource> m_audioSource;
    QIODevice *m_captureDevice = nullptr; // owned by m_audioSource, not us

    std::unique_ptr<QAudioSink> m_audioSink;
    QIODevice *m_playbackDevice = nullptr; // owned by m_audioSink, not us

    OpusEncoder *m_encoder = nullptr;
    OpusDecoder *m_decoder = nullptr;

    QByteArray m_captureAccum; // raw PCM bytes waiting to fill a 20ms frame

    Equalizer m_micEq;
    Equalizer m_outputEq;

    bool m_muted = false;
    bool m_deafened = false;
    double m_micGainLinear = 1.0;
    double m_outputGainLinear = 1.0;

    static constexpr int kSampleRate = 48000;
    static constexpr int kChannels = 2;
    static constexpr int kFrameSamples = 960; // 20ms at 48kHz
    static constexpr int kFrameBytes = kFrameSamples * kChannels * sizeof(int16_t);
};
