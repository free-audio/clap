#pragma once

#include "../../plugin.h"
#include "../../string-sizes.h"

// This extension lets the plugin expose its meters to the host: gain reduction, input and output
// levels, and so on, each with one or more channels.
//
// The host can display them in its mixer, or map them to a control surface, without opening the
// plugin's GUI. The values can be read from any thread, so the host can poll them at its own
// display rate.
//
// A plugin with a single gain reduction value should also implement
// clap.gain-adjustment-metering; a gain reduction meter here uses the same sign.

static CLAP_CONSTEXPR const char CLAP_EXT_METERS[] = "clap.meters/1";

#ifdef __cplusplus
extern "C" {
#endif

enum clap_meter_kind {
   // The meter doesn't fit any other kind. Its unit is unspecified and the host should only
   // rely on clap_meter_info.min_value and max_value to display it.
   CLAP_METER_KIND_OTHER = 0,

   // Gain adjustment in dB, with the same sign as clap.gain-adjustment-metering:
   // zero means no gain reduction, a negative value means the plugin is reducing the gain,
   // as with a compressor or limiter, and a positive value means it is adding gain, as with
   // an expander.
   CLAP_METER_KIND_GAIN_REDUCTION = 1,

   // Signal level in dBFS. Silence may be reported as -infinity or as any value below
   // clap_meter_info.min_value.
   CLAP_METER_KIND_LEVEL = 2,

   // Note: more entries could be added here in the future
};

enum clap_meter_flags {
   // A promise by the plugin: all the values one get_values() call returns come from the same
   // processed block. Without this flag each value is read on its own and two channels may
   // come from neighbouring blocks, which is fine for level bars but not for a value derived
   // from several channels, such as a correlation or a balance.
   //
   // To keep the promise without blocking either thread, the plugin keeps a ring buffer of N
   // copies of the meter's value array (three are enough) and an atomic index of the last
   // complete one: after each block the audio thread fills the next copy and then stores its
   // index. get_values() loads the index, copies that array, then loads the index again, and
   // keeps the copy only if the index advanced at most once; otherwise it retries a few times
   // and returns 0 if it never gets a clean copy. Without the second load, a reader that is
   // preempted during the copy can read a slot the audio thread is refilling.
   CLAP_METER_IS_COHERENT = 1 << 0,
};

typedef struct clap_meter_info {
   // Stable meter identifier, it must never change.
   clap_id id;

   // Displayable name, eg: "Gain Reduction", "Input", "Output".
   char name[CLAP_NAME_SIZE];

   // see enum clap_meter_kind
   uint32_t kind;

   // bitmask of clap_meter_flags
   uint32_t flags;

   // Number of values the meter has, eg: 1 for a linked gain reduction, 2 for a stereo level.
   uint32_t channel_count;

   // Display range suggested by the plugin, in the meter's unit.
   double min_value;
   double max_value;
} clap_meter_info_t;

typedef struct clap_plugin_meters {
   // Returns the number of meters.
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // Copies the meter's info to meter_info and returns true on success.
   // [main-thread]
   bool(CLAP_ABI *get_info)(const clap_plugin_t *plugin,
                            uint32_t             index,
                            clap_meter_info_t   *meter_info);

   // Copies the latest values of the meter meter_id into values, one per channel, up to
   // capacity values, and returns the number of values written.
   //
   // The values are the ones the plugin would show on its own meters for the most recently
   // processed block, ballistics included. Reading them must not change them: several readers
   // may poll the same meter.
   //
   // Unless the meter has CLAP_METER_IS_COHERENT, the values of one call may come from
   // different blocks: each is read on its own, and a stereo pair can be one block apart.
   // A plugin that needs its channels together publishes them as one block and sets the flag;
   // the host never synchronises, it only copies.
   //
   // After processing stops the plugin keeps returning the last values it computed, until
   // deactivate(); after activation and before the first process() it returns 0.
   //
   // Returns 0 if meter_id is unknown, or if the plugin is not active or has not processed
   // any block since it was activated.
   //
   // This function must be lock-free and wait-free, as it can be called concurrently with
   // process(), for example from a GUI or a network thread of the host.
   // [thread-safe]
   uint32_t(CLAP_ABI *get_values)(const clap_plugin_t *plugin,
                                  clap_id              meter_id,
                                  double              *values,
                                  uint32_t             capacity);
} clap_plugin_meters_t;

typedef struct clap_host_meters {
   // Informs the host that the list of meters or their info changed.
   // The host will call count() and get_info() again.
   //
   // The list can only change while the plugin is deactivated. If the plugin is activated, it
   // must call clap_host->request_restart() and delay the change until the host calls
   // clap_plugin->deactivate().
   // [main-thread & !active]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_meters_t;

#ifdef __cplusplus
}
#endif
