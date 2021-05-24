#include <clap/all.h>

#include <cstring>
#include <exception>
#include <functional>
#include <sstream>
#include <vector>

#include "gain/gain.hh"

struct PluginEntry {
   using create_func = std::function<const clap_plugin *(const clap_host *)>;

   PluginEntry(const clap_plugin_descriptor *d, create_func &&func)
      : desc(d), create(std::move(func)) {}

   const clap_plugin_descriptor *            desc;
   std::function<const clap_plugin *(const clap_host *)> create;
};

static std::vector<PluginEntry> g_plugins;

template <typename T>
static void addPlugin() {
   g_plugins.emplace_back(T::descriptor(), [](const clap_host *host) -> const clap_plugin * {
      auto plugin = new T(host);
      return plugin->clapPlugin();
   });
}

static bool clap_init(const char *plugin_path) {
   addPlugin<clap::Gain>();
   return true;
}

static void clap_deinit(void) { g_plugins.clear(); }

static int32_t clap_get_plugin_count(void) { return g_plugins.size(); }

static const clap_plugin_descriptor *clap_get_plugin_descriptor(int32_t index) {
   if (index < 0 || index >= g_plugins.size()) {
      std::ostringstream msg;
      msg << "index out of bounds: " << index << " not in 0.." << g_plugins.size();
      throw std::invalid_argument(msg.str());
   }

   return g_plugins[index].desc;
}

static const clap_plugin *clap_create_plugin(const clap_host *host, const char *plugin_id) {
   for (auto &entry : g_plugins)
      if (!strcmp(entry.desc->id, plugin_id))
         return entry.create(host);
   return nullptr;
}

CLAP_EXPORT const struct clap_plugin_entry clap_plugin_entry = {
   clap_init,
   clap_deinit,
   clap_get_plugin_count,
   clap_get_plugin_descriptor,
   clap_create_plugin,
};