#pragma once

#include <QString>

class PluginInfo {
public:
   PluginInfo();

private:
   QString name_;
   QString file_;
   QString index_; // in case of shell plugin
};
