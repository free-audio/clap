#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QSlider>
#include <QSplitter>
#include <QTreeWidget>

#include "plugin-host.hh"
#include "plugin-param.hh"
#include "plugin-parameters-widget.hh"

///////////////////
// ParamTreeItem //
///////////////////

PluginParametersWidget::ParamTreeItem::ParamTreeItem(ModuleTreeItem *parent, PluginParam &param)
   : QTreeWidgetItem(parent), param_(param) {}

QVariant PluginParametersWidget::ParamTreeItem::data(int column, int role) const {
   if (column == 0 && role == Qt::DisplayRole)
      return param_.info().name;
   return {};
}

void PluginParametersWidget::ParamTreeItem::setData(int column, int role, const QVariant &value) {
   // nothing to do, read-only
}

////////////////////
// ModuleTreeItem //
////////////////////

PluginParametersWidget::ModuleTreeItem::ModuleTreeItem(QTreeWidget *parent)
   : QTreeWidgetItem(parent), name_("/") {}

PluginParametersWidget::ModuleTreeItem::ModuleTreeItem(ModuleTreeItem *parent, const QString &name)
   : QTreeWidgetItem(parent), name_(name) {}

void PluginParametersWidget::ModuleTreeItem::clear() {
   while (childCount() > 0)
      removeChild(child(0));
   modules_.clear();
}

PluginParametersWidget::ModuleTreeItem &
PluginParametersWidget::ModuleTreeItem::subModule(const QString &name) {
   auto it = modules_.find(name);
   if (it != modules_.end())
      return *it.value();
   auto module = new ModuleTreeItem(this, name);
   addChild(module);
   modules_.insert(name, module);
   return *module;
}

void PluginParametersWidget::ModuleTreeItem::addItem(ParamTreeItem *item) { addChild(item); }

QVariant PluginParametersWidget::ModuleTreeItem::data(int column, int role) const {
   if (column == 0 && role == Qt::DisplayRole)
      return name_;
   return {};
}

void PluginParametersWidget::ModuleTreeItem::setData(int column, int role, const QVariant &value) {
   // nothing to do, read-only
}

////////////////////////////
// PluginParametersWidget //
////////////////////////////

PluginParametersWidget::PluginParametersWidget(QWidget *parent, PluginHost &pluginHost)
   : QWidget(parent), pluginHost_(pluginHost) {

   treeWidget_ = new QTreeWidget(this);

   // Tree
   rootModuleItem_ = new ModuleTreeItem(treeWidget_);
   treeWidget_->addTopLevelItem(rootModuleItem_);
   treeWidget_->setHeaderHidden(true);
   treeWidget_->setAnimated(true);
   treeWidget_->setRootIsDecorated(true);
   treeWidget_->setSelectionMode(QAbstractItemView::SingleSelection);
   treeWidget_->setSelectionBehavior(QAbstractItemView::SelectItems);
   treeWidget_->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
   connect(
      &pluginHost_, &PluginHost::paramsChanged, this, &PluginParametersWidget::computeDataModel);
   connect(treeWidget_,
           &QTreeWidget::currentItemChanged,
           this,
           &PluginParametersWidget::selectionChanged);

   // Info
   auto infoWidget = new QFrame(this);
   infoWidget->setLineWidth(1);
   infoWidget->setMidLineWidth(1);
   infoWidget->setFrameShape(QFrame::StyledPanel);
   infoWidget->setFrameShadow(QFrame::Sunken);

   idLabel_ = new QLabel;
   nameLabel_ = new QLabel;
   moduleLabel_ = new QLabel;
   isPerNoteLabel_ = new QLabel;
   isPerChannelLabel_ = new QLabel;
   isPeriodicLabel_ = new QLabel;
   isLockedLabel_ = new QLabel;
   isAutomatableLabel_ = new QLabel;
   isHiddenLabel_ = new QLabel;
   isBypassLabel_ = new QLabel;
   typeLabel_ = new QLabel;
   minValueLabel_ = new QLabel;
   maxValueLabel_ = new QLabel;
   defaultValueLabel_ = new QLabel;
   isBeingAdjusted_ = new QLabel;
   valueSlider_ = new QSlider;
   valueSlider_->setMinimum(0);
   valueSlider_->setMaximum(SLIDER_RANGE);
   valueSlider_->setOrientation(Qt::Horizontal);
   connect(valueSlider_, &QSlider::valueChanged, this, &PluginParametersWidget::sliderValueChanged);

   auto formLayout = new QFormLayout(infoWidget);
   formLayout->addRow(tr("id"), idLabel_);
   formLayout->addRow(tr("name"), nameLabel_);
   formLayout->addRow(tr("module"), moduleLabel_);
   formLayout->addRow(tr("is_per_note"), isPerNoteLabel_);
   formLayout->addRow(tr("is_per_channel"), isPerChannelLabel_);
   formLayout->addRow(tr("is_periodic"), isPeriodicLabel_);
   formLayout->addRow(tr("is_locked"), isLockedLabel_);
   formLayout->addRow(tr("is_automatable"), isAutomatableLabel_);
   formLayout->addRow(tr("is_hidden"), isHiddenLabel_);
   formLayout->addRow(tr("is_bypass"), isBypassLabel_);
   formLayout->addRow(tr("type"), typeLabel_);
   formLayout->addRow(tr("min_value"), minValueLabel_);
   formLayout->addRow(tr("max_value"), maxValueLabel_);
   formLayout->addRow(tr("default_value"), defaultValueLabel_);
   formLayout->addRow(tr("is_being_adjusted"), isBeingAdjusted_);
   formLayout->addRow(tr("value"), valueSlider_);

   infoWidget->setLayout(formLayout);

   // Splitter
   auto splitter = new QSplitter();
   splitter->addWidget(treeWidget_);
   splitter->addWidget(infoWidget);

   auto layout = new QHBoxLayout(this);
   layout->addWidget(splitter);
   setLayout(layout);

   computeDataModel();
   updateParamInfo();
}

void PluginParametersWidget::computeDataModel() {
   rootModuleItem_->clear();
   idToParamTreeItem_.clear();

   for (auto &it : pluginHost_.params()) {
      auto &param = *it.second;

      QString path(param.info().module);
      auto    modules = path.split("/", Qt::SkipEmptyParts);
      auto    module = rootModuleItem_;
      for (auto &m : modules)
         module = &module->subModule(m);

      auto item = std::make_unique<ParamTreeItem>(module, param);
      idToParamTreeItem_.emplace(param.info().id, std::move(item));
   }
   treeWidget_->sortItems(0, Qt::AscendingOrder);
}

void PluginParametersWidget::selectionChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
   if (!current) {
      disconnectFromParam();
      return;
   }

   auto module = dynamic_cast<ModuleTreeItem *>(current);
   if (module) {
      disconnectFromParam();
      return;
   }

   auto item = dynamic_cast<ParamTreeItem *>(current);
   if (item) {
      connectToParam(&item->param());
      return;
   }
}

void PluginParametersWidget::connectToParam(PluginParam *param) {
   if (currentParam_)
      disconnectFromParam();

   currentParam_ = param;
   connect(param, &PluginParam::infoChanged, this, &PluginParametersWidget::paramInfoChanged);
   connect(param, &PluginParam::valueChanged, this, &PluginParametersWidget::paramValueChanged);

   updateParamInfo();
   updateParamValue();
}

void PluginParametersWidget::disconnectFromParam() {
   if (!currentParam_)
      return;

   disconnect(
      currentParam_, &PluginParam::infoChanged, this, &PluginParametersWidget::paramInfoChanged);
   disconnect(
      currentParam_, &PluginParam::valueChanged, this, &PluginParametersWidget::paramValueChanged);
   updateParamInfo();
}

void PluginParametersWidget::updateParamInfo() {
   if (!currentParam_) {
      idLabel_->setText("-");
      nameLabel_->setText("-");
      moduleLabel_->setText("-");
      isPerNoteLabel_->setText("-");
      isPerChannelLabel_->setText("-");
      isPeriodicLabel_->setText("-");
      isLockedLabel_->setText("-");
      isAutomatableLabel_->setText("-");
      isHiddenLabel_->setText("-");
      isBypassLabel_->setText("-");
      typeLabel_->setText("-");
      minValueLabel_->setText("-");
      maxValueLabel_->setText("-");
      defaultValueLabel_->setText("-");
      isBeingAdjusted_->setText("-");
   } else {
      auto &p = *currentParam_;
      auto &i = p.info();
      idLabel_->setText(QString::number(i.id));
      nameLabel_->setText(i.name);
      moduleLabel_->setText(i.module);
      isPerNoteLabel_->setText(i.is_per_note ? "true" : "false");
      isPerChannelLabel_->setText(i.is_per_channel ? "true" : "false");
      isPeriodicLabel_->setText(i.is_periodic ? "true" : "false");
      isLockedLabel_->setText(i.is_locked ? "true" : "false");
      isAutomatableLabel_->setText(i.is_automatable ? "true" : "false");
      isHiddenLabel_->setText(i.is_hidden ? "true" : "false");
      isBypassLabel_->setText(i.is_bypass ? "true" : "false");
      isBeingAdjusted_->setText(p.isBeingAdjusted() ? "true" : "false");

      switch (i.type) {
      case CLAP_PARAM_BOOL:
         typeLabel_->setText("bool");
         minValueLabel_->setText("-");
         maxValueLabel_->setText("-");
         defaultValueLabel_->setText(i.default_value.b ? "true" : "false");
         break;

      case CLAP_PARAM_INT:
         typeLabel_->setText("int");
         minValueLabel_->setText(QString::number(i.min_value.i));
         maxValueLabel_->setText(QString::number(i.max_value.i));
         defaultValueLabel_->setText(QString::number(i.default_value.i));
         break;

      case CLAP_PARAM_ENUM:
         typeLabel_->setText("enum");
         minValueLabel_->setText("-");
         maxValueLabel_->setText("-");
         defaultValueLabel_->setText(QString::number(i.default_value.i));
         break;

      case CLAP_PARAM_FLOAT:
         typeLabel_->setText("float");
         minValueLabel_->setText(QString::number(i.min_value.d));
         maxValueLabel_->setText(QString::number(i.max_value.d));
         defaultValueLabel_->setText(QString::number(i.default_value.d));
         break;
      }
   }
}

void PluginParametersWidget::updateParamValue() {
   if (valueSlider_->isSliderDown())
      return;

   if (!currentParam_)
      return;

   auto info = currentParam_->info();
   auto v = currentParam_->value();
   switch (info.type) {
   case CLAP_PARAM_FLOAT:
      valueSlider_->setValue(SLIDER_RANGE * (v.d - info.min_value.d) /
                             (info.max_value.d - info.min_value.d));
      break;
   case CLAP_PARAM_INT:
      valueSlider_->setValue((SLIDER_RANGE * (v.i - info.min_value.i)) /
                             (info.max_value.i - info.min_value.i));
      break;
   }
}

void PluginParametersWidget::paramInfoChanged() { updateParamInfo(); }

void PluginParametersWidget::paramValueChanged() { updateParamValue(); }

void PluginParametersWidget::sliderValueChanged(int newValue) {
   if (!currentParam_)
      return;

   if (!valueSlider_->isSliderDown())
      return;

   auto &info = currentParam_->info();

   clap_param_value value;
   switch (info.type) {
   case CLAP_PARAM_FLOAT:
      value.d = newValue * (info.max_value.d - info.min_value.d) / SLIDER_RANGE + info.min_value.d;
      pluginHost_.setParamValueByHost(*currentParam_, value);
      break;
   case CLAP_PARAM_INT:
      value.i =
         (newValue * (info.max_value.i - info.min_value.i)) / SLIDER_RANGE + info.min_value.i;
      pluginHost_.setParamValueByHost(*currentParam_, value);
      break;
   }
}