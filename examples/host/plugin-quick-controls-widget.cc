#include <QComboBox>
#include <QGridLayout>
#include <QVBoxLayout>

#include "plugin-host.hh"
#include "plugin-quick-control-widget.hh"
#include "plugin-quick-controls-widget.hh"

PluginQuickControlsWidget::PluginQuickControlsWidget(QWidget *parent, PluginHost &pluginHost)
   : QWidget(parent), pluginHost_(pluginHost) {
   chooser_ = new QComboBox(this);
   connect(chooser_,
           &QComboBox::activated,
           this,
           &PluginQuickControlsWidget::selectPageFromChooser);

   for (auto &qc : controls_)
      qc = new PluginQuickControlWidget(this, pluginHost);

   auto grid = new QGridLayout(this);
   grid->setSpacing(3);

   const auto rowSize = CLAP_QUICK_CONTROLS_COUNT / 2;
   for (int i = 0; i < CLAP_QUICK_CONTROLS_COUNT; ++i)
      grid->addWidget(controls_[i], i / rowSize, i % rowSize);

   auto vbox = new QVBoxLayout(this);
   vbox->addWidget(chooser_);
   vbox->addLayout(grid);
   vbox->setSpacing(3);
   setLayout(vbox);
}

void PluginQuickControlsWidget::pagesChanged() {
   auto &pages = pluginHost_.quickControlsPages();
   chooser_->clear();
   for (auto &it : pages)
      chooser_->addItem(it.second->name, it.second->id);

   selectedPageChanged();
}

void PluginQuickControlsWidget::selectedPageChanged() {
   auto  pageId = pluginHost_.quickControlsSelectedPage();
   auto &pages = pluginHost_.quickControlsPages();
   auto &params = pluginHost_.params();
   auto  it = pages.find(pageId);

   for (int i = 0; i < CLAP_QUICK_CONTROLS_COUNT; ++i) {
      PluginParam *param = nullptr;

      if (it != pages.end()) {
         auto paramId = it->second->param_ids[i];
         auto paramIt = params.find(paramId);
         if (paramIt != params.end())
            param = paramIt->second.get();
      }

      controls_[i]->setPluginParam(param);
   }
}

void PluginQuickControlsWidget::selectPageFromChooser(int index) {
    clap_id pageId = chooser_->currentData().toUInt();
    pluginHost_.setQuickControlsSelectedPageByHost(pageId);
}
