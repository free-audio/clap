#include <QComboBox>
#include <QGridLayout>
#include <QVBoxLayout>

#include "plugin-host.hh"
#include "plugin-quick-control-widget.hh"
#include "plugin-quick-controls-widget.hh"

PluginQuickControlsWidget::PluginQuickControlsWidget(QWidget *parent, PluginHost &pluginHost)
   : QWidget(parent), _pluginHost(pluginHost) {
   _chooser = new QComboBox(this);
   connect(_chooser,
           &QComboBox::activated,
           this,
           &PluginQuickControlsWidget::selectPageFromChooser);

   for (auto &qc : _controls)
      qc = new PluginQuickControlWidget(this, pluginHost);

   auto grid = new QGridLayout();
   grid->setSpacing(3);

   const auto rowSize = CLAP_QUICK_CONTROLS_COUNT / 2;
   for (int i = 0; i < CLAP_QUICK_CONTROLS_COUNT; ++i)
      grid->addWidget(_controls[i], i / rowSize, i % rowSize);

   auto vbox = new QVBoxLayout();
   vbox->addWidget(_chooser);
   vbox->addLayout(grid);
   vbox->setSpacing(3);
   setLayout(vbox);
}

void PluginQuickControlsWidget::pagesChanged() {
   auto &pages = _pluginHost.quickControlsPages();
   _chooser->clear();
   for (auto &it : pages)
      _chooser->addItem(it.second->name, it.second->id);

   selectedPageChanged();
}

void PluginQuickControlsWidget::selectedPageChanged() {
   auto  pageId = _pluginHost.quickControlsSelectedPage();
   auto &pages = _pluginHost.quickControlsPages();
   auto &params = _pluginHost.params();
   auto  it = pages.find(pageId);

   for (int i = 0; i < CLAP_QUICK_CONTROLS_COUNT; ++i) {
      PluginParam *param = nullptr;

      if (it != pages.end()) {
         auto paramId = it->second->param_ids[i];
         auto paramIt = params.find(paramId);
         if (paramIt != params.end())
            param = paramIt->second.get();
      }

      _controls[i]->setPluginParam(param);
   }
}

void PluginQuickControlsWidget::selectPageFromChooser(int index) {
    clap_id pageId = _chooser->currentData().toUInt();
    _pluginHost.setQuickControlsSelectedPageByHost(pageId);
}
