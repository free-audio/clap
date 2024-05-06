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
   // 
   // For the string value getter specifically, the get function should return the 
   // size of the value it's written (or would've written if it had fit in the 
   // buffer), excluding the null terminator. 
   // If there's no value matching the key, a value of 0 is returned. If no value 
   // pointer is provided and/or value_capacity is 0, the value shouldn't be touched, 
   // but a correct required length should be returned.
   // .
   // [main-thread]
   bool(CLAP_ABI *get_int)(const clap_host_t *host, const char *key, int32_t *value);
   bool(CLAP_ABI *get_double)(const clap_host_t *host, const char *key, double *value);
   int32_t(CLAP_ABI *get_string)(const clap_host_t *host, const char *key, char *value, int32_t value_capacity);

   // Sets a value with a given key. Returns true if the value was set succesfully.
   // Note that string values provided are null-terminated and shouldn't be 
   // expected to have a lifetime past the set_string call.
   // [main-thread]
   bool(CLAP_ABI *set_int)(const clap_host_t *host, const char *key, int32_t value);
   bool(CLAP_ABI *set_double)(const clap_host_t *host, const char *key, double value);
   bool(CLAP_ABI *set_string)(const clap_host_t *host, const char *key, const char *value);

} clap_host_track_info_extended_t;

#ifdef __cplusplus
}
#endif
