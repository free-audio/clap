#include <clap/clap.h>

static const clap_plugin_descriptor_t g_plugin_desc = {
   .id = "com.your-company.YourPlugin",
   .name = "Plugin Name",
   .vendor = "Vendor",
   .url = "https://your-domain.com/your-plugin",
   .manual_url = "https://your-domain.com/your-plugin/manual",
   .support_url = "https://your-domain.com/support",
   .version = "1.4.2",
   .description = "This is a spectral synthesizer made to blow your mind.",
   .features = (const char*[]){
      "instrument", "additive", "surround"
   },
};

static bool entry_init(const char *plugin_path)
{
   // called only once, and first
   return true;
}

static void entry_deinit(void)
{
   // called last
}

static const void *entry_get_factory(const char *factory_id)
{
   return NULL;
}

CLAP_EXPORT const clap_plugin_entry_t clap_entry = {
   .clap_version = CLAP_VERSION,
   .init = entry_init,
   .deinit = entry_deinit,
   .get_factory = entry_get_factory,
};
