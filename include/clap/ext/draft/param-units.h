#pragma once

#include "../../plugin.h"

/// @page Parameter Units
///
/// This extension lets the plugin give the host, per parameter, the unit of its value and an
/// optional symbol: a short name that stays the same across versions of the plugin.
///
/// clap_plugin_params.value_to_text() already shows the unit to a person. This extension is for
/// hosts which have no plugin GUI to show and must treat the value as data: a console or a control
/// surface that draws its own controls, an OSC or scripting interface, a host that stores mappings
/// by name. Such a host can't reliably parse "2.3 kHz" back, and a clap_id may change when the
/// plugin is rebuilt.
///
/// The unit applies to the plain value of the parameter: the value clap.params uses for
/// min_value, max_value, default_value, get_value() and the parameter events.
///
/// The unit list is the one of LV2's units extension, one to one and in the same order, so a
/// plugin or a wrapper that moves from LV2 to CLAP keeps units:unit as it is.
///
/// A symbol is either empty or matches [A-Za-z_][A-Za-z0-9_]*, at most
/// CLAP_PARAM_SYMBOL_SIZE - 1 bytes. No two parameters of a plugin share a non-empty symbol. Once
/// a version of the plugin is released, a symbol keeps naming the same parameter in every later
/// version, even if its clap_id or its name change. A host may store the symbol in its own files
/// instead of the clap_id, and find the parameter again by it.

static CLAP_CONSTEXPR const char CLAP_EXT_PARAM_UNITS[] = "clap.param-units/1";

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // Capacity of clap_param_unit_info.symbol, null terminator included.
   CLAP_PARAM_SYMBOL_SIZE = 64,
};

enum clap_param_unit {
   // No unit, or a unit the list below doesn't have.
   CLAP_PARAM_UNIT_NONE = 0,

   // LV2's units, one to one: http://lv2plug.in/ns/extensions/units
   CLAP_PARAM_UNIT_SECONDS = 1,             // units:s
   CLAP_PARAM_UNIT_MILLISECONDS = 2,        // units:ms
   CLAP_PARAM_UNIT_MINUTES = 3,             // units:min
   CLAP_PARAM_UNIT_BARS = 4,                // units:bar
   CLAP_PARAM_UNIT_BEATS = 5,               // units:beat
   CLAP_PARAM_UNIT_FRAMES = 6,              // units:frame, audio frames (samples per channel)
   CLAP_PARAM_UNIT_METRES = 7,              // units:m
   CLAP_PARAM_UNIT_CENTIMETRES = 8,         // units:cm
   CLAP_PARAM_UNIT_MILLIMETRES = 9,         // units:mm
   CLAP_PARAM_UNIT_KILOMETRES = 10,         // units:km
   CLAP_PARAM_UNIT_INCHES = 11,             // units:inch
   CLAP_PARAM_UNIT_MILES = 12,              // units:mile
   CLAP_PARAM_UNIT_DECIBELS = 13,           // units:db
   CLAP_PARAM_UNIT_PERCENT = 14,            // units:pc, 0 to 100
   CLAP_PARAM_UNIT_COEFFICIENT = 15,        // units:coef, a plain multiplier
   CLAP_PARAM_UNIT_HERTZ = 16,              // units:hz
   CLAP_PARAM_UNIT_KILOHERTZ = 17,          // units:khz
   CLAP_PARAM_UNIT_MEGAHERTZ = 18,          // units:mhz
   CLAP_PARAM_UNIT_BPM = 19,                // units:bpm
   CLAP_PARAM_UNIT_OCTAVES = 20,            // units:oct
   CLAP_PARAM_UNIT_CENTS = 21,              // units:cent
   CLAP_PARAM_UNIT_SEMITONES = 22,          // units:semitone12TET
   CLAP_PARAM_UNIT_DEGREES = 23,            // units:degree
   CLAP_PARAM_UNIT_MIDI_NOTE = 24,          // units:midiNote, a MIDI note number

   // Units LV2 doesn't have. New units are only appended here.
   CLAP_PARAM_UNIT_DBFS = 0x100,            // decibels relative to digital full scale
   CLAP_PARAM_UNIT_DBU = 0x101,             // decibels relative to 0.775 V RMS
   CLAP_PARAM_UNIT_RATIO = 0x102,           // n for n:1, eg: a compressor's ratio
   CLAP_PARAM_UNIT_LUFS = 0x103,            // loudness units relative to full scale

   // Note: more entries could be added here in the future. A host reads a unit it doesn't know as
   // CLAP_PARAM_UNIT_NONE.
};

typedef struct clap_param_unit_info {
   // see enum clap_param_unit
   uint32_t unit;

   // A null terminated symbol as described above, or an empty string if the parameter has none.
   char symbol[CLAP_PARAM_SYMBOL_SIZE];
} clap_param_unit_info_t;

typedef struct clap_plugin_param_units {
   // Copies the unit and the symbol of the parameter param_id into info.
   // Returns false if param_id is unknown, or if the parameter has neither a unit nor a symbol.
   // [main-thread]
   bool(CLAP_ABI *get)(const clap_plugin_t *plugin, clap_id param_id, clap_param_unit_info_t *info);
} clap_plugin_param_units_t;

typedef struct clap_host_param_units {
   // Informs the host that the unit of one or more parameters changed, eg: a delay time that the
   // plugin now expresses in beats rather than in milliseconds. The host calls get() again.
   //
   // Note: If the plugin calls clap_host_params.rescan() with CLAP_PARAM_RESCAN_ALL or
   // CLAP_PARAM_RESCAN_INFO, all previously read units and symbols must be considered invalid and
   // the host reads them again. It is thus not necessary for the plugin to call changed() in this
   // case.
   //
   // Note: changed() is for units only. A parameter's symbol never changes during the lifetime of
   // the plugin instance; a rescan may only add or remove parameters with their symbols.
   //
   // [main-thread]
   void(CLAP_ABI *changed)(const clap_host_t *host);
} clap_host_param_units_t;

#ifdef __cplusplus
}
#endif
