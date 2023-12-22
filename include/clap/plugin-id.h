#pragma once

// Pair of plugin ABI and plugin identifier.
typedef struct clap_plugin_id {
   // The plugin ABI name, in lowercase.
   // eg: "clap"
   const char *abi;

   // The plugin ID, for example "com.u-he.Diva".
   // If the ABI rely upon binary plugin ids, then they shall be hex encoded (lower case).
   const char *id;
} clap_plugin_id_t;

