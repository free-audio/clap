#include <clap/all.h>

#include <cstring>
#include <exception>
#include <functional>
#include <sstream>
#include <vector>

#include "dc-offset/dc-offset.hh"
#include "gain/gain.hh"
#include "path-provider.hh"

struct PluginEntry {
   using create_func = std::function<const clap_plugin *(const clap_host *)>;

   PluginEntry(const clap_plugin_descriptor *d, create_func &&func)
      : desc(d), create(std::move(func)) {}

   const clap_plugin_descriptor *desc;
   std::function<const clap_plugin *(const clap_host *)> create;
};

static std::vector<PluginEntry> g_plugins;
static std::string g_pluginPath;

template <typename T>
static void addPlugin() {
   g_plugins.emplace_back(T::descriptor(), [](const clap_host *host) -> const clap_plugin * {
      auto plugin = new T(g_pluginPath, host);
      return plugin->clapPlugin();
   });
}

static bool clap_init(const char *plugin_path) {
   g_pluginPath = plugin_path;

   addPlugin<clap::Gain>();
   addPlugin<clap::DcOffset>();
   return true;
}

static void clap_deinit(void) {
   g_plugins.clear();
   g_pluginPath.clear();
}

static uint32_t clap_get_plugin_count(void) { return g_plugins.size(); }

static const clap_plugin_descriptor *clap_get_plugin_descriptor(uint32_t index) {
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

static uint32_t clap_get_invalidation_sources_count(void) { return 0; }

static const clap_plugin_invalidation_source *clap_get_invalidation_sources(uint32_t index) {
   return nullptr;
}

static void clap_refresh(void) {}

CLAP_EXPORT const struct clap_plugin_entry clap_plugin_entry = {
   CLAP_VERSION,
   clap_init,
   clap_deinit,
   clap_get_plugin_count,
   clap_get_plugin_descriptor,
   clap_create_plugin,
   clap_get_invalidation_sources_count,
   clap_get_invalidation_sources,
   clap_refresh,
};