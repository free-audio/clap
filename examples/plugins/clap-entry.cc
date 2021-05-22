#include <clap/all.h>

static bool clap_init(const char *plugin_path) { return true; }

static void clap_deinit(void) {}

static int32_t clap_get_plugin_count(void)
{
    return 0;
}

static const clap_plugin_descriptor *clap_get_plugin_descriptor(int32_t index)
{
    return nullptr;
}

static clap_plugin *clap_create_plugin(clap_host *host, const char *plugin_id)
{
    return nullptr;
}

CLAP_EXPORT const struct clap_plugin_entry clap_plugin_entry = {
    clap_init,
    clap_deinit,
    clap_get_plugin_count,
    clap_get_plugin_descriptor,
    clap_create_plugin,
};