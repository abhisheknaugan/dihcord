#pragma once

#include <QDialog>
#include <functional>

#include "voiceclient.h"

class QSlider;
class QLabel;
class QVBoxLayout;

// All sliders act live - moving one immediately changes the corresponding
// value on the AudioEngine inside the given VoiceClient. No "Apply"/"OK"
// step needed; closing the dialog just closes it, values stay changed.
class VoiceSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VoiceSettingsDialog(VoiceClient *voiceClient, QWidget *parent = nullptr);

private:
    QSlider *addSlider(QVBoxLayout *layout, const QString &labelText,
                        int minValue, int maxValue, int defaultValue,
                        std::function<void(int)> onChange);

    VoiceClient *m_voiceClient;
};
