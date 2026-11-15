#pragma once

#include "../plugin.h"
#include "../string-sizes.h"

/// @page Note Ports
///
/// This extension provides a way for the plugin to describe its current note ports.
/// If the plugin does not implement this extension, it won't have note input or output.
/// The plugin is only allowed to change its note ports configuration while it is deactivated.

static CLAP_CONSTEXPR const char CLAP_EXT_NOTE_PORTS[] = "clap.note-ports";

#ifdef __cplusplus
extern "C" {
#endif

enum clap_note_dialect {
   // Uses clap_event_note and clap_event_note_expression.
   CLAP_NOTE_DIALECT_CLAP = 1 << 0,

   // Uses clap_event_midi, no polyphonic expression
   CLAP_NOTE_DIALECT_MIDI = 1 << 1,

   // Uses clap_event_midi, with polyphonic expression (MPE)
   CLAP_NOTE_DIALECT_MIDI_MPE = 1 << 2,

   // Uses clap_event_midi2
   CLAP_NOTE_DIALECT_MIDI2 = 1 << 3,
};

typedef struct clap_note_port_info {
   // id identifies a port and must be stable.
   // id may overlap between input and output ports.
   clap_id  id;
   uint32_t supported_dialects;   // bitfield, see clap_note_dialect
   uint32_t preferred_dialect;    // one value of clap_note_dialect
   char     name[CLAP_NAME_SIZE]; // displayable name, i18n?
} clap_note_port_info_t;

// The note ports scan has to be done while the plugin is deactivated.
typedef struct clap_plugin_note_ports {
   // Number of ports, for either input or output.
   // [main-thread]
   uint32_t(CLAP_ABI *count)(const clap_plugin_t *plugin, bool is_input);

   // Get info about a note port.
   // Returns true on success and stores the result into info.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t   *plugin,
                       uint32_t               index,
                       bool                   is_input,
                       clap_note_port_info_t *info);
} clap_plugin_note_ports_t;

enum {
   // The ports have changed, the host shall perform a full scan of the ports.
   // This flag can only be used if the plugin is not active.
   // If the plugin active, call host->request_restart() and then call rescan()
   // when the host calls deactivate()
   CLAP_NOTE_PORTS_RESCAN_ALL = 1 << 0,

   // The ports name did change, the host can scan them right away.
   CLAP_NOTE_PORTS_RESCAN_NAMES = 1 << 1,
};

typedef struct clap_host_note_ports {
   // Query which dialects the host supports
   // [main-thread]
   uint32_t(CLAP_ABI *supported_dialects)(const clap_host_t *host);

   // Rescan the full list of note ports according to the flags.
   // [main-thread]
   void(CLAP_ABI *rescan)(const clap_host_t *host, uint32_t flags);
} clap_host_note_ports_t;

#ifdef __cplusplus
}
#endif
