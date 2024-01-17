#pragma once

// Pair of plugin ABI and plugin identifier.
typedef struct clap_plugin_id {
   // The plugin ABI name, in lowercase.
   // eg: "clap", "vst3", "vst2", "au", "lv2", ...
   const char *abi;

   // The plugin ID, formatted as follow:
   //
   // CLAP: use the plugin id
   //   eg: "com.u-he.diva"
   //
   // AU: format the string like "manu:type:subt"
   //   eg: "aumu:SgXT:VmbA"
   //
   // VST2: print the id as a signed 32-bits integer
   //   eg: "-4382976"
   //
   // VST3: print the id as a standard UUID
   //   eg: "123e4567-e89b-12d3-a456-426614174000"
   const char *id;
} clap_plugin_id_t;
