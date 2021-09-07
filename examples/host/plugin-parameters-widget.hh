#pragma once

#include <QHash>
#include <QList>
#include <QTreeWidgetItem>
#include <QWidget>

#include <clap/clap.h>

class QTreeWidget;
class QTreeWidgetItem;
class PluginHost;
class PluginParam;
class QLabel;
class QDial;
class QSlider;

class PluginParametersWidget : public QWidget {
   Q_OBJECT
public:
   explicit PluginParametersWidget(QWidget *parent, PluginHost &pluginHost);

   class ModuleTreeItem;
   class ParamTreeItem : public QTreeWidgetItem {
   public:
      ParamTreeItem(ModuleTreeItem *parent, PluginParam &param);
      QVariant data(int column, int role) const override;
      void setData(int column, int role, const QVariant &value) override;

      auto &param() { return param_; }
      auto &param() const { return param_; }

   private:
      PluginParam &param_;
   };

   class ModuleTreeItem : public QTreeWidgetItem {
   public:
      ModuleTreeItem(QTreeWidget *parent);
      ModuleTreeItem(ModuleTreeItem *parent, const QString &name);

      void clear();

      ModuleTreeItem &subModule(const QString &name);
      void addItem(ParamTreeItem *item);

      QVariant data(int column, int role) const override;
      void setData(int column, int role, const QVariant &value) override;

   private:
      QString name_;
      QHash<QString, ModuleTreeItem *> modules_;
   };

signals:

private:
   void computeDataModel();
   void selectionChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

   void connectToParam(PluginParam *param);
   void disconnectFromParam();

   void paramInfoChanged();
   void paramValueChanged();
   void sliderValueChanged(int newValue);
   void sliderModulationChanged(int newValue);

   void updateAll();
   void updateParamInfo();
   void updateParamValue();
   void updateParamModulation();
   void updateParamIsBeingAjustedChanged();

   static const constexpr int SLIDER_RANGE = 10000;

   PluginHost &_pluginHost;
   QTreeWidget *_treeWidget = nullptr;
   std::unordered_map<clap_id, std::unique_ptr<ParamTreeItem>> _idToParamTreeItem;
   ModuleTreeItem *_rootModuleItem;
   PluginParam *_currentParam = nullptr;

   QLabel *_idLabel = nullptr;
   QLabel *_nameLabel = nullptr;
   QLabel *_moduleLabel = nullptr;
   QLabel *_isPerNoteLabel = nullptr;
   QLabel *_isPerChannelLabel = nullptr;
   QLabel *_isPeriodicLabel = nullptr;
   QLabel *_isReadOnlyLabel = nullptr;
   QLabel *_isHiddenLabel = nullptr;
   QLabel *_isBypassLabel = nullptr;
   QLabel *_isSteppedLabel = nullptr;
   QLabel *_minValueLabel = nullptr;
   QLabel *_maxValueLabel = nullptr;
   QLabel *_defaultValueLabel = nullptr;
   QLabel *_isBeingAdjusted = nullptr;
   QSlider *_valueSlider = nullptr;
   QSlider *_modulationSlider = nullptr;
};
