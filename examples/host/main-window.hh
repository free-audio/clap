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

   Application &   application_;
   SettingsDialog *settingsDialog_ = nullptr;
   QWindow *       pluginViewWindow_ = nullptr;
   QWidget *       pluginViewWidget_ = nullptr;

   QMainWindow *           pluginParametersWindow_ = nullptr;
   PluginParametersWidget *pluginParametersWidget_ = nullptr;

   QMainWindow *              pluginQuickControlsWindow_ = nullptr;
   PluginQuickControlsWidget *pluginQuickControlsWidget_ = nullptr;
};
