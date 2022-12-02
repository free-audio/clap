﻿#pragma once

#include "../plugin.h"
#include "../string-sizes.h"

/// @page Parameters
/// @brief parameters management
///
/// Main idea:
///
/// The host sees the plugin as an atomic entity; and acts as a controller on top of its parameters.
/// The plugin is responsible for keeping its audio processor and its GUI in sync.
///
/// The host can at any time read parameters' value on the [main-thread] using
/// @ref clap_plugin_params.value().
///
/// There are two options to communicate parameter value changes, and they are not concurrent.
/// - send automation points during clap_plugin.process()
/// - send automation points during clap_plugin_params.flush(), for parameter changes
///   without processing audio
///
/// When the plugin changes a parameter value, it must inform the host.
/// It will send @ref CLAP_EVENT_PARAM_VALUE event during process() or flush().
/// If the user is adjusting the value, don't forget to mark the begining and end
/// of the gesture by sending CLAP_EVENT_PARAM_GESTURE_BEGIN and CLAP_EVENT_PARAM_GESTURE_END
/// events.
///
/// @note MIDI CCs are tricky because you may not know when the parameter adjustment ends.
/// Also if the host records incoming MIDI CC and parameter change automation at the same time,
/// there will be a conflict at playback: MIDI CC vs Automation.
/// The parameter automation will always target the same parameter because the param_id is stable.
/// The MIDI CC may have a different mapping in the future and may result in a different playback.
///
/// When a MIDI CC changes a parameter's value, set the flag CLAP_EVENT_DONT_RECORD in
/// clap_event_param.header.flags. That way the host may record the MIDI CC automation, but not the
/// parameter change and there won't be conflict at playback.
///
/// Scenarios:
///
/// I. Loading a preset
/// - load the preset in a temporary state
/// - call @ref clap_host_params.rescan() if anything changed
/// - call @ref clap_host_latency.changed() if latency changed
/// - invalidate any other info that may be cached by the host
/// - if the plugin is activated and the preset will introduce breaking changes
///   (latency, audio ports, new parameters, ...) be sure to wait for the host
///   to deactivate the plugin to apply those changes.
///   If there are no breaking changes, the plugin can apply them them right away.
///   The plugin is resonsible for updating both its audio processor and its gui.
///
/// II. Turning a knob on the DAW interface
/// - the host will send an automation event to the plugin via a process() or flush()
///
/// III. Turning a knob on the Plugin interface
/// - the plugin is responsible for sending the parameter value to its audio processor
/// - call clap_host_params->request_flush() or clap_host->request_process().
/// - when the host calls either clap_plugin->process() or clap_plugin_params->flush(),
///   send an automation event and don't forget to set begin_adjust,
///   end_adjust and should_record flags
///
/// IV. Turning a knob via automation
/// - host sends an automation point during clap_plugin->process() or clap_plugin_params->flush().
/// - the plugin is responsible for updating its GUI
///
/// V. Turning a knob via plugin's internal MIDI mapping
/// - the plugin sends a CLAP_EVENT_PARAM_SET output event, set should_record to false
/// - the plugin is responsible to update its GUI
///
/// VI. Adding or removing parameters
/// - if the plugin is activated call clap_host->restart()
/// - once the plugin isn't active:
///   - apply the new state
///   - if a parameter is gone or is created with an id that may have been used before,
///     call clap_host_params.clear(host, param_id, CLAP_PARAM_CLEAR_ALL)
///   - call clap_host_params->rescan(CLAP_PARAM_RESCAN_ALL)

static CLAP_CONSTEXPR const char CLAP_EXT_PARAMS[] = "clap.params";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // Is this param stepped? (integer values only)
   // if so the double value is converted to integer using a cast (equivalent to trunc).
   CLAP_PARAM_IS_STEPPED = 1 << 0,

   // Useful for for periodic parameters like a phase
   CLAP_PARAM_IS_PERIODIC = 1 << 1,

   // The parameter should not be shown to the user, because it is currently not used.
   // It is not necessary to process automation for this parameter.
   CLAP_PARAM_IS_HIDDEN = 1 << 2,

   // The parameter can't be changed by the host.
   CLAP_PARAM_IS_READONLY = 1 << 3,

   // This parameter is used to merge the plugin and host bypass button.
   // It implies that the parameter is stepped.
   // min: 0 -> bypass off
   // max: 1 -> bypass on
   CLAP_PARAM_IS_BYPASS = 1 << 4,

   // When set:
   // - automation can be recorded
   // - automation can be played back
   //
   // The host can send live user changes for this parameter regardless of this flag.
   //
   // If this parameter affects the internal processing structure of the plugin, ie: max delay, fft
   // size, ... and the plugins needs to re-allocate its working buffers, then it should call
   // host->request_restart(), and perform the change once the plugin is re-activated.
   CLAP_PARAM_IS_AUTOMATABLE = 1 << 5,

   // Does this parameter support per note automations?
   CLAP_PARAM_IS_AUTOMATABLE_PER_NOTE_ID = 1 << 6,

   // Does this parameter support per key automations?
   CLAP_PARAM_IS_AUTOMATABLE_PER_KEY = 1 << 7,

   // Does this parameter support per channel automations?
   CLAP_PARAM_IS_AUTOMATABLE_PER_CHANNEL = 1 << 8,

   // Does this parameter support per port automations?
   CLAP_PARAM_IS_AUTOMATABLE_PER_PORT = 1 << 9,

   // Does this parameter support the modulation signal?
   CLAP_PARAM_IS_MODULATABLE = 1 << 10,

   // Does this parameter support per note modulations?
   CLAP_PARAM_IS_MODULATABLE_PER_NOTE_ID = 1 << 11,

   // Does this parameter support per key modulations?
   CLAP_PARAM_IS_MODULATABLE_PER_KEY = 1 << 12,

   // Does this parameter support per channel modulations?
   CLAP_PARAM_IS_MODULATABLE_PER_CHANNEL = 1 << 13,

   // Does this parameter support per port modulations?
   CLAP_PARAM_IS_MODULATABLE_PER_PORT = 1 << 14,

   // Any change to this parameter will affect the plugin output and requires to be done via
   // process() if the plugin is active.
   //
   // A simple example would be a DC Offset, changing it will change the output signal and must be
   // processed.
   CLAP_PARAM_REQUIRES_PROCESS = 1 << 15,
};
typedef uint32_t clap_param_info_flags;

/* This describes a parameter */
typedef struct clap_param_info {
   // stable parameter identifier, it must never change.
   clap_id id;

   clap_param_info_flags flags;

   // This value is optional and set by the plugin.
   // Its purpose is to provide a fast access to the
   // plugin parameter object by caching its pointer.
   // For instance:
   //
   // in clap_plugin_params.get_info():
   //    Parameter *p = findParameter(param_id);
   //    param_info->cookie = p;
   //
   // later, in clap_plugin.process():
   //
   //    Parameter *p = (Parameter *)event->cookie;
   //    if (!p) [[unlikely]]
   //       p = findParameter(event->param_id);
   //
   // where findParameter() is a function the plugin implements
   // to map parameter ids to internal objects.
   //
   // Important:
   //  - The cookie is invalidated by a call to
   //    clap_host_params->rescan(CLAP_PARAM_RESCAN_ALL) or when the plugin is
   //    destroyed.
   //  - The host will either provide the cookie as issued or nullptr
   //    in events addressing parameters.
   //  - The plugin must gracefully handle the case of a cookie
   //    which is nullptr.
   //  - Many plugins will process the parameter events more quickly if the host
   //    can provide the cookie in a faster time than a hashmap lookup per param
   //    per event.
   void *cookie;

   // the display name
   char name[CLAP_NAME_SIZE];

   // the module path containing the param, eg:"oscillators/wt1"
   // '/' will be used as a separator to show a tree like structure.
   char module[CLAP_PATH_SIZE];

   double min_value;     // minimum plain value
   double max_value;     // maximum plain value
   double default_value; // default plain value
} clap_param_info_t;

typedef struct clap_plugin_params {
   // Returns the number of parameters.
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin);

   // Copies the parameter's info to param_info and returns true on success.
   // [main-thread]
   bool(CLAP_ABI *get_info)(const clap_plugin_t *plugin,
                            uint32_t             param_index,
                            clap_param_info_t   *param_info);

   // Gets the parameter plain value.
   // [main-thread]
   bool(CLAP_ABI *get_value)(const clap_plugin_t *plugin, clap_id param_id, double *value);

   // Formats the display text for the given parameter value.
   // The host should always format the parameter value to text using this function
   // before displaying it to the user.
   // [main-thread]
   bool(CLAP_ABI *value_to_text)(
      const clap_plugin_t *plugin, clap_id param_id, double value, char *display, uint32_t size);

   // Converts the display text to a parameter value.
   // [main-thread]
   bool(CLAP_ABI *text_to_value)(const clap_plugin_t *plugin,
                                 clap_id              param_id,
                                 const char          *display,
                                 double              *value);

   // Flushes a set of parameter changes.
   // This method must not be called concurrently to clap_plugin->process().
   //
   // Note: if the plugin is processing, then the process() call will already achieve the
   // parameter update (bi-directional), so a call to flush isn't required, also be aware
   // that the plugin may use the sample offset in process(), while this information would be
   // lost within flush().
   //
   // [active ? audio-thread : main-thread]
   void(CLAP_ABI *flush)(const clap_plugin_t        *plugin,
                         const clap_input_events_t  *in,
                         const clap_output_events_t *out);
} clap_plugin_params_t;

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
   //   - is_per_key (flag)
   //   - is_per_channel (flag)
   //   - is_per_port (flag)
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
   void(CLAP_ABI *rescan)(const clap_host_t *host, clap_param_rescan_flags flags);

   // Clears references to a parameter.
   // [main-thread]
   void(CLAP_ABI *clear)(const clap_host_t *host, clap_id param_id, clap_param_clear_flags flags);

   // Request a parameter flush.
   //
   // The host will then schedule a call to either:
   // - clap_plugin.process()
   // - clap_plugin_params.flush()
   //
   // This function is always safe to use and should not be called from an [audio-thread] as the
   // plugin would already be within process() or flush().
   //
   // [thread-safe,!audio-thread]
   void(CLAP_ABI *request_flush)(const clap_host_t *host);
} clap_host_params_t;

#ifdef __cplusplus
}
#endif
