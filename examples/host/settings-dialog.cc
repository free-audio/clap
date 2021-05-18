#include <QDialogButtonBox>
#include <QVBoxLayout>

#include "settings-widget.hh"
#include "settings.hh"

#include "settings-dialog.hh"

SettingsDialog::SettingsDialog(Settings &settings, QWidget *parent)
   : QDialog(parent), settings_(settings) {
   setModal(true);
   setWindowTitle(tr("Settings"));

   QVBoxLayout *vbox = new QVBoxLayout();
   settingsWidget_ = new SettingsWidget(settings);
   vbox->addWidget(settingsWidget_);

   auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
   buttons->show();
   vbox->addWidget(buttons);
   connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
   connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

   setLayout(vbox);

   setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}
