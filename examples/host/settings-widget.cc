#include <QTabWidget>
#include <QVBoxLayout>

#include "audio-settings-widget.hh"
#include "midi-settings-widget.hh"
#include "settings-widget.hh"
#include "settings.hh"

SettingsWidget::SettingsWidget(Settings &settings) : _settings(settings) {
   QVBoxLayout *layout = new QVBoxLayout();

   _audioSettingsWidget = new AudioSettingsWidget(settings.audioSettings());
   layout->addWidget(_audioSettingsWidget);

   _midiSettingsWidget = new MidiSettingsWidget(settings.midiSettings());
   layout->addWidget(_midiSettingsWidget);

   setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
   setLayout(layout);
}
