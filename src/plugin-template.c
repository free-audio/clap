#include <string.h>
#include <stdlib.h>

#include <clap/clap.h>

static const clap_plugin_descriptor_t s_my_plug_desc = {
   .id = "com.your-company.YourPlugin",
   .name = "Plugin Name",
   .vendor = "Vendor",
   .url = "https://your-domain.com/your-plugin",
   .manual_url = "https://your-domain.com/your-plugin/manual",
   .support_url = "https://your-domain.com/support",
   .version = "1.4.2",
   .description = "This is a spectral synthesizer made to blow your mind.",
   .features = (const char *[]){"instrument", "additive", "surround"},
};

typedef struct {
   clap_plugin_t      plugin;
   const clap_host_t *host;
} my_plug_t;

/////////////////
// clap_plugin //
/////////////////

static bool my_plug_init(const struct clap_plugin *plugin) { return true; }

static void my_plug_destroy(const struct clap_plugin *plugin) {}

static bool my_plug_activate(const struct clap_plugin *plugin,
                             double                    sample_rate,
                             uint32_t                  min_frames_count,
                             uint32_t                  max_frames_count) {
   return true;
}

static void my_plug_deactivate(const struct clap_plugin *plugin) {}

static bool my_plug_start_processing(const struct clap_plugin *plugin) { return true; }

static void my_plug_stop_processing(const struct clap_plugin *plugin) {}

static void my_plug_reset(const struct clap_plugin *plugin) {}

static clap_process_status my_plug_process(const struct clap_plugin *plugin,
                                           const clap_process_t     *process) {
   return CLAP_PROCESS_CONTINUE;
}

static const void *my_plug_get_extension(const struct clap_plugin *plugin, const char *id) {
   return NULL;
}

static void my_plug_on_main_thread(const struct clap_plugin *plugin) {}

clap_plugin_t *my_plug_create(const clap_host_t *host) {
   my_plug_t *p = calloc(1, sizeof(*p));
   p->host = host;
   p->plugin.desc = &s_my_plug_desc;
   p->plugin.plugin_data = p;
   p->plugin.init = my_plug_init;
   p->plugin.destroy = my_plug_destroy;
   p->plugin.activate = my_plug_activate;
   p->plugin.deactivate = my_plug_deactivate;
   p->plugin.start_processing = my_plug_start_processing;
   p->plugin.stop_processing = my_plug_stop_processing;
   p->plugin.reset = my_plug_reset;
   p->plugin.process = my_plug_process;
   p->plugin.get_extension = my_plug_get_extension;
   p->plugin.on_main_thread = my_plug_on_main_thread;

   return &p->plugin;
}

/////////////////////////
// clap_plugin_factory //
/////////////////////////

static struct {
   const clap_plugin_descriptor_t *desc;
   clap_plugin_t *(*create)(const clap_host_t *host);
} s_plugins[] = {
   {
      .desc = &s_my_plug_desc,
      .create = my_plug_create,
   },
};

static uint32_t plugin_factory_get_plugin_count(const struct clap_plugin_factory *factory) {
   return sizeof(s_plugins) / sizeof(s_plugins[0]);
}

static const clap_plugin_descriptor_t *
plugin_factory_get_plugin_descriptor(const struct clap_plugin_factory *factory, uint32_t index) {
   return s_plugins[index].desc;
}

static const clap_plugin_t *plugin_factory_create_plugin(const struct clap_plugin_factory *factory,
                                                         const clap_host_t                *host,
                                                         const char *plugin_id) {
   int N = sizeof(s_plugins) / sizeof(s_plugins[0]);
   for (int i = 0; i < N; ++i)
      if (!strcmp(plugin_id, s_plugins[i].desc->id))
         return s_plugins[i].create(host);
   return NULL;
}

static const clap_plugin_factory_t s_plugin_factory = {
   .get_plugin_count = plugin_factory_get_plugin_count,
   .get_plugin_descriptor = plugin_factory_get_plugin_descriptor,
   .create_plugin = plugin_factory_create_plugin,
};

////////////////
// clap_entry //
////////////////

static bool entry_init(const char *plugin_path) {
   // called only once, and first
   return true;
}

static void entry_deinit(void) {
   // called last
}

static const void *entry_get_factory(const char *factory_id) {
   if (!strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID))
      return &s_plugin_factory;
   return NULL;
}

CLAP_EXPORT const clap_plugin_entry_t clap_entry = {
   .clap_version = CLAP_VERSION,
   .init = entry_init,
   .deinit = entry_deinit,
   .get_factory = entry_get_factory,
};
