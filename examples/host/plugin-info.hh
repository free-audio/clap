#pragma once

#include <QString>

class PluginInfo {
public:
   PluginInfo() = default;

private:
   QString _name;
   QString _file;
   QString _index; // in case of shell plugin
};
