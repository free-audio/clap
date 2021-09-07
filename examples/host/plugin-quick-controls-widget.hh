#pragma once

#include <QWidget>

#include <clap/all.h>

class QComboBox;
class PluginHost;
class PluginQuickControlWidget;
class PluginQuickControlsWidget : public QWidget {
   Q_OBJECT;

public:
   PluginQuickControlsWidget(QWidget *parent, PluginHost &pluginHost);

   void pagesChanged();
   void selectedPageChanged();

private:
   void selectPageFromChooser(int index);

   PluginHost &_pluginHost;

   QComboBox *_chooser = nullptr;
   std::array<PluginQuickControlWidget *, CLAP_QUICK_CONTROLS_COUNT> _controls;
};