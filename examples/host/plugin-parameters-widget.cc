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
   : QWidget(parent), _pluginHost(pluginHost) {

   _treeWidget = new QTreeWidget(this);

   // Tree
   _rootModuleItem = new ModuleTreeItem(_treeWidget);
   _treeWidget->addTopLevelItem(_rootModuleItem);
   _treeWidget->setHeaderHidden(true);
   _treeWidget->setAnimated(true);
   _treeWidget->setRootIsDecorated(true);
   _treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
   _treeWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
   _treeWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
   connect(
      &_pluginHost, &PluginHost::paramsChanged, this, &PluginParametersWidget::computeDataModel);
   connect(_treeWidget,
           &QTreeWidget::currentItemChanged,
           this,
           &PluginParametersWidget::selectionChanged);

   // Info
   auto infoWidget = new QFrame(this);
   infoWidget->setLineWidth(1);
   infoWidget->setMidLineWidth(1);
   infoWidget->setFrameShape(QFrame::StyledPanel);
   infoWidget->setFrameShadow(QFrame::Sunken);

   _idLabel = new QLabel;
   _nameLabel = new QLabel;
   _moduleLabel = new QLabel;
   _isPerNoteLabel = new QLabel;
   _isPerChannelLabel = new QLabel;
   _isPeriodicLabel = new QLabel;
   _isReadOnlyLabel = new QLabel;
   _isHiddenLabel = new QLabel;
   _isBypassLabel = new QLabel;
   _isSteppedLabel = new QLabel;
   _minValueLabel = new QLabel;
   _maxValueLabel = new QLabel;
   _defaultValueLabel = new QLabel;
   _isBeingAdjusted = new QLabel;
   _valueLabel = new QLabel;

   _valueSlider = new QSlider;
   _valueSlider->setMinimum(0);
   _valueSlider->setMaximum(SLIDER_RANGE);
   _valueSlider->setOrientation(Qt::Horizontal);
   connect(_valueSlider, &QSlider::valueChanged, this, &PluginParametersWidget::sliderValueChanged);

   _modulationSlider = new QSlider;
   _modulationSlider->setMinimum(-SLIDER_RANGE);
   _modulationSlider->setMaximum(SLIDER_RANGE);
   _modulationSlider->setOrientation(Qt::Horizontal);
   connect(_modulationSlider, &QSlider::valueChanged, this, &PluginParametersWidget::sliderModulationChanged);

   auto formLayout = new QFormLayout(infoWidget);
   formLayout->addRow(tr("id"), _idLabel);
   formLayout->addRow(tr("name"), _nameLabel);
   formLayout->addRow(tr("module"), _moduleLabel);
   formLayout->addRow(tr("is_per_note"), _isPerNoteLabel);
   formLayout->addRow(tr("is_per_channel"), _isPerChannelLabel);
   formLayout->addRow(tr("is_periodic"), _isPeriodicLabel);
   formLayout->addRow(tr("is_read_only"), _isReadOnlyLabel);
   formLayout->addRow(tr("is_hidden"), _isHiddenLabel);
   formLayout->addRow(tr("is_bypass"), _isBypassLabel);
   formLayout->addRow(tr("is_stepped"), _isSteppedLabel);
   formLayout->addRow(tr("min_value"), _minValueLabel);
   formLayout->addRow(tr("max_value"), _maxValueLabel);
   formLayout->addRow(tr("default_value"), _defaultValueLabel);
   formLayout->addRow(tr("is_being_adjusted"), _isBeingAdjusted);
   formLayout->addRow(tr("value"), _valueLabel);
   formLayout->addRow(tr("value"), _valueSlider);
   formLayout->addRow(tr("modulation"), _modulationSlider);

   infoWidget->setLayout(formLayout);

   // Splitter
   auto splitter = new QSplitter();
   splitter->addWidget(_treeWidget);
   splitter->addWidget(infoWidget);

   auto layout = new QHBoxLayout(this);
   layout->addWidget(splitter);
   setLayout(layout);

   computeDataModel();
   updateParamInfo();
}

void PluginParametersWidget::computeDataModel() {
   _rootModuleItem->clear();
   _idToParamTreeItem.clear();

   for (auto &it : _pluginHost.params()) {
      auto &param = *it.second;

      QString path(param.info().module);
      auto modules = path.split("/", Qt::SkipEmptyParts);
      auto module = _rootModuleItem;
      for (auto &m : modules)
         module = &module->subModule(m);

      auto item = std::make_unique<ParamTreeItem>(module, param);
      _idToParamTreeItem.emplace(param.info().id, std::move(item));
   }
   _treeWidget->sortItems(0, Qt::AscendingOrder);
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
   if (_currentParam)
      disconnectFromParam();

   _currentParam = param;
   connect(param, &PluginParam::infoChanged, this, &PluginParametersWidget::paramInfoChanged);
   connect(param, &PluginParam::valueChanged, this, &PluginParametersWidget::paramValueChanged);
   connect(param,
           &PluginParam::isBeingAdjustedChanged,
           this,
           &PluginParametersWidget::updateParamIsBeingAjustedChanged);

   updateAll();
}

void PluginParametersWidget::disconnectFromParam() {
   if (!_currentParam)
      return;

   disconnect(
      _currentParam, &PluginParam::infoChanged, this, &PluginParametersWidget::paramInfoChanged);
   disconnect(
      _currentParam, &PluginParam::valueChanged, this, &PluginParametersWidget::paramValueChanged);
   disconnect(_currentParam,
              &PluginParam::isBeingAdjustedChanged,
              this,
              &PluginParametersWidget::updateParamIsBeingAjustedChanged);

   _currentParam = nullptr;

   updateAll();
}

void PluginParametersWidget::updateAll() {
   updateParamInfo();
   updateParamValue();
   updateParamModulation();
   updateParamIsBeingAjustedChanged();
}

void PluginParametersWidget::updateParamInfo() {
   if (!_currentParam) {
      _idLabel->setText("-");
      _nameLabel->setText("-");
      _moduleLabel->setText("-");
      _isPerNoteLabel->setText("-");
      _isPerChannelLabel->setText("-");
      _isPeriodicLabel->setText("-");
      _isReadOnlyLabel->setText("-");
      _isHiddenLabel->setText("-");
      _isBypassLabel->setText("-");
      _isSteppedLabel->setText("-");
      _minValueLabel->setText("-");
      _maxValueLabel->setText("-");
      _defaultValueLabel->setText("-");
      _isBeingAdjusted->setText("-");
      _valueLabel->setText("-");
   } else {
      auto &p = *_currentParam;
      auto &i = p.info();
      _idLabel->setText(QString::number(i.id));
      _nameLabel->setText(i.name);
      _moduleLabel->setText(i.module);
      _isPerNoteLabel->setText(i.flags & CLAP_PARAM_IS_PER_NOTE ? "true" : "false");
      _isPerChannelLabel->setText(i.flags & CLAP_PARAM_IS_PER_CHANNEL ? "true" : "false");
      _isPeriodicLabel->setText(i.flags & CLAP_PARAM_IS_PERIODIC ? "true" : "false");
      _isReadOnlyLabel->setText(i.flags & CLAP_PARAM_IS_READONLY ? "true" : "false");
      _isHiddenLabel->setText(i.flags & CLAP_PARAM_IS_HIDDEN ? "true" : "false");
      _isBypassLabel->setText(i.flags & CLAP_PARAM_IS_BYPASS ? "true" : "false");
      _isBeingAdjusted->setText(p.isBeingAdjusted() ? "true" : "false");

      _isSteppedLabel->setText("float");
      _minValueLabel->setText(QString::number(i.min_value));
      _maxValueLabel->setText(QString::number(i.max_value));
      _defaultValueLabel->setText(QString::number(i.default_value));
   }
}

void PluginParametersWidget::updateParamIsBeingAjustedChanged() {
   if (!_currentParam) {
      _isBeingAdjusted->setText("-");
   } else {
      auto &p = *_currentParam;
      _isBeingAdjusted->setText(p.isBeingAdjusted() ? "true" : "false");
   }
}

void PluginParametersWidget::updateParamValue() {
   if (_valueSlider->isSliderDown())
      return;

   if (!_currentParam)
      return;

   auto info = _currentParam->info();
   auto v = _currentParam->value();
   _valueSlider->setValue(SLIDER_RANGE * (v - info.min_value) / (info.max_value - info.min_value));
   updateParamValueText();
}

void PluginParametersWidget::updateParamValueText()
{
   _valueLabel->setText(_pluginHost.paramValueToText(_currentParam->info().id, _currentParam->value()));
}

void PluginParametersWidget::updateParamModulation() {
   if (_valueSlider->isSliderDown())
      return;

   if (!_currentParam)
      return;

   auto info = _currentParam->info();
   auto v = _currentParam->value();
   _valueSlider->setValue(SLIDER_RANGE * (v - info.min_value) / (info.max_value - info.min_value));
}

void PluginParametersWidget::paramInfoChanged() { updateParamInfo(); }

void PluginParametersWidget::paramValueChanged() { updateParamValue(); }

void PluginParametersWidget::sliderValueChanged(int newValue) {
   if (!_currentParam)
      return;

   if (!_valueSlider->isSliderDown())
      return;

   auto &info = _currentParam->info();

   double value = newValue * (info.max_value - info.min_value) / SLIDER_RANGE + info.min_value;
   _pluginHost.setParamValueByHost(*_currentParam, value);
   updateParamValueText();
}

void PluginParametersWidget::sliderModulationChanged(int newValue) {
   if (!_currentParam)
      return;

   if (!_modulationSlider->isSliderDown())
      return;

   auto &info = _currentParam->info();

   double dist = info.max_value - info.min_value;
   double value = newValue * dist / SLIDER_RANGE;
   _pluginHost.setParamModulationByHost(*_currentParam, value);
}