#pragma once

#include <QMainWindow>

class Application;
class SettingsDialog;
class PluginParametersWidget;
class PluginQuickControlsWidget;

class MainWindow : public QMainWindow {
   Q_OBJECT

public:
   explicit MainWindow(Application &app);
   ~MainWindow();

   WId getEmbedWindowId();

public:
   void loadNativePluginPreset();
   void showSettingsDialog();
   void showPluginParametersWindow();
   void showPluginQuickControlsWindow();
   void resizePluginView(int width, int height);

private:
   void createMenu();

   void togglePluginWindowVisibility();
   void recreatePluginWindow();
   void scalePluginWindow();

   Application &   _application;
   SettingsDialog *_settingsDialog = nullptr;
   QWindow *       _pluginViewWindow = nullptr;
   QWidget *       _pluginViewWidget = nullptr;

   QMainWindow *           _pluginParametersWindow = nullptr;
   PluginParametersWidget *_pluginParametersWidget = nullptr;

   QMainWindow *              _pluginQuickControlsWindow = nullptr;
   PluginQuickControlsWidget *_pluginQuickControlsWidget = nullptr;
};
