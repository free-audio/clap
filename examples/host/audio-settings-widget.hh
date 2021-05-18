#pragma once

#include <QWidget>

class AudioSettings;
class QComboBox;

class AudioSettingsWidget : public QWidget {
   Q_OBJECT
public:
   explicit AudioSettingsWidget(AudioSettings &audioSettings);

signals:

public slots:
   void selectedDeviceChanged(int index);
   void selectedSampleRateChanged(int index);
   void selectedBufferSizeChanged(int index);

private:
   AudioSettings &audioSettings_;
   QComboBox *    sampleRateWidget_;
   QComboBox *    bufferSizeWidget_;
};
