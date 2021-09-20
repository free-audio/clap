#pragma once

#include "../clap.h"
#include "../string-sizes.h"

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
/// There is two options to communicate parameter value change, and they are not concurrent.
/// - send automation points during clap_plugin.process()
/// - send automation points during clap_plugin_params.flush(), this one is used when the plugin is
///   not processing
///
/// When the plugin changes a parameter value, it must inform the host.
/// It will send @ref CLAP_EVENT_PARAM_VALUE event during process() or flush().
/// - set the flag CLAP_EVENT_PARAM_BEGIN_ADJUST to mark the begining of automation recording
/// - set the flag CLAP_EVENT_PARAM_END_ADJUST to mark the end of automation recording
/// - set the flag CLAP_EVENT_PARAM_SHOULD_RECORD if the event should be recorded
///
/// @note MIDI CCs are a tricky because you may not know when the parameter adjustment ends.
/// Also if the hosts records incoming MIDI CC and parameter change automation at the same time,
/// there will be a conflict at playback: MIDI CC vs Automation.
/// The parameter automation will always target the same parameter because the param_id is stable.
/// The MIDI CC may have a different mapping in the future and may result in a different playback.
///
/// When a MIDI CC changes a parameter's value, set @ref clap_event_param.should_record to false.
/// That way the host may record the MIDI CC automation, but not the parameter change and there
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
/// - the host will send an automation event to the plugin via a process() or flush()
///
/// III. Turning a knob on the Plugin interface
/// - if the plugin is not processing, call clap_host_params->request_flush() or
///   clap_host->request_process().
/// - send an automation event and don't forget to set begin_adjust, end_adjust and should_record
///   attributes
/// - the plugin is responsible to send the parameter value to its audio processor
///
/// IV. Turning a knob via automation
/// - host sends an automation point during clap_plugin->process() or clap_plugin_params->flush().
/// - the plugin is responsible to update its GUI
///
/// V. Turning a knob via plugin's internal MIDI mapping
/// - the plugin sends a CLAP_EVENT_PARAM_SET output event, set should_record to false
/// - the plugin is responsible to update its GUI
///
/// VI. Adding or removing parameters
/// - if the plugin is activated call clap_host->restart()
/// - once the plugin isn't active:
///   - apply the new state
///   - call clap_host_params->rescan(CLAP_PARAM_RESCAN_ALL)
///   - if a parameter is created with an id that may have been used before,
///     call clap_host_params.clear(host, param_id, CLAP_PARAM_CLEAR_ALL)

static CLAP_CONSTEXPR const char CLAP_EXT_PARAMS[] = "clap.params";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // Is this param stepped? (integer values only)
   // if so the double value is converted to integer using a cast (equivalent to trunc).
   CLAP_PARAM_IS_STEPPED = 1 << 0,

   // Does this param supports per note automations?
   CLAP_PARAM_IS_PER_NOTE = 1 << 1,

   // Does this param supports per channel automations?
   CLAP_PARAM_IS_PER_CHANNEL = 1 << 2,

   // Useful for phase ;-)
   CLAP_PARAM_IS_PERIODIC = 1 << 3,

   // The parameter should not be shown to the user, because it is currently not used.
   // It is not necessary to process automation for this parameter.
   CLAP_PARAM_IS_HIDDEN = 1 << 4,

   // This parameter is used to merge the plugin and host bypass button.
   // It implies that the parameter is stepped.
   // min: 0 -> bypass off
   // max: 1 -> bypass on
   CLAP_PARAM_IS_BYPASS = (1 << 5) | CLAP_PARAM_IS_STEPPED,

   // The parameter can't be changed by the host.
   CLAP_PARAM_IS_READONLY = 1 << 6,

   // Does the parameter support the modulation signal?
   CLAP_PARAM_IS_MODULATABLE = 1 << 7,

   // Any change to this parameter will affect the plugin output and requires to be done via
   // process() if the plugin is active.
   //
   // A simple example would be a DC Offset, changing it will change the output signal and must be
   // processed.
   CLAP_PARAM_REQUIRES_PROCESS = 1 << 8,
};
typedef uint32_t clap_param_info_flags;

/* This describes a parameter */
typedef struct clap_param_info {
   // stable parameter identifier, it must never change.
   clap_id id;

   clap_param_info_flags flags;

   // This value is optional and set by the plugin.
   // Its purpose is to provide a fast access to the plugin parameter:
   //
   //    Parameter *p = findParameter(param_id);
   //    param_info->cookie = p;
   //
   //    /* and later on */
   //    Parameter *p = (Parameter *)cookie;
   //
   // It is invalidated on clap_host_params->rescan(CLAP_PARAM_RESCAN_ALL) and when the plugin is
   // destroyed.
   void *cookie;

   char name[CLAP_NAME_SIZE];     // the display name
   char module[CLAP_MODULE_SIZE]; // the module containing the param, eg:
                                  // "oscillators/wt1"; '/' will be used as a
                                  // separator to show a tree like structure.

   double min_value;     // minimum plain value
   double max_value;     // maximum plain value
   double default_value; // default plain value
} clap_param_info;

typedef struct clap_plugin_params {
   // Returns the number of parameters.
   // [main-thread]
   uint32_t (*count)(const clap_plugin *plugin);

   // Copies the parameter's info to param_info and returns true on success.
   // [main-thread]
   bool (*get_info)(const clap_plugin *plugin, int32_t param_index, clap_param_info *param_info);

   // Gets the parameter plain value.
   // [main-thread]
   bool (*get_value)(const clap_plugin *plugin, clap_id param_id, double *value);

   // Formats the display text for the given parameter value.
   // [main-thread]
   bool (*value_to_text)(
      const clap_plugin *plugin, clap_id param_id, double value, char *display, uint32_t size);

   // Converts the display text to a parameter value.
   // [main-thread]
   bool (*text_to_value)(const clap_plugin *plugin,
                         clap_id            param_id,
                         const char *       display,
                         double *           value);

   // Flushes a set of parameter changes.
   // This method must not be called concurrently to clap_plugin->process().
   // This method must not be used if the plugin is processing.
   //
   // [active && !processing : audio-thread]
   // [!active : main-thread]
   void (*flush)(const clap_plugin *    plugin,
                 const clap_event_list *input_parameter_changes,
                 const clap_event_list *output_parameter_changes);
} clap_plugin_params;

enum {
   // The parameter values did change, eg. after loading a preset.
   // The host will scan all the parameters value.
   // The host will not record those changes as automation points.
   // New values takes effect immediately.
   CLAP_PARAM_RESCAN_VALUES = 1 << 0,

   // The value to text conversion changed, and the text needs to be rendered again.
   CLAP_PARAM_RESCAN_TEXT = 1 << 1,

   // The parameter info did change, use this flag for:
   // - name change
   // - module change
   // - is_periodic (flag)
   // - is_hidden (flag)
   // New info takes effect immediately.
   CLAP_PARAM_RESCAN_INFO = 1 << 2,

   // Invalidates everything the host knows about parameters.
   // It can only be used while the plugin is deactivated.
   // If the plugin is activated use clap_host->restart() and delay any change until the host calls
   // clap_plugin->deactivate().
   //
   // You must use this flag if:
   // - some parameters were added or removed.
   // - some parameters had critical changes:
   //   - is_per_note (flag)
   //   - is_per_channel (flag)
   //   - is_readonly (flag)
   //   - is_bypass (flag)
   //   - is_stepped (flag)
   //   - is_modulatable (flag)
   //   - min_value
   //   - max_value
   //   - cookie
   CLAP_PARAM_RESCAN_ALL = 1 << 3,
};
typedef uint32_t clap_param_rescan_flags;

enum {
   // Clears all possible references to a parameter
   CLAP_PARAM_CLEAR_ALL = 1 << 0,

   // Clears all automations to a parameter
   CLAP_PARAM_CLEAR_AUTOMATIONS = 1 << 1,

   // Clears all modulations to a parameter
   CLAP_PARAM_CLEAR_MODULATIONS = 1 << 2,
};
typedef uint32_t clap_param_clear_flags;

typedef struct clap_host_params {
   // Rescan the full list of parameters according to the flags.
   // [main-thread]
   void (*rescan)(const clap_host *host, clap_param_rescan_flags flags);

   // Clears references to a parameter
   // [main-thread]
   void (*clear)(const clap_host *host, clap_id param_id, clap_param_clear_flags flags);

   // Request the host to call clap_plugin_params->fush().
   // This is useful if the plugin has parameters value changes to report to the host but the plugin
   // is not processing.
   //
   // eg. the plugin has a USB socket to some hardware controllers and receives a parameter change
   // while it is not processing.
   //
   // This must not be called on the [audio-thread].
   //
   // [thread-safe]
   void (*request_flush)(const clap_host *host);
} clap_host_params;

#ifdef __cplusplus
}
#endif