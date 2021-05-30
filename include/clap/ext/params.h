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
/// There is a single way at a time for the host to set parameters value.
/// - if the plugin is active, send @ref CLAP_EVENT_PARAM_SET during the process call [audio-thread]
/// - if the plugin is not active, call @ref clap_plugin_params.set_value [main-thread]
///
/// Rationale: Ideally there would be a single way to set parameters values.
/// Sending @ref CLAP_EVENT_PARAM_SET via @ref clap_plugin.process is the natural way to play
/// parameter automation. But, the process call is only possible if the plugin is active. If the
/// plugin is not active, then everything happens on the [main-thread] using @ref clap_plugin_params
/// and @ref clap_host_params interfaces.
///
/// When the plugin changes a parameter value, it must inform the host.
/// If the plugin is active, it will send @ref CLAP_EVENT_PARAM_SET during the process call
/// - set @ref clap_event_param.begin_adjust to mark the begining of automation recording
/// - set @ref clap_event_param.end_adjust to mark the end of automation recording
/// If the plugin is not active, it will use:
/// - @ref clap_host_params.adjust_begin() - marks the begining of automation recording
/// - @ref clap_host_params.adjust() - adds a new point in the automation recording
/// - @ref clap_host_params.adjust_end() - marks the end of automation recording
///
/// @note MIDI CCs are a tricky because you may not know when the parameter adjustment ends.
/// Also if the hosts records incoming MIDI CC and parameter change automation at the same time,
/// there will be a conflict at playback: MIDI CC vs Automation.
/// The parameter automation will always target the same parameter because the param_id is stable.
/// The MIDI CC may have a different mapping in the future and may result in a different playback.
///
/// It is highly recommanded to use @ref clap_plugin_midi_mappings to let the host solve
/// this problem and offer a consistent experience to the user across different plugins.
///
/// Scenarios:
///
/// I. Loading a preset
/// - load the preset in a temporary state, if the plugin is activated and preset will introduce
///   breaking change like latency, audio ports change, new parameters, ...
///   report those to the host and wait for the host to deactivate the plugin
///   to apply those changes. If there are no breaking changes, the plugin can apply them
///   them right away.
///   The plugin is resonsible to update both its audio processor and its gui.
///
/// II. Turning a knob on the DAW interface
/// - if the plugin is active, the host will send a @ref CLAP_EVENT_PARAM_SET event to the plugin
/// - if the plugin is not active, the host will call @ref clap_plugin_params.set_value
///
/// III. Turning a knob on the Plugin interface
/// - if the plugin is not active
///   - host_params->begin_adjust(...)
///   - host_params->adjust(...) many times -> updates host's knob and record automation
///   - host_params->end_adjust(...)
/// - if the plugin is active
///   - send CLAP_PARAM_SET event and don't forget to set begin_adjust and end_adjust attributes
/// - the plugin is responsible to send the parameter value to its audio processor
///
/// IV. Turning a knob via automation
/// - host sends a @ref CLAP_EVENT_PARAM_SET during the process call
/// - the plugin is responsible to update its GUI
///
/// V. Turning a knob via internal MIDI mapping
/// - the plugin sends a CLAP_EVENT_PARAM_SET output event
/// - the plugin is responsible to update its GUI
///
/// VI. Adding or removing parameters
/// - call host_params->rescan(CLAP_PARAM_RESCAN_ALL)
/// - if the plugin is activated, apply the new parameters once the host deactivates the plugin

#define CLAP_EXT_PARAMS "clap/params"

enum {
   CLAP_PARAM_FLOAT = 0, // uses value.d
   CLAP_PARAM_BOOL = 1,  // uses value.b
   CLAP_PARAM_INT = 2,   // uses value.i
   CLAP_PARAM_ENUM = 3,  // uses value.i
};
typedef uint32_t clap_param_type;

/* This describes the parameter and provides the current value */
typedef struct clap_param_info {
   /* param info */
   clap_id id;
   char    name[CLAP_NAME_SIZE];     // the display name
   char    module[CLAP_MODULE_SIZE]; // the module containing the param, eg:
                                     // "/filters/moog"; '/' will be used as a
                                     // separator to show a tree like structure.

   bool is_per_note;    // does this param supports per note automations?
   bool is_per_channel; // does this param supports per channel automations?
   bool is_used;        // is this parameter used by the patch?
   bool is_periodic;    // after the last value, go back to the first one
   bool is_locked;      // if true, the parameter can't be changed by the host
   bool is_automatable; // can the host send param event to change it in the process call?
   bool is_hidden; // don't show it to the user, unless the parameter is already used (automation,
                   // modulation, controller mapping)
   bool is_bypass; // used to merge the plugin and host bypass button.

   /* value */
   clap_param_type  type;             // this field is not allowed to change for a given param id
   clap_param_value min_value;        // minimum plain value
   clap_param_value max_value;        // maximum plain value
   clap_param_value default_value;    // default plain value
   uint32_t         enum_entry_count; // the number of values in the enum, if type is an enum
} clap_param_info;

typedef struct clap_plugin_params {
   // Returns the number of parameters.
   // [main-thread]
   uint32_t (*count)(const clap_plugin *plugin);

   // Copies the parameter's info to param_info and returns true on success.
   // [main-thread]
   bool (*info)(const clap_plugin *plugin, int32_t param_index, clap_param_info *param_info);

   // [main-thread]
   bool (*enum_value)(const clap_plugin *plugin,
                      clap_id            param_id,
                      int32_t            value_index,
                      clap_param_value * value);

   // Gets the parameter plain value.
   // [main-thread]
   bool (*value)(const clap_plugin *plugin, clap_id param_id, clap_param_value *value);

   // Sets the parameter plain value.
   // If the plupin is activated, then the host must send a param event
   // in the next process call to update the audio processor.
   // [main-thread]
   bool (*set_value)(const clap_plugin *plugin,
                     clap_id            param_id,
                     clap_param_value   value,
                     clap_param_value   modulation);

   // Formats the display text for the given parameter value.
   // [thread-safe,lock-wait-free]
   bool (*value_to_text)(const clap_plugin *plugin,
                         clap_id            param_id,
                         clap_param_value   value,
                         char *             display,
                         uint32_t           size);

   bool (*text_to_value)(const clap_plugin *plugin,
                         clap_id            param_id,
                         const char *       display,
                         clap_param_value * value);
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
   // - is_modulable
   // - is_used
   // - is_periodic
   // - is_hidden
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
   //   - is_automatable
   //   - is_locked
   //   - is_bypass
   //   - min_value
   //   - max_value
   //   - enum definition (enum_entry_count or enum value)
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

typedef struct clap_host_params {
   /* [main-thread] */
   void (*adjust_begin)(const clap_host *host, clap_id param_id);

   // If the plugin is activated, the host must send a parameter update
   // in the next process call to update the audio processor.
   // Only for value changes that happens in the gui.
   // [main-thread]
   void (*adjust)(const clap_host *host, clap_id param_id, clap_param_value value);

   /* [main-thread] */
   void (*adjust_end)(const clap_host *host, clap_id param_id);

   // Rescan the full list of parameters according to the flags.
   // [main-thread]
   void (*rescan)(const clap_host *host, uint32_t flags);
} clap_host_params;

#ifdef __cplusplus
}
#endif