#pragma once

// Pair of plugin ABI and plugin identifier.
typedef struct clap_plugin_id {
   // The plugin ABI name, in lowercase.
   // eg: "clap", "vst3", "vst2", "au", "lv2", ...
   const char *abi;

   // The plugin ID, for example "com.u-he.Diva".
   // If the ABI rely upon binary plugin ids, then they shall be hex encoded (lower case).
   // eg:
   //     T binary_id;
   //     const uint8_t * data = (const uint8_t *)&binary_id;
   //     char *buffer = malloc(2 * sizeof(binary_id) + 1);
   //     hex_encode(data, sizeof (binary_id), buffer);
   //
   // Note: if the binary id is sensible to the CPU endianness, the encoded id will be too.
   // As a result, the id should be decoded immediately by the consumer.
   const char *id;
} clap_plugin_id_t;
