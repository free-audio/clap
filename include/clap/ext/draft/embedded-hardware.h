#pragma once

#include "../../plugin.h"
#include "../../color.h"
#include "../../events.h"
#include "../../id.h"
#include "../../string-sizes.h"

/*
  clap.embedded-hardware

  lets a plugin which runs inside a piece of hardware - a pedal, a desktop box, the
  channel strip of a digital mixing desk with the plugin on an insert, a rack unit, a
  handheld game device where the plugin is the whole app - find out what it
  runs on, where the physical controls are and what is printed on them, and talk to
  them: it receives buttons, knobs, encoders, faders, sticks, triggers, touch and motion,
  and drives leds, led rings and strips, rumble motors and displays. a null result from
  get_extension means there is no hardware: the plugin runs in an ordinary host, and a
  plugin built for hardware must still work there as a plain clap.

    auto *eh = (const clap_host_embedded_hardware_t *)host->get_extension(
        host, CLAP_EXT_EMBEDDED_HARDWARE);
    auto *reg = (const clap_host_event_registry_t *)host->get_extension(host,
  CLAP_EXT_EVENT_REGISTRY); uint16_t space; if (eh && reg && reg->query(host,
  CLAP_EXT_EMBEDDED_HARDWARE, &space))
      ...  // events with header.space_id == space are ours
    // a host offering this extension must offer the event registry and answer the query.
    // the query is the plugin's opt in: see -- input --

  the device: the hardware the plugin runs on. it has zero or more panels - a main
  panel, an expander which may come and go, a game controller plugged in over usb, the
  built-in controls of a handheld - and each panel carries elements: the physical things
  a user touches, looks at or feels. a device with no panels still describes itself.

  coordinates: each panel has its own origin at its top left corner as the user faces
  it, x to the right and y downwards, in millimetres. an element is placed by its
  centre, its bounding width and height and a clockwise rotation in degrees about that
  centre. a panel is itself placed on the device's front by x_mm/y_mm, so a plugin which
  wants to draw the whole device can. a panel or element whose host cannot measure it -
  a generic usb pad, a motion sensor - says so with NO_GEOMETRY and has zeros there.
  values follow conventions of their own, stated once here: a stick's and a motion
  sensor's y runs up, and motion's z out of the front, as on every game controller;
  everything placed on a panel, touch points included, has y running down.

  identity: panel and element ids are chosen by the device and unique across the whole
  device; never enumeration indices. for everything the device can recognise - its own
  panels, an expander by its serial or slot - they are stable across boots, firmware
  updates and re-attaching, so a plugin may persist them, e.g. in a user's control
  assignment. a unit the device cannot tell from another of its kind, typically a
  generic usb pad, gets fresh ids on every attach, never reused while the host runs, so
  that no stale id ever addresses a different controller.

  roles: an element's id says which control it is on this device; its role, if it has
  one, says what it is on any device - the south face button, the left stick, the
  status light - so a plugin can ask for "the d-pad" without knowing the device. roles
  are read per direction: inputs take values from clap_embedded_hardware_input_role,
  outputs from clap_embedded_hardware_output_role, since the two overlap
  (DPAD_UP and RUMBLE are both 0). NO_ROLE is UINT32_MAX, never 0.

  composite parts: an encoder with a push switch and an led ring around it is three
  elements, the encoder, a button and an led ring, the latter two with parent_id set to
  the encoder, which has the PUSHABLE flag. a stick's click, the ring round a stick, a
  scribble display beside a knob and a touch screen over a display are linked the same
  way. each part keeps its own kind and events, so a plugin which only knows buttons
  still works the push switch.

  printed labels: label is what is printed on or next to the element ("Mix", "A", "△"),
  as the user reads it, in utf-8 with symbols where unicode has them; alt_label is a
  second print, typically a shift function. label_color, cap_color and label_position
  say how the print looks and where it sits, so a game can prompt "press A" in the
  right colour and a host can draw legends. a print never changes; what changes goes on
  a display.

  controls the host keeps for itself (power, often volume) are not listed at all; a
  control the host lists but also acts on (menu + volume for brightness, a charging
  light) has HOST_SHARED, and a plugin should not bind anything critical to it.

  properties: what the lists do not say, the host answers by key in get_property(),
  e.g. the colour of the case, so a plugin can match its screen to it. keys are lower
  case with underscores; the ones this header names are the
  CLAP_EMBEDDED_HARDWARE_PROPERTY_ defines, and a vendor's own keys start with
  its reverse domain name ("com.example.amp_model"). values are utf-8, in the format
  the key states; colours are "#rrggbb". a property is asked of the device or of one
  panel, and a panel without a value of its own answers with the device's.

  -- input --

  inputs arrive as events in the space the event registry returns for
  CLAP_EXT_EMBEDDED_HARDWARE: in process() like any other event, sample timed, or
  on the main thread through the plugin's process_main_thread(), or both. the plugin
  chooses per element and per event type in input_routing(), so a knob's VALUE can go
  to process() while its TOUCH goes to the main thread, and a menu button can stay away
  from the audio thread altogether. with both threads, each gets its own copy, with no
  ordering between the two.

  a plugin opts in by asking the event registry for the space. until a plugin instance
  has asked, the host sends it no event in this space, not even a RESET, and does not
  call its input_routing(): a plain clap may not check header.space_id, and would take
  a button for a note. once it has asked, a plugin without the plugin side of this
  extension gets BUTTON, POSITION, VALUE, DELTA and TOUCH in process(), and the
  high-rate kinds (AXIS, MOTION, TOUCH_POINT) nowhere, so it is not flooded on a
  handheld.

  every input event carries timestamp_ns, the time the hardware reported it on the
  clock get_time_ns() reads, the same for the copy on each thread. live events - a
  finger moving something now - have CLAP_EVENT_IS_LIVE in header.flags; events which
  replay a state after a RESET do not.

  RESET: forget all held state of one panel (or of every panel, for CLAP_INVALID_ID) on
  the thread it arrives on: every button released, every touch gone, nothing moving.
  replayed, non-live events for the current state of whatever is routed to that thread
  follow immediately: a BUTTON for every button held, a TOUCH for every element
  touched, the POSITION of every switch, the VALUE of every pot, fader, pedal, touch
  strip and analog button, the AXIS of every stick away from its centre and a
  TOUCH_POINT BEGIN for every finger down. between two RESETs, every press on a thread
  is followed by exactly one release on that thread. a host sends RESET
  - for every panel when delivery starts on a thread: on the audio thread at the first
    process() after each start_processing(), or after the plugin asked for the space if
    it was already processing; on the main thread when input_routing() has first been
    asked;
  - for a panel when it is attached (then its state follows) and when it is removed
    (with nothing following);
  - on a thread an element or event type was routed away from, after input_routing()
    is asked again;
  - after resume();
  and may send one at any other time; a plugin handles a RESET whenever it comes.
  events for the audio thread which arrive while the plugin is not processing are
  dropped, not queued; the RESET at the next start restores what matters.

  get_input_state() reads where an input is now on the main thread, for a plugin which
  wants it before any event.

  -- output --

  the plugin drives outputs with events in the same space: PIXELS for leds, rings and
  strips, RUMBLE for motors and TEXT for displays. it pushes them into process()'s
  out_events or into process_main_thread()'s out queue, whichever thread it is on.
  set_pixels() does what PIXELS does, straight from the main thread, without waiting
  for a process_main_thread() call. every way writes the same state, and the most
  recent write wins; lights can be slow, so a host may rate limit and show only the
  latest colour of each pixel. outputs the host also drives (HOST_SHARED) may be
  overridden by it. when the plugin is destroyed the host stops its rumble and takes
  its lights back; on suspend it turns them off, on resume it restores what the plugin
  last set.

  the plugin says which outputs it takes, and from which threads, in output_routing(),
  as it says where inputs go in input_routing(): ROUTE_PROCESS for out_events,
  ROUTE_MAIN_THREAD for process_main_thread()'s out queue and set_pixels(). an output
  it takes is the plugin's from then: the host stops driving it, a status light or a
  legend of its own, and shows what the plugin sends, dark or still until it does. an
  output routed 0 stays the host's, which drives it as it would with no plugin. the
  host ignores an output event from a thread the plugin did not route it to, and
  set_pixels() returns false for it. an output routed away again goes back to the
  host, as at destroy. until the plugin has asked for the event space every output is
  the host's; a plugin without the plugin side, which cannot be asked, takes every
  output from both threads. the host asks output_routing() before delivery starts, and
  ignores what the plugin sends before it has asked, which is often from inside init():
  a plugin sends its outputs' first state when delivery has started, at its first
  RESET on either thread.

  -- focus --

  a host may put something else in front of a panel's outputs for a while - its own
  menu, a volume overlay, another plugin brought forward on a shared surface - while
  the plugin keeps running and processing. it says so with a FOCUS event, focused 0,
  and again with focused 1 when the plugin's outputs show again. FOCUS is a hint: a
  plugin without focus may stop sending PIXELS, TEXT and RUMBLE for that panel and stop
  requesting frames for its displays there, but need not. the host still takes every
  output, keeps the latest state and shows it when focus returns, so a plugin which
  skipped updates sends what changed then, and requests a frame for its displays.
  focus is about outputs only: a host which also takes a panel's inputs for its menu
  sends a RESET for the panel when it gives them back.

  a panel has focus when delivery starts. FOCUS goes, like RESET, to process() and,
  for a plugin with the plugin side, to process_main_thread(), whatever the routing,
  and only to a plugin which has asked for the event space. a RESET of a panel without
  focus is followed by FOCUS 0 for it, so a plugin learns the focus with the state.

  -- displays --

  each DISPLAY element is a surface of its own; a device may have none, one main screen,
  or several, e.g. a small one beside some knob (parent_id is then that knob). there
  are two kinds:
  - TEXT: a character display of columns x rows. it shows TEXT events, drawn by the
    host in the device's own characters.
  - GRAPHIC: a pixel display of width x height. it shows TEXT events drawn by the host
    in the device's font, or what the plugin draws: the plugin claims the surface in
    display_open() and then draws it through one api. a surface has one owner; while
    the plugin owns it the host ignores TEXT for that surface and draws over it only
    for its own system overlays (volume, battery). a surface the plugin does not claim
    stays the host's, e.g. for its own legends.

  apis: how the plugin draws a claimed surface. an api is named by a string, as
  clap.gui names window apis, with a version of its own, so one api can change without
  the extension's id changing: CLAP_EMBEDDED_HARDWARE_DISPLAY_API_PIXELS is
  "pixels/1". an api a vendor adds starts with its reverse domain name
  ("com.example.sdl-renderer/1"). the host offers an api through display_open(), whose
  config names it: one the host supports (display_is_api_supported) and, if the plugin
  has one, its choice (display_get_preferred_api). a plugin which cannot draw that api
  returns false from display_open(), and the host may offer another. every host
  supports pixels/1 on every GRAPHIC display, so a plugin which draws pixels/1 draws
  anywhere.

  an api hands the plugin a surface twice: once in the config, for as long as the
  display is open (a gl context's get_proc_address, a drm lease), and once in each
  frame display_render() gets (a pixel buffer, a framebuffer). each is a union of
  pointers to that api's own structs, read by config.api and frame.api, with ptr for an
  api this header does not know; an api which needs nothing there has it null. each
  api says what it puts in them, and whether the host calls display_render() at all:
  an api in which the plugin presents frames itself, an exclusive screen, never gets
  one.

  pixels/1: the plugin draws XRGB8888 into the host's buffer, frame.surface.pixels,
  and the host converts to what the display can show, as display_format says, so a
  1-bit oled is drawn in two colours. config.surface is null. drawing is pull based:
  the plugin calls request_frame() for a surface when it
  has something new to show, and the host calls display_render() for it at most once
  per refresh; apart from one display_render() after each display_set_visible(true)
  the host never renders without a request, so an idle plugin costs nothing. to
  animate, call request_frame() from display_render().

  -- lifecycle --

  suspend: the host calls suspend() while the plugin is still processing, then stops
  processing (and may deactivate), hides every display it owns (display_set_visible
  false, and may display_close), stops rumble and turns the plugin's lights off. the
  device may never wake, so save anything precious. resume: the host calls resume(),
  then restores the plugin's lights, restarts processing, which brings a RESET on the
  audio thread, sends a RESET on the main thread and shows the displays again.
  request_quit() asks the host to end the plugin, back to a launcher or off.

  threading: host functions are [main-thread] except request_process_main_thread,
  request_frame and get_time_ns, which are [thread-safe] and wait-free. the plugin's
  functions are all [main-thread]; display_render() and process_main_thread() may run
  concurrently with process().

  structs: every event, and every struct but the three which carry pointers, has no
  padding and the same layout on 32 and 64-bit abis, with uint32_t in place of bool, so
  header.size and every offset are the same everywhere and no uninitialised byte is
  copied with an event. display_config, frame and pixels_surface are only ever passed
  by pointer, never copied, and follow the abi's pointer size, as clap's own structs
  do. reserved fields are 0.

  not covered: a panel the plugin declares for the host to bind physical
  controls to (keys, encoders, legends);
  curves the host draws on a surface, in the data model of clap.mini-curve-display;
  layers, damage rectangles and plugin-owned buffers for pixels; display apis beyond
  pixels/1, such as gles or an exclusive screen (e.g. kms/drm), which the api
  hook leaves room for; moving motorized faders; a writable data folder, battery and
  background states.
*/

static CLAP_CONSTEXPR const char CLAP_EXT_EMBEDDED_HARDWARE[] = "clap.embedded-hardware/1";

// add to clap_plugin_descriptor.features to mark a plugin built for hardware, so a
// launcher on a device can list those first
#define CLAP_PLUGIN_FEATURE_EMBEDDED_HARDWARE "embedded-hardware"

#ifdef __cplusplus
extern "C" {
#endif

// display apis, see -- displays --. every host supports pixels/1 on a GRAPHIC display
#define CLAP_EMBEDDED_HARDWARE_DISPLAY_API_PIXELS "pixels/1"

// keys for get_property(). the colour of the case, "#rrggbb"; asked of a panel, the
// colour of that panel, e.g. an expander in a colour of its own
#define CLAP_EMBEDDED_HARDWARE_PROPERTY_HOUSING_COLOR "housing_color"

// in a role field: the element has no role
static const CLAP_CONSTEXPR uint32_t CLAP_EMBEDDED_HARDWARE_NO_ROLE = UINT32_MAX;

/* -- the device -- */

typedef struct clap_embedded_hardware_device_info {
   // for display, e.g. "Example Audio", "FamousAudioCompany", "RetroHandheld Inc."
   char vendor[CLAP_NAME_SIZE];

   // for display, e.g. "Demo Box", "RG29XX Plus"
   char product[CLAP_NAME_SIZE];

   // reverse dns, stable for every unit of this model, e.g. "com.example.demobox"
   char model_id[CLAP_NAME_SIZE];

   // the processor, for a plugin which tunes itself to it, e.g. "Allwinner H700"; ""
   // if unknown
   char chip[CLAP_NAME_SIZE];

   // this unit's serial number, "" if the device has none or does not tell
   char serial[CLAP_NAME_SIZE];

   // free form, "" if unknown
   char hardware_revision[CLAP_NAME_SIZE];
   char firmware_version[CLAP_NAME_SIZE];
} clap_embedded_hardware_device_info_t;

/* -- panels -- */

enum clap_embedded_hardware_panel_flags {
   // the panel can be attached or removed while the plugin runs: an expander, a game
   // controller. see hardware_changed and RESET
   CLAP_EMBEDDED_HARDWARE_PANEL_REMOVABLE = 1 << 0,

   // the host does not know the panel's size or where its controls are, e.g. a generic
   // usb pad; sizes and positions on it are 0, and roles are the way to find things
   CLAP_EMBEDDED_HARDWARE_PANEL_NO_GEOMETRY = 1 << 1,
};

typedef struct clap_embedded_hardware_panel_info {
   clap_id id;

   // clap_embedded_hardware_panel_flags
   uint32_t flags;

   // for display, e.g. "Main", "Expander", "8BitDo SN30 Pro"
   char name[CLAP_NAME_SIZE];

   // the panel's size
   double width_mm;
   double height_mm;

   // where the panel's top left corner sits on the device's front, in the same
   // orientation as the panel's own coordinates
   double x_mm;
   double y_mm;
} clap_embedded_hardware_panel_info_t;

/* -- elements -- */

enum clap_embedded_hardware_element_kind {
   // inputs

   // a push button; see MOMENTARY/LATCHING, VELOCITY, PRESSURE, ANALOG. sends BUTTON
   CLAP_EMBEDDED_HARDWARE_BUTTON = 0,

   // a switch with `steps` fixed positions, 2 for a toggle, a lid or a headphone jack.
   // sends POSITION
   CLAP_EMBEDDED_HARDWARE_SWITCH = 1,

   // an absolute rotary control, a potentiometer. sends VALUE
   CLAP_EMBEDDED_HARDWARE_POT = 2,

   // an absolute linear control. sends VALUE
   CLAP_EMBEDDED_HARDWARE_FADER = 3,

   // an endless relative rotary control. sends DELTA; its push switch, if PUSHABLE, is a
   // BUTTON whose parent is the encoder
   CLAP_EMBEDDED_HARDWARE_ENCODER = 4,

   // a touch sensitive strip, one finger; sends VALUE for the position and TOUCH
   CLAP_EMBEDDED_HARDWARE_TOUCH_STRIP = 5,

   // an expression pedal, usually through a jack. sends VALUE
   CLAP_EMBEDDED_HARDWARE_PEDAL = 6,

   // an analog stick, axis_count axes, springing back to its centre. sends AXIS; its
   // click is a BUTTON whose parent is the stick
   CLAP_EMBEDDED_HARDWARE_STICK = 7,

   // a touch screen or pad, up to max_touch_points fingers. sends TOUCH_POINT. one laid
   // over a display has that display as its parent
   CLAP_EMBEDDED_HARDWARE_TOUCH_SURFACE = 8,

   // an accelerometer or gyroscope, see its role; axis_count axes. sends MOTION
   CLAP_EMBEDDED_HARDWARE_MOTION = 9,

   // outputs

   // a single led
   CLAP_EMBEDDED_HARDWARE_LED = 64,

   // pixel_count leds on an arc, see arc_start_deg and arc_sweep_deg
   CLAP_EMBEDDED_HARDWARE_LED_RING = 65,

   // pixel_count leds on a line through the element's centre, along its width and
   // turned by rotation_deg. pixel 0 is at the left end before rotation
   CLAP_EMBEDDED_HARDWARE_LED_STRIP = 66,

   // a display, see display_kind
   CLAP_EMBEDDED_HARDWARE_DISPLAY = 67,

   // a vibration motor. takes RUMBLE
   CLAP_EMBEDDED_HARDWARE_RUMBLE = 68,
};

enum clap_embedded_hardware_shape {
   CLAP_EMBEDDED_HARDWARE_SHAPE_RECT = 0,

   // an ellipse inscribed in the bounding box; a circle when width equals height
   CLAP_EMBEDDED_HARDWARE_SHAPE_ROUND = 1,
};

enum clap_embedded_hardware_element_flags {
   // BUTTON: pressed for as long as it is held. the default
   CLAP_EMBEDDED_HARDWARE_MOMENTARY = 1 << 0,

   // BUTTON: mechanically stays down until pressed again
   CLAP_EMBEDDED_HARDWARE_LATCHING = 1 << 1,

   // BUTTON: velocity in BUTTON events is meaningful
   CLAP_EMBEDDED_HARDWARE_VELOCITY = 1 << 2,

   // BUTTON: the element reports pressure while held, as VALUE events
   CLAP_EMBEDDED_HARDWARE_PRESSURE = 1 << 3,

   // any input: the element senses a finger on it and sends TOUCH, e.g. a
   // capacitive knob
   CLAP_EMBEDDED_HARDWARE_TOUCH_SENSE = 1 << 4,

   // POT, FADER: the device can move it. there is no way to ask it to; the flag tells
   // the plugin that the control may move under its own power
   CLAP_EMBEDDED_HARDWARE_MOTORIZED = 1 << 5,

   // POT, ENCODER: clicks into `steps` detents per travel or revolution
   CLAP_EMBEDDED_HARDWARE_DETENTED = 1 << 6,

   // POT, FADER: has a single detent in the centre of its travel
   CLAP_EMBEDDED_HARDWARE_CENTER_DETENT = 1 << 7,

   // BUTTON: an analog trigger. sends VALUE, 0 released to 1 fully pulled, over its
   // whole travel, and BUTTON press and release at a threshold the host chooses
   CLAP_EMBEDDED_HARDWARE_ANALOG = 1 << 8,

   // any element: the host also acts on it, e.g. menu + volume for brightness, a
   // charging light. inputs are still delivered and outputs still taken, but the host
   // may override an output, and a plugin should not bind critical actions to an input
   CLAP_EMBEDDED_HARDWARE_HOST_SHARED = 1 << 9,

   // any element: its position is unknown, see PANEL_NO_GEOMETRY; x, y, width,
   // height and rotation are 0
   CLAP_EMBEDDED_HARDWARE_NO_GEOMETRY = 1 << 10,

   // ENCODER, POT, STICK: the control can be pushed. the push switch is a BUTTON element
   // of its own with this element as parent_id, which has the id and sends the events;
   // the flag says it is there without searching the list. a host sets the flag exactly
   // when it lists that BUTTON
   CLAP_EMBEDDED_HARDWARE_PUSHABLE = 1 << 11,
};

enum clap_embedded_hardware_color_model {
   // a single colour, native_color, at brightness_steps levels
   CLAP_EMBEDDED_HARDWARE_COLOR_MONO = 0,

   // two dies, typically red and green, which together make an amber
   CLAP_EMBEDDED_HARDWARE_COLOR_BICOLOR = 1,

   // full colour
   CLAP_EMBEDDED_HARDWARE_COLOR_RGB = 2,
};

// where a print sits, as the user faces the panel
enum clap_embedded_hardware_label_position {
   // unknown, or no print
   CLAP_EMBEDDED_HARDWARE_LABEL_UNKNOWN = 0,

   // on the control itself, e.g. a letter on a face button
   CLAP_EMBEDDED_HARDWARE_LABEL_ON = 1,

   CLAP_EMBEDDED_HARDWARE_LABEL_BELOW = 2,
   CLAP_EMBEDDED_HARDWARE_LABEL_ABOVE = 3,
   CLAP_EMBEDDED_HARDWARE_LABEL_LEFT = 4,
   CLAP_EMBEDDED_HARDWARE_LABEL_RIGHT = 5,
};

enum clap_embedded_hardware_display_kind {
   // not a display
   CLAP_EMBEDDED_HARDWARE_DISPLAY_NONE = 0,

   // a character display: display_width columns by display_height rows. takes TEXT
   CLAP_EMBEDDED_HARDWARE_DISPLAY_TEXT = 1,

   // a pixel display: display_width x display_height pixels. takes TEXT, or the
   // plugin's own pixels through display_open
   CLAP_EMBEDDED_HARDWARE_DISPLAY_GRAPHIC = 2,
};

// what a display can show natively. the plugin draws XRGB8888 whatever this says, and
// the host converts
enum clap_embedded_hardware_display_format {
   // TEXT displays, and unknown
   CLAP_EMBEDDED_HARDWARE_FORMAT_NONE = 0,

   // on or off, in native_color, e.g. a monochrome oled
   CLAP_EMBEDDED_HARDWARE_FORMAT_MONO1 = 1,

   // 256 levels of native_color
   CLAP_EMBEDDED_HARDWARE_FORMAT_GRAY8 = 2,

   // 16 bit colour, 5 bits red, 6 green, 5 blue
   CLAP_EMBEDDED_HARDWARE_FORMAT_RGB565 = 3,

   // 32 bit native endian 0xXXRRGGBB, as DRM_FORMAT_XRGB8888; X is ignored
   CLAP_EMBEDDED_HARDWARE_FORMAT_XRGB8888 = 4,
};

typedef struct clap_embedded_hardware_element_info {
   clap_id id;
   clap_id panel_id;

   // the element this one is part of, e.g. the encoder an led ring surrounds, the
   // stick a click belongs to, the knob a scribble display labels; CLAP_INVALID_ID
   // if none
   clap_id parent_id;

   // clap_embedded_hardware_element_kind
   uint32_t kind;

   // what the element is on any device: an input_role for inputs, an output_role for
   // outputs; CLAP_EMBEDDED_HARDWARE_NO_ROLE if none
   uint32_t role;

   // clap_embedded_hardware_element_flags
   uint32_t flags;

   // the centre of the element on its panel, and its bounding box before rotation
   double x_mm;
   double y_mm;
   double width_mm;
   double height_mm;

   // clockwise, about the centre
   double rotation_deg;

   // clap_embedded_hardware_shape
   uint32_t shape;

   // SWITCH: the number of positions. POT, FADER, PEDAL, TOUCH_STRIP: the resolution
   // the device reads at. ENCODER: detents, or steps, per revolution. 0 if unknown
   uint32_t steps;

   // what is printed on or next to the element, "" if nothing
   char label[CLAP_NAME_SIZE];

   // a secondary print, typically a shift function, "" if none
   char alt_label[CLAP_NAME_SIZE];

   // the colour of the print, e.g. a green "A"; alpha 0 if plain or unknown
   clap_color_t label_color;

   // the colour of the control itself, e.g. a red record button; alpha 0 if unknown
   clap_color_t cap_color;

   // clap_embedded_hardware_label_position
   uint32_t label_position;

   // STICK: 2, x and y. MOTION: 3. 0 otherwise
   uint32_t axis_count;

   // TOUCH_SURFACE: the most fingers it tracks at once. 0 otherwise
   uint32_t max_touch_points;

   // LED, LED_RING, LED_STRIP, and DISPLAY in MONO1 or GRAY8:
   // clap_embedded_hardware_color_model
   uint32_t color_model;

   // the colour of a MONO led or a monochrome display when fully lit; alpha is unused
   clap_color_t native_color;

   // outputs: distinct brightness levels per colour channel, 2 for plain on/off.
   // RUMBLE: distinct intensities, 2 for plain on/off
   uint32_t brightness_steps;

   // LED: 1. LED_RING, LED_STRIP: the number of leds
   uint32_t pixel_count;

   uint32_t reserved0;

   // LED_RING: where pixel 0 sits, in degrees clockwise from 12 o'clock, and how far
   // the ring runs, clockwise, to the centre of the last pixel. a full ring of n
   // pixels starting at the top has 0 and 360 * (n - 1) / n
   double arc_start_deg;
   double arc_sweep_deg;

   // DISPLAY: clap_embedded_hardware_display_kind; NONE for anything else
   uint32_t display_kind;

   // DISPLAY: TEXT: columns and rows. GRAPHIC: pixels, as the user sees the display;
   // the host hides any rotation the panel is mounted with
   uint32_t display_width;
   uint32_t display_height;

   // DISPLAY: clap_embedded_hardware_display_format
   uint32_t display_format;

   uint32_t reserved1[4];
} clap_embedded_hardware_element_info_t;

typedef struct clap_embedded_hardware_input_state {
   // BUTTON: held, 0 or 1
   uint32_t pressed;

   // TOUCH_STRIP, TOUCH_SURFACE, and any element with TOUCH_SENSE: a finger is on it
   uint32_t touched;

   // SWITCH: the position, 0..steps-1
   uint32_t position;

   uint32_t reserved;

   // POT, FADER, PEDAL, TOUCH_STRIP: the position, 0..1. BUTTON with PRESSURE or
   // ANALOG: the pressure or pull
   double value;

   // STICK, MOTION: the latest values, as AXIS and MOTION carry them
   double axes[3];
} clap_embedded_hardware_input_state_t;

/* -- roles -- */

// what an input is on any device. face buttons are named by position, not by printed letter,
// because the letters move between layouts: most handhelds use the snes layout (A east, B south)
// while xbox style devices put A south. the print is in label
enum clap_embedded_hardware_input_role {
   // d-pad
   CLAP_EMBEDDED_HARDWARE_INPUT_DPAD_UP = 0,
   CLAP_EMBEDDED_HARDWARE_INPUT_DPAD_DOWN = 1,
   CLAP_EMBEDDED_HARDWARE_INPUT_DPAD_LEFT = 2,
   CLAP_EMBEDDED_HARDWARE_INPUT_DPAD_RIGHT = 3,

   // face buttons, by position in the diamond
   CLAP_EMBEDDED_HARDWARE_INPUT_FACE_SOUTH = 16,
   CLAP_EMBEDDED_HARDWARE_INPUT_FACE_EAST = 17,
   CLAP_EMBEDDED_HARDWARE_INPUT_FACE_WEST = 18,
   CLAP_EMBEDDED_HARDWARE_INPUT_FACE_NORTH = 19,
   // six-button layouts (saturn / mega drive style C and Z)
   CLAP_EMBEDDED_HARDWARE_INPUT_FACE_EXTRA_1 = 20,
   CLAP_EMBEDDED_HARDWARE_INPUT_FACE_EXTRA_2 = 21,

   // shoulders and triggers; L2 and R2 may be ANALOG
   CLAP_EMBEDDED_HARDWARE_INPUT_L1 = 32,
   CLAP_EMBEDDED_HARDWARE_INPUT_R1 = 33,
   CLAP_EMBEDDED_HARDWARE_INPUT_L2 = 34,
   CLAP_EMBEDDED_HARDWARE_INPUT_R2 = 35,
   // rear grip buttons or paddles
   CLAP_EMBEDDED_HARDWARE_INPUT_BACK_L1 = 36,
   CLAP_EMBEDDED_HARDWARE_INPUT_BACK_R1 = 37,
   CLAP_EMBEDDED_HARDWARE_INPUT_BACK_L2 = 38,
   CLAP_EMBEDDED_HARDWARE_INPUT_BACK_R2 = 39,

   // STICK elements; their clicks (L3 / R3) are BUTTONs with the stick as parent
   CLAP_EMBEDDED_HARDWARE_INPUT_STICK_L = 48,
   CLAP_EMBEDDED_HARDWARE_INPUT_STICK_R = 49,
   CLAP_EMBEDDED_HARDWARE_INPUT_STICK_L_CLICK = 50,
   CLAP_EMBEDDED_HARDWARE_INPUT_STICK_R_CLICK = 51,

   // system buttons
   CLAP_EMBEDDED_HARDWARE_INPUT_START = 64,
   CLAP_EMBEDDED_HARDWARE_INPUT_SELECT = 65,
   CLAP_EMBEDDED_HARDWARE_INPUT_MENU = 66,     // menu, home, guide, "M"
   CLAP_EMBEDDED_HARDWARE_INPUT_FUNCTION = 67, // a second system button, "F", "FN"
   CLAP_EMBEDDED_HARDWARE_INPUT_POWER = 68,
   CLAP_EMBEDDED_HARDWARE_INPUT_RESET = 69,
   CLAP_EMBEDDED_HARDWARE_INPUT_VOLUME_UP = 70,
   CLAP_EMBEDDED_HARDWARE_INPUT_VOLUME_DOWN = 71,

   // SWITCHes with 2 steps, sensing rather than played: position 1 is engaged
   CLAP_EMBEDDED_HARDWARE_INPUT_LID = 80,             // 1 = lid closed
   CLAP_EMBEDDED_HARDWARE_INPUT_HEADPHONES = 81,      // 1 = plug inserted
   CLAP_EMBEDDED_HARDWARE_INPUT_FUNCTION_SWITCH = 82, // two position slider, 1 = on

   // surfaces and sensors
   CLAP_EMBEDDED_HARDWARE_INPUT_TOUCHSCREEN = 96,
   CLAP_EMBEDDED_HARDWARE_INPUT_ACCELEROMETER = 97,
   CLAP_EMBEDDED_HARDWARE_INPUT_GYROSCOPE = 98,

   // an element without a role is found by its label, kind and position
};

// what an output is on any device
enum clap_embedded_hardware_output_role {
   // the only motor, or the heavy low frequency one of a pair
   CLAP_EMBEDDED_HARDWARE_OUTPUT_RUMBLE = 0,
   // the light high frequency motor of a pair
   CLAP_EMBEDDED_HARDWARE_OUTPUT_RUMBLE_HIGH = 1,

   // power / charge / notification light
   CLAP_EMBEDDED_HARDWARE_OUTPUT_LED_STATUS = 16,
   // rings around the sticks, LED_RINGs with the stick as parent
   CLAP_EMBEDDED_HARDWARE_OUTPUT_LED_STICK_L = 17,
   CLAP_EMBEDDED_HARDWARE_OUTPUT_LED_STICK_R = 18,
   // light bar on a game controller
   CLAP_EMBEDDED_HARDWARE_OUTPUT_LED_LIGHTBAR = 19,

   // the device's main screen, a DISPLAY
   CLAP_EMBEDDED_HARDWARE_OUTPUT_SCREEN = 48,
};

/* -- events -- */

// common to every event in this space:
// - header.space_id is the id the event registry gives for CLAP_EXT_EMBEDDED_HARDWARE
// - header.time is the sample offset in process(), and 0 on the main thread
// - input events have CLAP_EVENT_IS_LIVE in header.flags when a finger made them now,
//   and not when they replay state after a RESET
// - input events carry timestamp_ns, see get_time_ns
enum clap_embedded_hardware_event_type {
   // inputs

   // clap_embedded_hardware_event_button
   CLAP_EMBEDDED_HARDWARE_EVENT_BUTTON = 0,

   // clap_embedded_hardware_event_position
   CLAP_EMBEDDED_HARDWARE_EVENT_POSITION = 1,

   // clap_embedded_hardware_event_value
   CLAP_EMBEDDED_HARDWARE_EVENT_VALUE = 2,

   // clap_embedded_hardware_event_delta
   CLAP_EMBEDDED_HARDWARE_EVENT_DELTA = 3,

   // clap_embedded_hardware_event_touch
   CLAP_EMBEDDED_HARDWARE_EVENT_TOUCH = 4,

   // clap_embedded_hardware_event_axis, from a STICK
   CLAP_EMBEDDED_HARDWARE_EVENT_AXIS = 5,

   // clap_embedded_hardware_event_axis, from a MOTION sensor; apart from AXIS
   // so a plugin can leave out a high rate stream
   CLAP_EMBEDDED_HARDWARE_EVENT_MOTION = 6,

   // clap_embedded_hardware_event_touch_point, from a TOUCH_SURFACE
   CLAP_EMBEDDED_HARDWARE_EVENT_TOUCH_POINT = 7,

   // clap_embedded_hardware_event_reset; delivered to every thread which gets
   // any event of the panel, whatever the routing says
   CLAP_EMBEDDED_HARDWARE_EVENT_RESET = 8,

   // clap_embedded_hardware_event_focus; delivered as RESET is
   CLAP_EMBEDDED_HARDWARE_EVENT_FOCUS = 9,

   // outputs

   // clap_embedded_hardware_event_pixels
   CLAP_EMBEDDED_HARDWARE_EVENT_PIXELS = 16,

   // clap_embedded_hardware_event_rumble
   CLAP_EMBEDDED_HARDWARE_EVENT_RUMBLE = 17,

   // clap_embedded_hardware_event_text
   CLAP_EMBEDDED_HARDWARE_EVENT_TEXT = 18,
};

typedef struct clap_embedded_hardware_event_button {
   clap_event_header_t header;

   clap_id element_id;

   // 1 pressed, 0 released
   uint32_t pressed;

   uint64_t timestamp_ns;

   // 0..1, on press; 1 for an element without VELOCITY and on release
   double velocity;
} clap_embedded_hardware_event_button_t;

typedef struct clap_embedded_hardware_event_position {
   clap_event_header_t header;

   clap_id element_id;

   // 0..steps-1
   uint32_t position;

   uint64_t timestamp_ns;
} clap_embedded_hardware_event_position_t;

typedef struct clap_embedded_hardware_event_value {
   clap_event_header_t header;

   clap_id  element_id;
   uint32_t reserved;
   uint64_t timestamp_ns;

   // 0..1: the left or bottom end of the travel is 0, a pot fully anti-clockwise is
   // 0, a released trigger is 0
   double value;
} clap_embedded_hardware_event_value_t;

typedef struct clap_embedded_hardware_event_delta {
   clap_event_header_t header;

   clap_id element_id;

   // positive is clockwise; in the element's steps, so a detented encoder sends one
   // per click. accumulated if the device saw several since the last event
   int32_t steps;

   uint64_t timestamp_ns;
} clap_embedded_hardware_event_delta_t;

typedef struct clap_embedded_hardware_event_touch {
   clap_event_header_t header;

   clap_id element_id;

   // 1 a finger is on it, 0 it left
   uint32_t touched;

   uint64_t timestamp_ns;
} clap_embedded_hardware_event_touch_t;

// every axis of an input at once, so a stick moving diagonally is one event
// - STICK: [0] x, -1 left to +1 right; [1] y, -1 down to +1 up; centre calibrated,
//   no dead zone applied
// - MOTION, ACCELEROMETER: m/s^2, including gravity as phones report it, so a device
//   held upright and still reads about +9.81 on y; GYROSCOPE: rad/s, anti-clockwise
//   positive about each axis. [0] x right, [1] y up, [2] z out of the front
// unused entries are 0. the host may coalesce consecutive moves of one input
typedef struct clap_embedded_hardware_event_axis {
   clap_event_header_t header;

   clap_id element_id;

   // the element's axis_count
   uint32_t axis_count;

   uint64_t timestamp_ns;
   double   values[3];
} clap_embedded_hardware_event_axis_t;

enum clap_embedded_hardware_touch_phase {
   CLAP_EMBEDDED_HARDWARE_TOUCH_BEGIN = 0,
   CLAP_EMBEDDED_HARDWARE_TOUCH_MOVE = 1,
   CLAP_EMBEDDED_HARDWARE_TOUCH_END = 2,
};

// one finger on a TOUCH_SURFACE. x and y run 0..1 across the element, x right and y
// down. on a surface laid over a display they run across that display as the user
// sees it, or across the plugin's canvas when the host scales one up (see
// display_get_preferred_size): touches beginning outside the canvas are not sent, and
// a touch in progress may move outside 0..1
typedef struct clap_embedded_hardware_event_touch_point {
   clap_event_header_t header;

   clap_id element_id;

   // stable from BEGIN to END, reused afterwards
   uint32_t touch_id;

   uint64_t timestamp_ns;

   // clap_embedded_hardware_touch_phase
   uint32_t phase;
   uint32_t reserved;

   double x;
   double y;
} clap_embedded_hardware_event_touch_point_t;

// see RESET in the comment at the top
typedef struct clap_embedded_hardware_event_reset {
   clap_event_header_t header;

   // the panel whose state to forget, CLAP_INVALID_ID for every panel. a panel no
   // longer in the lists has been removed
   clap_id panel_id;

   uint32_t reserved;
   uint64_t timestamp_ns;
} clap_embedded_hardware_event_reset_t;

typedef struct clap_embedded_hardware_event_focus {
   clap_event_header_t header;

   // the panel, CLAP_INVALID_ID for every panel
   clap_id panel_id;

   // 1: the plugin's outputs on the panel are shown. 0: they may not be, for now
   uint32_t focused;
   uint64_t timestamp_ns;
} clap_embedded_hardware_event_focus_t;

enum clap_embedded_hardware_pixels_mode {
   // the event carries count colours, one per pixel
   CLAP_EMBEDDED_HARDWARE_PIXELS_EACH = 0,

   // the event carries one colour for all count pixels; first = 0 and count =
   // UINT32_MAX set every pixel of the element
   CLAP_EMBEDDED_HARDWARE_PIXELS_FILL = 1,
};

// sets pixels first .. first + count - 1 of an LED, LED_RING or LED_STRIP; the rest
// keep their colour, and pixels beyond the element's pixel_count are ignored.
//
// the colours follow the struct in the same event, so header.size is
// CLAP_EMBEDDED_HARDWARE_PIXELS_SIZE(count) for EACH and
// CLAP_EMBEDDED_HARDWARE_PIXELS_SIZE(1) for FILL; a host reads no colour past
// header.size. there is no cap on count, but an out queue may refuse a large event,
// so a plugin keeps them to what it needs.
//
// colours: alpha is the brightness, red/green/blue the colour. a MONO led lights at
// alpha scaled by the brightest of red, green and blue, so 255,255,255,255 is fully on
// and 0,0,0,0 off for every model. a BICOLOR led uses red and green
typedef struct clap_embedded_hardware_event_pixels {
   clap_event_header_t header;

   clap_id  element_id;
   uint32_t first;
   uint32_t count;

   // clap_embedded_hardware_pixels_mode
   uint32_t mode;

   // followed by the colours, clap_color_t each
} clap_embedded_hardware_event_pixels_t;

#define CLAP_EMBEDDED_HARDWARE_PIXELS_SIZE(count)                                                  \
   ((uint32_t)(sizeof(clap_embedded_hardware_event_pixels_t) + (count) * sizeof(clap_color_t)))

static inline clap_color_t *
clap_embedded_hardware_pixels_colors(clap_embedded_hardware_event_pixels_t *ev) {
   return (clap_color_t *)(ev + 1);
}

static inline const clap_color_t *
clap_embedded_hardware_pixels_colors_const(const clap_embedded_hardware_event_pixels_t *ev) {
   return (const clap_color_t *)(ev + 1);
}

// runs a RUMBLE element at intensity for duration_ms, replacing whatever it was doing.
// intensity 0 or duration_ms 0 stops it; there is no "run forever". the host may clamp
// long durations. from process(), the host starts it at header.time as closely as the
// hardware allows
typedef struct clap_embedded_hardware_event_rumble {
   clap_event_header_t header;

   clap_id  element_id;
   uint32_t duration_ms;

   // 0..1
   double intensity;
} clap_embedded_hardware_event_rumble_t;

// the longest line a TEXT event carries, terminating zero included
#define CLAP_EMBEDDED_HARDWARE_TEXT_SIZE 64

// sets one line of a display the host draws: any TEXT display, or a GRAPHIC one the
// plugin has not claimed. the line is replaced whole; the host cuts it at the
// display's width and shows what the device's characters or font can show. a line
// past the display's rows is ignored, and line UINT32_MAX clears every line
typedef struct clap_embedded_hardware_event_text {
   clap_event_header_t header;

   clap_id  element_id;
   uint32_t line;

   // utf-8, zero terminated
   char text[CLAP_EMBEDDED_HARDWARE_TEXT_SIZE];
} clap_embedded_hardware_event_text_t;

/* -- displays the plugin draws -- */

// pixels/1's surface in a frame: the host's buffer, valid only during display_render
typedef struct clap_embedded_hardware_pixels_surface {
   void    *data;
   uint32_t stride_bytes;

   // clap_embedded_hardware_display_format: XRGB8888 in pixels/1
   uint32_t format;

   // 0: the contents are undefined, draw everything. n: the buffer holds the frame
   // presented n frames ago, so only what changed since needs redrawing
   uint32_t buffer_age;
   uint32_t reserved;
} clap_embedded_hardware_pixels_surface_t;

typedef struct clap_embedded_hardware_display_config {
   // the api the host offers, a CLAP_EMBEDDED_HARDWARE_DISPLAY_API_ string or a
   // vendor's; the plugin compares it by value, as strcmp does
   const char *api;

   // what the api hands over for as long as the display is open, read by api; null
   // for pixels/1
   union {
      const void *ptr;
   } surface;

   // the canvas the plugin draws, in pixels: the display's size, or the size the
   // plugin asked for in display_get_preferred_size if the host scales it up
   uint32_t width;
   uint32_t height;

   // nominal refresh of the display
   double refresh_rate_hz;

   uint32_t reserved[4];
} clap_embedded_hardware_display_config_t;

typedef struct clap_embedded_hardware_frame {
   // as in the config
   const char *api;

   // what the api hands over for this frame, read by api
   union {
      // pixels/1
      const clap_embedded_hardware_pixels_surface_t *pixels;
      const void                                    *ptr;
   } surface;

   // estimate of when this frame reaches the display, on the get_time_ns clock, for
   // animation
   uint64_t present_time_ns;

   // as in the config
   uint32_t width;
   uint32_t height;

   uint32_t reserved[4];
} clap_embedded_hardware_frame_t;

/* -- the host side -- */

typedef struct clap_host_embedded_hardware {
   // returns false if the device does not describe itself
   // [main-thread]
   bool(CLAP_ABI *get_device_info)(const clap_host_t                    *host,
                                   clap_embedded_hardware_device_info_t *info);

   // the panels attached now
   // [main-thread]
   uint32_t(CLAP_ABI *count_panels)(const clap_host_t *host);

   // [main-thread]
   bool(CLAP_ABI *get_panel)(const clap_host_t                   *host,
                             uint32_t                             index,
                             clap_embedded_hardware_panel_info_t *panel);

   // the elements on every panel attached now; filter on panel_id for one panel
   // [main-thread]
   uint32_t(CLAP_ABI *count_elements)(const clap_host_t *host);

   // [main-thread]
   bool(CLAP_ABI *get_element)(const clap_host_t                     *host,
                               uint32_t                               index,
                               clap_embedded_hardware_element_info_t *element);

   // where an input is now. a pot is somewhere before anyone turns it, so a plugin
   // reads this once it has the extension rather than waiting for an event. returns
   // false for an unknown id or an output
   // [main-thread]
   bool(CLAP_ABI *get_input_state)(const clap_host_t                    *host,
                                   clap_id                               element_id,
                                   clap_embedded_hardware_input_state_t *state);

   // the answers from clap_plugin_embedded_hardware.input_routing or
   // output_routing have changed; the host asks again for every input, output and
   // event type, then sends RESET where an input was routed away, and takes back an
   // output routed away
   // [main-thread]
   void(CLAP_ABI *routing_changed)(const clap_host_t *host);

   // asks for a process_main_thread() call soon, e.g. to send a rumble or text from a
   // ui action. wait-free
   // [thread-safe]
   void(CLAP_ABI *request_process_main_thread)(const clap_host_t *host);

   // now, on the monotonic clock timestamp_ns and present_time_ns use, in nanoseconds:
   // CLOCK_MONOTONIC on linux, the performance counter on windows, mach absolute time
   // on macos. wait-free
   // [thread-safe]
   uint64_t(CLAP_ABI *get_time_ns)(const clap_host_t *host);

   // sets pixels first .. first + count - 1 of an output, as a PIXELS event in EACH
   // mode does, but from the main thread, whether or not the plugin is processing.
   // returns false for an unknown id, an element without pixels or one the plugin has
   // not routed ROUTE_MAIN_THREAD in output_routing
   // [main-thread]
   bool(CLAP_ABI *set_pixels)(const clap_host_t  *host,
                              clap_id             element_id,
                              uint32_t            first,
                              uint32_t            count,
                              const clap_color_t *colors);

   // asks for one display_render() of a display the plugin opened, at its next
   // refresh, for an api which renders on request, as pixels/1 does; calls before that
   // coalesce. wait-free, so a meter can ask from the audio thread
   // [thread-safe]
   void(CLAP_ABI *request_frame)(const clap_host_t *host, clap_id display_id);

   // asks the host to end the plugin: back to a launcher, or off. the host tears the
   // plugin down through the usual clap lifecycle and may ask the user first, so the
   // plugin keeps running normally until then. reason is shown to the user if the host
   // can, e.g. a crash message; null or "" for none
   // [main-thread]
   void(CLAP_ABI *request_quit)(const clap_host_t *host, const char *reason);

   // a property of the device (panel_id CLAP_INVALID_ID) or of one panel, by key; see
   // properties above. copies the value, utf-8 and NUL terminated, into value,
   // truncated to capacity bytes, and returns its length without the NUL, as snprintf
   // does, so a plugin can ask again with a bigger buffer. returns -1, with value ""
   // if capacity allows, for a key the host does not know or an unknown panel
   // [main-thread]
   int32_t(CLAP_ABI *get_property)(
      const clap_host_t *host, clap_id panel_id, const char *key, char *value, uint32_t capacity);

   // whether the host can offer this api on a GRAPHIC display; always true for
   // CLAP_EMBEDDED_HARDWARE_DISPLAY_API_PIXELS. see -- displays --
   // [main-thread]
   bool(CLAP_ABI *display_is_api_supported)(const clap_host_t *host,
                                            clap_id            display_id,
                                            const char        *api);
} clap_host_embedded_hardware_t;

/* -- the plugin side -- */

enum clap_embedded_hardware_routing {
   // the events arrive in process()
   CLAP_EMBEDDED_HARDWARE_ROUTE_PROCESS = 1 << 0,

   // the events arrive in process_main_thread(), on the main thread
   CLAP_EMBEDDED_HARDWARE_ROUTE_MAIN_THREAD = 1 << 1,
};

typedef struct clap_plugin_embedded_hardware {
   // where to deliver one event type of one input element, as
   // clap_embedded_hardware_routing flags; 0 drops them, which also tells the
   // host the plugin does not use them for now. asked, once the plugin has asked the
   // event registry for the space (typically in init()), for every input and every
   // event type its kind sends (BUTTON: BUTTON, and VALUE with PRESSURE or ANALOG;
   // SWITCH: POSITION; POT, FADER, PEDAL: VALUE; ENCODER: DELTA; TOUCH_STRIP: VALUE and
   // TOUCH; STICK: AXIS; TOUCH_SURFACE: TOUCH_POINT; MOTION: MOTION; any with
   // TOUCH_SENSE: TOUCH), and again after hardware_changed and routing_changed
   // [main-thread]
   uint32_t(CLAP_ABI *input_routing)(const clap_plugin_t *plugin,
                                     clap_id              element_id,
                                     uint16_t             event_type);

   // which threads the plugin sends one event type of one output element from, as
   // clap_embedded_hardware_routing flags; 0 leaves the output to the host. see
   // -- output --. asked when input_routing is, for every output and every event type
   // it takes (LED, LED_RING, LED_STRIP: PIXELS; RUMBLE: RUMBLE; DISPLAY: TEXT)
   // [main-thread]
   uint32_t(CLAP_ABI *output_routing)(const clap_plugin_t *plugin,
                                      clap_id              element_id,
                                      uint16_t             event_type);

   // exchanges main-thread events, as process() does audio-thread ones: the inputs
   // routed ROUTE_MAIN_THREAD, with header.time 0, in in; outputs (PIXELS, RUMBLE,
   // TEXT) into out. called when there are inputs to deliver, and after
   // request_process_main_thread(), when in may be empty. never called from inside a
   // call the plugin is making. a valid host only puts this extension's events in in,
   // but a plugin checks header.space_id and header.type and skips what it does not know
   // [main-thread]
   void(CLAP_ABI *process_main_thread)(const clap_plugin_t        *plugin,
                                       const clap_input_events_t  *in,
                                       const clap_output_events_t *out);

   // panels came or went; re-read the lists. ids of elements which are still there are
   // unchanged. held state is cleared by the RESET for that panel, not by this call
   // [main-thread]
   void(CLAP_ABI *hardware_changed)(const clap_plugin_t *plugin);

   // a GRAPHIC display: the api the plugin would rather draw it with. assign one of the
   // CLAP_EMBEDDED_HARDWARE_DISPLAY_API_ strings, or a string of the plugin's
   // that lives as long as the plugin, and return true; false for no preference. the
   // host need not honour it; the config says which api it offers
   // [main-thread]
   bool(CLAP_ABI *display_get_preferred_api)(const clap_plugin_t *plugin,
                                             clap_id              display_id,
                                             const char         **api);

   // a GRAPHIC display, drawn with api: on entry width and height are its size. return
   // true and write a smaller size to have the host scale up a fixed canvas, e.g.
   // 320x240 for a pixel art ui; false to draw at the display's size. the host need not
   // honour it; the config has the final size
   // [main-thread]
   bool(CLAP_ABI *display_get_preferred_size)(const clap_plugin_t *plugin,
                                              clap_id              display_id,
                                              const char          *api,
                                              uint32_t            *width,
                                              uint32_t            *height);

   // the host offers a GRAPHIC display, to be drawn with config->api, once it has asked
   // input_routing; true claims it, false leaves it to the host and to TEXT events, or
   // refuses that api, and the host may offer another. allocate drawing resources here
   // [main-thread]
   bool(CLAP_ABI *display_open)(const clap_plugin_t                           *plugin,
                                clap_id                                        display_id,
                                const clap_embedded_hardware_display_config_t *config);

   // shown or hidden, e.g. under a system menu or on suspend. hidden is a good time to
   // stop animating
   // [main-thread]
   void(CLAP_ABI *display_set_visible)(const clap_plugin_t *plugin,
                                       clap_id              display_id,
                                       bool                 visible);

   // draws one frame into frame->surface, as frame->api says: for pixels/1 into
   // frame->surface.pixels->data. only called while visible, and only for an api which
   // renders on request. true presents it, false keeps the previous frame
   // [main-thread]
   bool(CLAP_ABI *display_render)(const clap_plugin_t                  *plugin,
                                  clap_id                               display_id,
                                  const clap_embedded_hardware_frame_t *frame);

   // the host takes a claimed display back, e.g. when its panel goes, on suspend or
   // before destroy; free what display_open allocated
   // [main-thread]
   void(CLAP_ABI *display_close)(const clap_plugin_t *plugin, clap_id display_id);

   // the device goes to sleep or wakes; see "lifecycle" at the top
   // [main-thread]
   void(CLAP_ABI *suspend)(const clap_plugin_t *plugin);
   void(CLAP_ABI *resume)(const clap_plugin_t *plugin);
} clap_plugin_embedded_hardware_t;

#ifdef __cplusplus
}
#endif
