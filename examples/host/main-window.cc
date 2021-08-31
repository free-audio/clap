#include <iostream>

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QToolBar>
#include <QWindow>

#include "application.hh"
#include "engine.hh"
#include "main-window.hh"
#include "plugin-host.hh"
#include "plugin-parameters-widget.hh"
#include "plugin-quick-controls-widget.hh"
#include "settings-dialog.hh"
#include "settings.hh"

MainWindow::MainWindow(Application &app)
   : QMainWindow(nullptr), application_(app),
     settingsDialog_(new SettingsDialog(application_.settings(), this)),
     pluginViewWindow_(new QWindow()),
     pluginViewWidget_(QWidget::createWindowContainer(pluginViewWindow_)) {

   createMenu();

   setCentralWidget(pluginViewWidget_);
   pluginViewWidget_->show();
   pluginViewWidget_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

   connect(settingsDialog_, SIGNAL(accepted()), &application_, SLOT(restartEngine()));

   auto &pluginHost = app.engine()->pluginHost();

   pluginParametersWindow_ = new QMainWindow(this);
   pluginParametersWidget_ = new PluginParametersWidget(pluginParametersWindow_, pluginHost);
   pluginParametersWindow_->setCentralWidget(pluginParametersWidget_);

   pluginQuickControlsWindow_ = new QMainWindow(this);
   pluginQuickControlsWidget_ =
      new PluginQuickControlsWidget(pluginQuickControlsWindow_, pluginHost);
   pluginQuickControlsWindow_->setCentralWidget(pluginQuickControlsWidget_);
}

MainWindow::~MainWindow() {}

void MainWindow::createMenu() {
   QMenuBar *menuBar = new QMenuBar(this);
   setMenuBar(menuBar);

   QMenu *fileMenu = menuBar->addMenu(tr("File"));
   fileMenu->addAction(tr("Load plugin"));
   connect(fileMenu->addAction(tr("Load Native Plugin Preset")),
           &QAction::triggered,
           this,
           &MainWindow::loadNativePluginPreset);
   fileMenu->addSeparator();
   connect(fileMenu->addAction(tr("Settings")),
           &QAction::triggered,
           this,
           &MainWindow::showSettingsDialog);
   fileMenu->addSeparator();
   connect(fileMenu->addAction(tr("Quit")),
           &QAction::triggered,
           QApplication::instance(),
           &Application::quit);

   auto windowsMenu = menuBar->addMenu("Windows");
   connect(windowsMenu->addAction(tr("Show Parameters")),
           &QAction::triggered,
           this,
           &MainWindow::showPluginParametersWindow);
   connect(windowsMenu->addAction(tr("Show Quick Controls")),
           &QAction::triggered,
           this,
           &MainWindow::showPluginQuickControlsWindow);
   menuBar->addSeparator();
   connect(windowsMenu->addAction(tr("Toggle Plugin Window Visibility")),
           &QAction::triggered,
           this,
           &MainWindow::togglePluginWindowVisibility);
   connect(windowsMenu->addAction(tr("Recreate Plugin Window")),
           &QAction::triggered,
           this,
           &MainWindow::recreatePluginWindow);
   connect(windowsMenu->addAction(tr("Scale Plugin Window")),
           &QAction::triggered,
           this,
           &MainWindow::scalePluginWindow);

   QMenu *helpMenu = menuBar->addMenu(tr("Help"));
   helpMenu->addAction(tr("Manual"));
   helpMenu->addAction(tr("About"));
}

void MainWindow::showSettingsDialog() {
   int result = settingsDialog_->exec();
   if (result == QDialog::Accepted)
      application_.restartEngine();
}

void MainWindow::showPluginParametersWindow() { pluginParametersWindow_->show(); }
void MainWindow::showPluginQuickControlsWindow() { pluginQuickControlsWindow_->show(); }

WId MainWindow::getEmbedWindowId() { return pluginViewWidget_->winId(); }

void MainWindow::resizePluginView(int width, int height) {
   pluginViewWidget_->setMinimumSize(width, height);
   pluginViewWidget_->setMaximumSize(width, height);
   pluginViewWidget_->show();
   adjustSize();
}

void MainWindow::loadNativePluginPreset()
{
   auto file = QFileDialog::getOpenFileName(this, tr("Load Plugin Native Preset"));
   if (file.isEmpty())
      return;

   application_.engine()->pluginHost().loadNativePluginPreset(file.toStdString());
}

void MainWindow::togglePluginWindowVisibility()
{
   bool isVisible = !pluginViewWidget_->isVisible();
   pluginViewWidget_->setVisible(isVisible);
   application_.engine()->pluginHost().setPluginWindowVisibility(isVisible);
}

void MainWindow::recreatePluginWindow()
{
   application_.engine()->pluginHost().setParentWindow(getEmbedWindowId());
}

void MainWindow::scalePluginWindow()
{
   // TODO
}