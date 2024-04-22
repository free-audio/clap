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
   // Represents a document/project/session.
   CLAP_PLUGIN_LOCATION_PROJECT,

   // Represents a group of tracks.
   // It can contain both track groups, tracks and devices (post processing).
   // The first device within a track group has the index of
   // the last track or track group within this group + 1.
   CLAP_PLUGIN_LOCATION_TRACK_GROUP,

   // Represents a single tracks. It contains devices (serial).
   CLAP_PLUGIN_LOCATION_TRACK,

   // Represents a single device.
   // It contains other nested device chains.
   CLAP_PLUGIN_LOCATION_DEVICE,

   // Represents a nested device chain (serial).
   // Its parent must be a device.
   // It contains other devices.
   CLAP_PLUGIN_LOCATION_NESTED_DEVICE_CHAIN,
};

typedef struct clap_plugin_location_element {
   // Kind of the element, must be one of the CLAP_PLUGIN_LOCATION_* values.
   uint32_t kind;

   // Index within the parent element.
   // Set to 0 if irrelevant.
   uint32_t index;

   // Internal ID of the element.
   // This is not intended for display to the user,
   // but rather to give the host a potential quick way for lookups.
   const char *id;

   // User friendly name of the element.
   const char *name;

   // Color for this element, should be CLAP_COLOR_TRANSPARENT if no color is
   // used for this element.
   clap_color_t color;
} clap_plugin_location_element_t;

typedef struct clap_plugin_location {
   // Called by the host when the location of the plugin instance changes.
   //
   // The last item in this array always refers to the device itself, and as
   // such is expected to be of kind CLAP_PLUGIN_LOCATION_DEVICE.
   // [main-thread]
   void (*set_location)(clap_plugin_t                  *plugin,
                        clap_plugin_location_element_t *path,
                        int                             num_elements);
} clap_plugin_location_t;

#ifdef __cplusplus
}
#endif
