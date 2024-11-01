#pragma once

#include "../../plugin.h"
#include "../../color.h"

// This extension allows a host to tell the plugin more about its position 
// within a project or session. 

static CLAP_CONSTEXPR const char CLAP_EXT_LOCATION[] = "clap.location/1";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   CLAP_PLUGIN_LOCATION_PROJECT,
   CLAP_PLUGIN_LOCATION_TRACK,
   CLAP_PLUGIN_LOCATION_TRACK_GROUP,
   CLAP_PLUGIN_LOCATION_DEVICE,
   CLAP_PLUGIN_LOCATION_DEVICE_PARALLEL_GROUP,
   CLAP_PLUGIN_LOCATION_DEVICE_SERIAL_GROUP,
};

typedef struct clap_plugin_location_element {
   // Internal ID of the element. This is not intended for display to the user, 
   // but rather to give the host a potential quick way for lookups.
   const char *id;
   // User friendly name of the element.
   const char *name;
   // Kind of the element, must be one of the CLAP_PLUGIN_LOCATION_* values.
   int kind;
   // Index within the parent element. 
   uint32_t index_in_group;
   // Index of the element in a global context. When sorting elements by this index, 
   // the order should mirror the order as it is displayed in the host. 
   uint32_t global_index;
   // Color for this element, should be 0x00000000 if no color is used for this 
   // element. 
   clap_color_t color;
} clap_plugin_location_element_t;


typedef struct clap_plugin_location {
   // Called by the host when the location of the plugin instance changes.
   //
   // The last item in this array always refers to the device itself, and as 
   // such is expected to be of kind CLAP_PLUGIN_LOCATION_DEVICE. 
   // [main-thread]
   void (*set_location)(clap_plugin_t *plugin, clap_plugin_location_element_t *path, int num_elements);
} clap_plugin_location_t;

#ifdef __cplusplus
}
#endif
