#include "voicesettingsdialog.h"

#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>

VoiceSettingsDialog::VoiceSettingsDialog(VoiceClient *voiceClient, QWidget *parent)
    : QDialog(parent)
    , m_voiceClient(voiceClient)
{
    setWindowTitle("Voice Settings");
    setMinimumWidth(360);

    auto *mainLayout = new QVBoxLayout(this);

    // --- Microphone section (shapes how OTHERS hear you) ---
    auto *micGroup = new QGroupBox("Microphone (how others hear you)");
    auto *micLayout = new QVBoxLayout(micGroup);

    addSlider(micLayout, "Mic Volume", -24, 24, 0, [this](int v) {
        m_voiceClient->audioEngine().setMicGainDb(v);
    });
    addSlider(micLayout, "Bass", -18, 18, 0, [this](int v) {
        m_voiceClient->audioEngine().micEqualizer().setBandGain(0, v);
    });
    addSlider(micLayout, "Mid", -18, 18, 0, [this](int v) {
        m_voiceClient->audioEngine().micEqualizer().setBandGain(1, v);
    });
    addSlider(micLayout, "Treble", -18, 18, 0, [this](int v) {
        m_voiceClient->audioEngine().micEqualizer().setBandGain(2, v);
    });

    mainLayout->addWidget(micGroup);

    // --- Output section (shapes what YOU hear from others) ---
    auto *outputGroup = new QGroupBox("Output (what you hear)");
    auto *outputLayout = new QVBoxLayout(outputGroup);

    addSlider(outputLayout, "Output Volume", -24, 24, 0, [this](int v) {
        m_voiceClient->audioEngine().setOutputGainDb(v);
    });
    addSlider(outputLayout, "Bass", -18, 18, 0, [this](int v) {
        m_voiceClient->audioEngine().outputEqualizer().setBandGain(0, v);
    });
    addSlider(outputLayout, "Mid", -18, 18, 0, [this](int v) {
        m_voiceClient->audioEngine().outputEqualizer().setBandGain(1, v);
    });
    addSlider(outputLayout, "Treble", -18, 18, 0, [this](int v) {
        m_voiceClient->audioEngine().outputEqualizer().setBandGain(2, v);
    });

    mainLayout->addWidget(outputGroup);
}

QSlider *VoiceSettingsDialog::addSlider(QVBoxLayout *layout, const QString &labelText,
                                         int minValue, int maxValue, int defaultValue,
                                         std::function<void(int)> onChange)
{
    auto *label = new QLabel(QString("%1 (%2)").arg(labelText).arg(defaultValue));
    layout->addWidget(label);

    auto *slider = new QSlider(Qt::Horizontal);
    slider->setMinimum(minValue);
    slider->setMaximum(maxValue);
    slider->setValue(defaultValue);
    layout->addWidget(slider);

    connect(slider, &QSlider::valueChanged, this, [label, labelText, onChange](int value) {
        label->setText(QString("%1 (%2)").arg(labelText).arg(value));
        onChange(value);
    });

    return slider;
}
