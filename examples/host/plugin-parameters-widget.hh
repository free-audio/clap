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
      void     setData(int column, int role, const QVariant &value) override;

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
      void            addItem(ParamTreeItem *item);

      QVariant data(int column, int role) const override;
      void     setData(int column, int role, const QVariant &value) override;

   private:
      QString                          name_;
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

   void updateParamInfo();
   void updateParamValue();

   static const constexpr int SLIDER_RANGE = 10000;

   PluginHost &                                                pluginHost_;
   QTreeWidget *                                               treeWidget_ = nullptr;
   std::unordered_map<clap_id, std::unique_ptr<ParamTreeItem>> idToParamTreeItem_;
   ModuleTreeItem *                                            rootModuleItem_;
   PluginParam *                                               currentParam_ = nullptr;

   QLabel * idLabel_ = nullptr;
   QLabel * nameLabel_ = nullptr;
   QLabel * moduleLabel_ = nullptr;
   QLabel * isPerNoteLabel_ = nullptr;
   QLabel * isPerChannelLabel_ = nullptr;
   QLabel * isPeriodicLabel_ = nullptr;
   QLabel * isLockedLabel_ = nullptr;
   QLabel * isAutomatableLabel_ = nullptr;
   QLabel * isHiddenLabel_ = nullptr;
   QLabel * isBypassLabel_ = nullptr;
   QLabel * typeLabel_ = nullptr;
   QLabel * minValueLabel_ = nullptr;
   QLabel * maxValueLabel_ = nullptr;
   QLabel * defaultValueLabel_ = nullptr;
   QLabel * isBeingAdjusted_ = nullptr;
   QSlider *valueSlider_ = nullptr;
};
