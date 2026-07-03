#pragma once

#include <QVector>
#include <cstdint>

// Simple 3-band parametric EQ (bass/mid/treble) using RBJ cookbook biquad
// peaking filters, applied in series. Operates in-place on interleaved
// 16-bit stereo PCM at 48kHz (matching Discord's voice format). Each band
// is independently adjustable: center frequency, gain (dB), and Q
// (bandwidth). Defaults to flat (0dB - no coloration) until the person
// adjusts something in the voice settings UI.
class Equalizer
{
public:
    struct Band {
        double frequencyHz;
        double gainDb = 0.0;
        double q = 0.9;
    };

    Equalizer();

    void setBandGain(int bandIndex, double gainDb); // bandIndex: 0=bass, 1=mid, 2=treble
    double bandGain(int bandIndex) const;

    void setMasterGainDb(double gainDb); // simple overall volume trim, +/- 24dB
    double masterGainDb() const { return m_masterGainDb; }

    // Processes `frameCount` interleaved stereo samples (frameCount*2 int16 values) in-place.
    void process(int16_t *interleavedStereoSamples, int frameCount);

    void reset(); // clears filter history (call on sample-rate/format changes)

private:
    struct BiquadCoeffs {
        double b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0;
    };
    struct BiquadState {
        double z1 = 0, z2 = 0; // per-channel history, kept separately for L/R
    };

    void recomputeCoeffs(int bandIndex);
    double processSample(double in, BiquadCoeffs &c, BiquadState &s);

    static constexpr int kBandCount = 3;
    Band m_bands[kBandCount];
    BiquadCoeffs m_coeffs[kBandCount];
    BiquadState m_stateLeft[kBandCount];
    BiquadState m_stateRight[kBandCount];

    double m_masterGainDb = 0.0;
    double m_masterGainLinear = 1.0;

    static constexpr double kSampleRate = 48000.0;
};
