#pragma once

#include "../../plugin.h"

// This extension reports which note expressions are supported by a plugin.
// If this extension isn't present all note expressions are assumed to be supported.

// TODO : is the clap_supported_note_expressions bitmask future-proof? Or should it be a "count / get-info" pattern?

static CLAP_CONSTEXPR const char CLAP_EXT_NOTE_EXPRESSIONS[] = "clap.note-expressions.draft/0";

#ifdef __cplusplus
extern "C" {
#endif

enum clap_supported_note_expressions
{
  CLAP_NOTE_EXPRESSION_MASK_VOLUME = 1 << 0,
  CLAP_NOTE_EXPRESSION_MASK_PAN = 1 << 1,
  CLAP_NOTE_EXPRESSION_MASK_TUNING = 1 << 2,
  CLAP_NOTE_EXPRESSION_MASK_VIBRATO = 1 << 3,
  CLAP_NOTE_EXPRESSION_MASK_EXPRESSION = 1 << 4,
  CLAP_NOTE_EXPRESSION_MASK_BRIGHTNESS = 1 << 5,
  CLAP_NOTE_EXPRESSION_MASK_PRESSURE = 1 << 6,

  CLAP_NOTE_EXPRESSION_MASK_ALL = (1<<7)-1 // just the and of the above
};

/*
  retrieve additional information for the plugin itself, if note expressions are being supported and if there
  is a limit in MIDI channels (to reduce the offered controllers etc. in the host)
*/
typedef struct clap_plugin_note_expressions
{
   // [main-thread]
   uint32_t(CLAP_ABI* supportedNoteExpressions) (const clap_plugin* plugin, int16_t port_index, int16_t channel); // returns a bitmap of clap_supported_note_expressions
} clap_plugin_note_expressions_t;


typedef struct clap_host_note_expressions {
   // port_index = -1 means 'all ports'
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host,
                           int16_t            port_index);
} clap_host_note_expressions_t;

#ifdef __cplusplus
}
#endif