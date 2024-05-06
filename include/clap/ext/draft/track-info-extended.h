#pragma once

#include "../../plugin.h"
#include "../../string-sizes.h"

// This extension extends the track-info extension with a more generic key/value system.
// As it extends track-info, it doesn't have its own clap_plugin_* struct with a callback for
// when the values change, instead the host is expected to call clap_plugin_track_info::changed in 
// this case.

static CLAP_CONSTEXPR const char CLAP_EXT_TRACK_INFO_EXTENDED[] = "clap.track-info-extended/1";


// Index of the track the instance is on (read_only, int32_t).
static CLAP_CONSTEXPR const char CLAP_TRACK_INFO_EXTENDED_TRACK_INDEX[] = "clap.track-info-extended.track-index";
// Index of the plugin instance within the track (read_only, int32_t).
static CLAP_CONSTEXPR const char CLAP_TRACK_INFO_EXTENDED_PLUGIN_INDEX[] = "clap.track-info-extended.plugin-index";

#ifdef __cplusplus
extern "C" {
#endif


typedef struct clap_host_track_info_extended {

   // Gets a value with a given key from the host, fills 'value' with the 
   // corresponding value and returns true if a value was filled succesfully. 
   // If it returns false, the value of 'value' isn't touched.
   // [main-thread]
   bool(CLAP_ABI *get_int)(const clap_host_t *host, const char *key, int32_t *value);
   bool(CLAP_ABI *get_string)(const clap_host_t *host, const char *key, char (*value)[CLAP_NAME_SIZE]);
   bool(CLAP_ABI *get_double)(const clap_host_t *host, const char *key, double *value);

   // Sets a value with a given key. Returns true if the value was set succesfully.
   // [main-thread]
   bool(CLAP_ABI *set_int)(const clap_host_t *host, const char *key, int32_t value);
   bool(CLAP_ABI *set_string)(const clap_host_t *host, const char *key, char (value)[CLAP_NAME_SIZE]);
   bool(CLAP_ABI *set_double)(const clap_host_t *host, const char *key, double value);

} clap_host_track_info_extended_t;

#ifdef __cplusplus
}
#endif
