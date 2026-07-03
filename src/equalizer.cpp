#include "equalizer.h"

#include <algorithm>
#include <cmath>

Equalizer::Equalizer()
{
    m_bands[0] = Band{120.0, 0.0, 0.9};   // bass
    m_bands[1] = Band{1000.0, 0.0, 0.9};  // mid
    m_bands[2] = Band{6000.0, 0.0, 0.9};  // treble

    for (int i = 0; i < kBandCount; ++i) {
        recomputeCoeffs(i);
    }
}

void Equalizer::setBandGain(int bandIndex, double gainDb)
{
    if (bandIndex < 0 || bandIndex >= kBandCount) {
        return;
    }
    m_bands[bandIndex].gainDb = std::clamp(gainDb, -18.0, 18.0);
    recomputeCoeffs(bandIndex);
}

double Equalizer::bandGain(int bandIndex) const
{
    if (bandIndex < 0 || bandIndex >= kBandCount) {
        return 0.0;
    }
    return m_bands[bandIndex].gainDb;
}

void Equalizer::setMasterGainDb(double gainDb)
{
    m_masterGainDb = std::clamp(gainDb, -24.0, 24.0);
    m_masterGainLinear = std::pow(10.0, m_masterGainDb / 20.0);
}

void Equalizer::recomputeCoeffs(int bandIndex)
{
    const Band &band = m_bands[bandIndex];
    BiquadCoeffs &c = m_coeffs[bandIndex];

    double A = std::pow(10.0, band.gainDb / 40.0);
    double omega = 2.0 * M_PI * band.frequencyHz / kSampleRate;
    double alpha = std::sin(omega) / (2.0 * band.q);
    double cosw = std::cos(omega);

    double b0 = 1 + alpha * A;
    double b1 = -2 * cosw;
    double b2 = 1 - alpha * A;
    double a0 = 1 + alpha / A;
    double a1 = -2 * cosw;
    double a2 = 1 - alpha / A;

    c.b0 = b0 / a0;
    c.b1 = b1 / a0;
    c.b2 = b2 / a0;
    c.a1 = a1 / a0;
    c.a2 = a2 / a0;
}

double Equalizer::processSample(double in, BiquadCoeffs &c, BiquadState &s)
{
    // Transposed Direct Form II - numerically stable, standard for
    // real-time biquad processing.
    double out = c.b0 * in + s.z1;
    s.z1 = c.b1 * in - c.a1 * out + s.z2;
    s.z2 = c.b2 * in - c.a2 * out;
    return out;
}

void Equalizer::process(int16_t *interleavedStereoSamples, int frameCount)
{
    for (int i = 0; i < frameCount; ++i) {
        double left = interleavedStereoSamples[i * 2];
        double right = interleavedStereoSamples[i * 2 + 1];

        for (int band = 0; band < kBandCount; ++band) {
            if (m_bands[band].gainDb == 0.0) {
                continue; // skip flat bands - saves CPU, common case
            }
            left = processSample(left, m_coeffs[band], m_stateLeft[band]);
            right = processSample(right, m_coeffs[band], m_stateRight[band]);
        }

        left *= m_masterGainLinear;
        right *= m_masterGainLinear;

        interleavedStereoSamples[i * 2] = static_cast<int16_t>(std::clamp(left, -32768.0, 32767.0));
        interleavedStereoSamples[i * 2 + 1] = static_cast<int16_t>(std::clamp(right, -32768.0, 32767.0));
    }
}

void Equalizer::reset()
{
    for (int i = 0; i < kBandCount; ++i) {
        m_stateLeft[i] = BiquadState{};
        m_stateRight[i] = BiquadState{};
    }
}
