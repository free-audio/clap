#pragma once

#include "../clap.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @page Parameters
/// @brief parameters management
///
/// Main idea:
///
/// The host sees the plugin as an atomic entity; and acts as a controler on top of its parameters.
/// The plugin is responsible to keep in sync its audio processor and its GUI.
///
/// The host can read at any time parameters value on the [main-thread] using
/// @ref clap_plugin_params.value().
///
/// There is a single way for the host to set parameters value:
/// - send @ref CLAP_EVENT_PARAM_SET during the process call [audio-thread]
///
/// When the plugin changes a parameter value, it must inform the host.
/// It will send @ref CLAP_EVENT_PARAM_SET during the process call
/// - set @ref clap_event_param.begin_adjust to mark the begining of automation recording
/// - set @ref clap_event_param.end_adjust to mark the end of automation recording
/// - set @ref clap_event_param.should_record to true if the event should be recorded
///
/// @note MIDI CCs are a tricky because you may not know when the parameter adjustment ends.
/// Also if the hosts records incoming MIDI CC and parameter change automation at the same time,
/// there will be a conflict at playback: MIDI CC vs Automation.
/// The parameter automation will always target the same parameter because the param_id is stable.
/// The MIDI CC may have a different mapping in the future and may result in a different playback.
///
/// When a MIDI CC changes a parameter's value, set @ref clap_event_param.should_record to false.
/// That way the host will record the MIDI CC automation, but not the parameter change and there
/// won't be conflict at playback.
///
/// Scenarios:
///
/// I. Loading a preset
/// - load the preset in a temporary state
/// - call @ref clap_host_params.changed() if anything changed
/// - call @ref clap_host_latency.changed() if latency changed
/// - invalidate any other info that may be cached by the host
/// - if the plugin is activated and the preset will introduce breaking change
///   (latency, audio ports, new parameters, ...) be sure to wait for the host
///   to deactivate the plugin to apply those changes.
///   If there are no breaking changes, the plugin can apply them them right away.
///   The plugin is resonsible to update both its audio processor and its gui.
///
/// II. Turning a knob on the DAW interface
/// - the host will send a @ref CLAP_EVENT_PARAM_SET event to the plugin via a process call
///
/// III. Turning a knob on the Plugin interface
/// - send CLAP_PARAM_SET event and don't forget to set begin_adjust, end_adjust and should_record
///   attributes
/// - the plugin is responsible to send the parameter value to its audio processor
///
/// IV. Turning a knob via automation
/// - host sends a @ref CLAP_EVENT_PARAM_SET during the process call
/// - the plugin is responsible to update its GUI
///
/// V. Turning a knob via internal MIDI mapping
/// - the plugin sends a CLAP_EVENT_PARAM_SET output event, set should_record to false
/// - the plugin is responsible to update its GUI
///
/// VI. Adding or removing parameters
/// - call host_params->rescan(CLAP_PARAM_RESCAN_ALL)
/// - if the plugin is activated, apply the new parameters once the host deactivates the plugin
/// - if a parameter is created with an id that may have been used before, call:
///   - clap_host_params.clear(host, param_id, CLAP_PARAM_CLEAR_ALL)

#define CLAP_EXT_PARAMS "clap/params"

enum {
   // Is this param stepped? (integer values only)
   // if so the double value is converted to integer using a cast (equivalent to trunc).
   CLAP_PARAM_STEPPED = 1 << 0,

   // Does this param supports per note automations?
   CLAP_PARAM_IS_PER_NOTE = 1 << 1,

   // Does this param supports per channel automations?
   CLAP_PARAM_IS_PER_CHANNEL = 1 << 2,

   // Useful for phase ;-)
   CLAP_PARAM_IS_PERIODIC = 1 << 3,

   // The parameter should not be shown to the user,
   // this can be useful if a parameter is not used in a patch.
   CLAP_PARAM_IS_HIDDEN = 1 << 4,

   // This parameter bypass the sed to merge the plugin and host bypass button.
   CLAP_PARAM_IS_BYPASS = 1 << 5,

   // The parameter can't be changed by the host.
   CLAP_PARAM_IS_READONLY = 1 << 6,
};

/* This describes the parameter and provides the current value */
typedef struct clap_param_info {
   /* param info */
   clap_id id;
   char    name[CLAP_NAME_SIZE];     // the display name
   char    module[CLAP_MODULE_SIZE]; // the module containing the param, eg:
                                     // "/filters/moog"; '/' will be used as a
                                     // separator to show a tree like structure.

   uint32_t flags;
   double   min_value;     // minimum plain value
   double   max_value;     // maximum plain value
   double   default_value; // default plain value
} clap_param_info;

typedef struct clap_plugin_params {
   // Returns the number of parameters.
   // [main-thread]
   uint32_t (*count)(const clap_plugin *plugin);

   // Copies the parameter's info to param_info and returns true on success.
   // [main-thread]
   bool (*info)(const clap_plugin *plugin, int32_t param_index, clap_param_info *param_info);

   // Gets the parameter plain value.
   // [main-thread]
   bool (*value)(const clap_plugin *plugin, clap_id param_id, double *value);

   // Formats the display text for the given parameter value.
   // [thread-safe,lock-wait-free]
   bool (*value_to_text)(
      const clap_plugin *plugin, clap_id param_id, double value, char *display, uint32_t size);

   bool (*text_to_value)(const clap_plugin *plugin,
                         clap_id            param_id,
                         const char *       display,
                         double *           value);
} clap_plugin_params;

enum {
   // The parameter values did change. Typically what happens when loading a preset.
   // The host will scan the parameter value and value_to_text.
   // The host will not record those changes as automation points.
   // New values takes effect immediately.
   CLAP_PARAM_RESCAN_VALUES = 1 << 0,

   // The parameter info did change, use this flag for:
   // - name change
   // - module change
   // - flags
   // New info takes effect immediately.
   CLAP_PARAM_RESCAN_INFO = 1 << 1,

   // This invalidates everything the host knows about parameters and takes effect after the host
   // did deactivate the plugin if it was activated.
   //
   // You must use this flag if:
   // - some parameters were added or removed.
   // - some parameters had critical changes which requieres to invalidate the host queues:
   //   - is_per_note
   //   - is_per_channel
   //   - is_readonly
   //   - is_bypass
   //   - min_value
   //   - max_value
   //
   // The plugin can't perform the parameter list change immediately:
   // 1. host_params->rescan(host, CLAP_PARAM_RESCAN_ALL);
   // 2. the host needs to flush its parameters update queues and suspend
   //    the plugin execution in the audio engine
   // 3. the host calls plugin->set_active(plugin, 0, false); and the plugin
   //    can switch to its new parameters
   // 4. the host scans all the parameters
   // 5. the host activates the plugin
   CLAP_PARAM_RESCAN_ALL = 1 << 2,
};

enum {
   // Clears all possible references to a parameter
   CLAP_PARAM_CLEAR_ALL = 1 << 0,

   // Clears all automations to a parameter
   CLAP_PARAM_CLEAR_AUTOMATIONS = 1 << 1,

   // Clears all modulations to a parameter
   CLAP_PARAM_CLEAR_MODULATIONS = 1 << 2,
};

typedef struct clap_host_params {
   // Rescan the full list of parameters according to the flags.
   // [main-thread]
   void (*rescan)(const clap_host *host, uint32_t flags);

   // Clears references to a parameter
   // [main-thread]
   void (*clear)(const clap_host *host, clap_id param_id, uint32_t flags);
} clap_host_params;

#ifdef __cplusplus
}
#endif