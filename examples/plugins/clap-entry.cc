#include <clap/all.h>

#include <exception>
#include <sstream>
#include <vector>

#include "gain/gain.hh"

static std::vector<const clap_plugin_descriptor *> g_plugins;

static bool clap_init(const char *plugin_path) {
   g_plugins.push_back(&g_gain_plugin_descriptor);
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

   return g_plugins[index];
}

static clap_plugin *clap_create_plugin(clap_host *host, const char *plugin_id) { return nullptr; }

CLAP_EXPORT const struct clap_plugin_entry clap_plugin_entry = {
   clap_init,
   clap_deinit,
   clap_get_plugin_count,
   clap_get_plugin_descriptor,
   clap_create_plugin,
};