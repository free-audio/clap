#include <QTabWidget>
#include <QVBoxLayout>

#include "audio-settings-widget.hh"
#include "midi-settings-widget.hh"
#include "settings-widget.hh"
#include "settings.hh"

SettingsWidget::SettingsWidget(Settings &settings) : settings_(settings) {
   QVBoxLayout *layout = new QVBoxLayout();

   audioSettingsWidget_ = new AudioSettingsWidget(settings.audioSettings());
   layout->addWidget(audioSettingsWidget_);

   midiSettingsWidget_ = new MidiSettingsWidget(settings.midiSettings());
   layout->addWidget(midiSettingsWidget_);

   setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
   setLayout(layout);
}
