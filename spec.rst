==========================
CLAP (CLever Audio Plugin)
==========================

--------------------------
A free audio plugin format
--------------------------

.. contents::

Goals
=====

- Make a free digital instrument and effect plugin interface
- Be easy to understand and implement
- Don't require alien technology, or masoshist design

  - Based on C, not Objective-C or C++
  - No dependancy on external libraries
  - No serialization format
  - No C++ multiple inheritence
  - No macro obfuscation
  - No object file to compile in the SDK, CLAP is an interface only.
  - Simple resource management mechanism

- Designed to work on any operating system and processor architecture
- Be event oriented
- Be extensible
- Be easy to bridge
- More and more advanced modular systems are showing up, things should be dynamics

  - dynamic ports
  - dynamic parameters

- Full MIDI access

Specification
=============

How to read the specification
-----------------------------

The specification should be read along the reference headers.
https://free-audio.github.io/clap/ gives a convinient splet
view for that.

Encoding
--------

All the strings exchanged through the CLAP interface must be encoded
in valid UTF-8.

C++ exceptions
--------------

A CLAP interface must not send exception.

Plugins location
----------------

Common
~~~~~~

- Directories should be scanned recursively.

Linux
~~~~~

- Plugins distributed with packages should be installed to: ``/usr/lib/clap/`` or ``/usr/local/lib/clap/``
- Plugins installed in the user's home should be installed to: ``${HOME}/.clap/``

Windows
~~~~~~~

- Plugins should be installed to: ``C:\Program Files\clap\``

Mac
~~~

TBD

Instantiate a plugin
--------------------

Plugin instantiation can be done in a few steps:

- load the dynamic library with ``dlopen`` or symilar functions
- find the symbol ``clap_plugin_factory``
- use the factory to:

  - get the number of plugins available ``factory->get_plugin_count(...);``
  - create plugins by index to enumerate the collection ``factory->create_plugin_by_index(...);``
  - create plugins by identifier to create a specific one ``factory->create_plugin_by_id(...);``

Release a plugin
~~~~~~~~~~~~~~~~

To release a plugin, call ``plugin->destroy(plugin);``.

Plugin description
~~~~~~~~~~~~~~~~~~

``struct clap_plugin`` contains an interger ``clap_version`` which
indicates which version of the clap interface has been used to build the plugin, and
a few methods. The attribute ``clap_version`` must be initialized by the plugin with
``CLAP_PLUGIN_VERSION``.

Then to get the plugin's name, you have to use
``plugin->get_attribute(plugin, CLAP_ATTR_NAME, ...);``.

See ``#include <clap/clap.h>`` for more information.

Extension system
~~~~~~~~~~~~~~~~

To extend clap's functionnality, there is a pretty simple mechanism:

.. code:: c

  // query a plugin extension
  const void *plug_ext = plugin->extension(plug, "company/ext-name");
  if (plug_ext) // check if the interface was implemented
  {
    // cast to the concrete type
    const struct interface_type *plug_ext1 = (const struct interface_type *)plug_ext;

    // use the interface ...
    plug_ext1->my_fct(plug, ...);
  }

  // For the host, this is similar:
  const void *host_ext = host->extension(host, "company/ext-name");
  ...


If the extension is not supported, the plugin should return ``NULL``.

Extensions are interface, and **there is no need for the caller to free the pointer**.

By convention, extensions should provide a define for the extension name.

.. code:: c

  // defined in <clap/ext/ports.h>
  # define CLAP_EXT_AUDIO_PORTS "clap/audio-ports"

Audio ports configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

A plugin may have multiple audio ports, and multiple configurations
(mono, stereo, quad, surround, ...). Yet we want to keep things simple,
flexible and dynamic.

An audio port has:
 - a direction (input or output)
 - a channel count: the number of pin in the bus
 - a channel mapping (eg: stereo left then right)
 - a role (input or output, sidechain input, audio rate modulation signal)
 - a name
 - is repeatable: can be used as a template to create multiple instance of the
   same port and so connect multiple signals to it. For example you have an
   analyzer and you want a repeatable input port, so the user can connect
   an arbitrary number of signals.
 
Standard channel mappings
`````````````````````````

+----------+-----+---------------------+
| type     | pin | description         |
+==========+=====+=====================+
| mono     | 0   | mono                |
+----------+-----+---------------------+
| stereo   | 0   | left                |
|          +-----+---------------------+
|          | 1   | right               |
+----------+-----+---------------------+
| surround | 0   | front left          |
|          +-----+---------------------+
|          | 1   | front right         |
|          +-----+---------------------+
|          | 2   | center              |
|          +-----+---------------------+
|          | 3   | low frequency       |
|          +-----+---------------------+
|          | 4   | surround left       |
|          +-----+---------------------+
|          | 5   | surround right      |
|          +-----+---------------------+
|          | 6   | surround back left  |
|          +-----+---------------------+
|          | 7   | surround back right |
+----------+-----+---------------------+

Activation
----------

Before doing any processing, the plugin must be activated by calling
``bool succeed = plugin->activate(plugin);``.

If ``succeed == true`` then the activation succeed. If the activation failed,
then the plugin is unusable.

The host must not call ``activate()`` if the plugin is already activated.
Yet the plugin should handle correctly double calls to ``activate()``.

The plugin activation could be nothing, or could be a task which takes time,
like connecting a remote server or device.
So the host should not activate plugins in the audio processing thread.

To deactivate the plugin, just call ``plugin->deactivate(plugin)``. Like
``activate()``, ``deactivate()`` should not be called from the audio processing
thread as it may take time.

``deactivate()`` must not be called if the plugin is not activated. Yet the
plugin should handle this mis-usage.

The host must de-activate the plugin before destroying it. Again, if
deactivate was not called before destroy(), the plugin should handle it
gracefully.

Processing
----------

The processing is done in one call: ``plugin->process(plugin, process);``.
The data structure process regroup everything needed by the plugin:

- number of frames
- events (in)
- some time info

Once the processing is finished, the methods returns a process status
which can be:


+---------------------------+-------------------------------------------------------------+
| Status                    | Meaning                                                     |
+===========================+=============================================================+
| ``CLAP_PROCESS_ERROR``    | An error happened, and the output should be discarded       |
+---------------------------+-------------------------------------------------------------+
| ``CLAP_PROCESS_CONTINUE`` | Succeed, the plugins wants to process the next block        |
+---------------------------+-------------------------------------------------------------+
| ``CLAP_PROCESS_SLEEP``    | Succeed, every voices terminated, wake me up on a new event |
+---------------------------+-------------------------------------------------------------+

If ``process()`` returns ``CLAP_PROCESS_SLEEP`` and some parameters were ramping
(see ``CLAP_EVENT_PARAM_RAMP`` event), then the host must send a ``CLAP_EVENT_PARAM_SET``
or  ``CLAP_EVENT_PARAM_RAMP`` for those parameters at the next call to process.

Audio buffers
~~~~~~~~~~~~~

- The audio buffers are allocated by the host.
- In-place processing is not supported by default, yet the host can use it
  if the plugin has the attribute ``CLAP_ATTR_SUPPORTS_IN_PLACE_PROCESSING``.
- See `Standard channel mappings`_.

Events
~~~~~~

- Event's time is relative to the first sample of the processing block.
- The plugin must not modify the events.

Notes
`````

A note is identified by a key. A key correspond to the keys of a midi keyboard (128 keys).
If the plugin supports tuning then it should use the ``event->note.pitch`` as
the note frequency.

The note A4 correspond to the key 57 and the frequency 440Hz.
The note A3 correspond to the key 45 and the frequency 220Hz.

If the plugin supports tuning, then the host could play the note A4 by sending
a NOTE_ON event with key = 0 and pitch = 440. Then to stop the the host can
send a NOTE_OFF event with the same key, so 0 in our case or it can send
a NOTE_ON event on the same key (0), which would terminate the note on the key
0 and start a new note on the key 0 with the given pitch.

Here is a scenario where the plugin does not support tuning:

- NOTE_ON, key = 60, pitch = 42; starts the note C4, with the pitch 493.88Hz
- NOTE_OFF, key = 0, pitch = 493.88; ignored because no note has been started on key 0
- NOTE_ON, key = 60, pitch = 54; retrigers the note C4, with the pitch 493.88Hz
- NOTE_OFF, key = 60, pitch = 62; stops the note C4

Here is a scenario where the plugin does support tuning:

- NOTE_ON, key = 60, pitch = 42; starts a note, with the pitch 42Hz
- NOTE_OFF, key = 0, pitch = 493.88; ignored because no note has been started on key 0
- NOTE_ON, key = 60, pitch = 54; stops the note with the pitch 42Hz and starts
  a note with a pitch of 54Hz
- NOTE_OFF, key = 60, pitch = 62; stops the note with a pitch of 54Hz

The plugin is encouraged to use an array with 128 pointers to voice, so
it can quickly figure which voice is playing the given key.

Parameters
``````````

Parameters can be automated by the host using ``CLAP_EVENT_PARAM_SET`` or
``CLAP_EVENT_PARAM_RAMP``. Or they can be automated at audio rate by using
an audio buffer.

When using ``CLAP_EVENT_PARAM_RAMP``, the parameter is set to ``ev->param.value``
and has to be incremented by ``event->param.increment`` for each samples, except
for the time of the event, and until an event ``CLAP_EVENT_PARAM_SET`` or
``CLAP_EVENT_PARAM_RAMP`` occur for this parameter.

Parameters
----------

The host can get the plugin's parameters tree by using the params extension:

- ``params->count(plugin);`` to know the number of parameters
- ``params->get_param(plugin, param_index, &param);`` to get the parameter
  value and description

.. code:: c

  #include <clap/ext/params.h>

  struct clap_plugin_params *params = plugin->extension(plugin, CLAP_EXT_PARAMS);
  if (!params)
    return; // no params extensions
  uint32_t count = ports->count(plugin);
  for (uint32_t i = 0; i < count; ++i) {
    struct clap_param param;
    if (!ports->get_param(plugin, i, &param))
      continue;
    // ...
  }

See `clap/ext/params.h`_.
 
Types
~~~~~

There are a few parameter types:

+-------+-----------------+------------------------------------------------------+
| type  | value attribute | description                                          |
+=======+=================+======================================================+
| group | none            | not a value, but the only parameter which can have   |
|       |                 | child. It should be used to organize parameters in   |
|       |                 | the host GUI.                                        |
+-------+-----------------+------------------------------------------------------+
| bool  | ``value.b``     | a boolean value, can be true or false                |
+-------+-----------------+------------------------------------------------------+
| float | ``value.f``     | a float value                                        |
+-------+-----------------+------------------------------------------------------+
| int   | ``value.i``     | an integer value                                     |
+-------+-----------------+------------------------------------------------------+
| enum  | ``value.i``     | an enumeration, it uses integer values, and the host |
|       |                 | should rely on ``display_text`` to show its value.   |
+-------+-----------------+------------------------------------------------------+

Scales
~~~~~~

The plugin can inform the host, which scale to use for the parameter's UI
(knob, slider, ...). ``clap_param->scale`` can be set to ``CLAP_PARAM_LINEAR``
or ``CLAP_PARAM_LOG``. A logarithmic scale is convinient for a frequency
parameter.

Automation
~~~~~~~~~~

When a parameter is modified by the GUI, the plugin should send a
``CLAP_EVENT_PARAM_SET`` event must be sent to the host, using
``host->events(host, plugin, events);`` so the host can record the automation.

When a parameter is modified by an other parameter (this is discouraged),
for example imagine you have a parameter modulating "absolutely" an other
one through an XY mapping.
The host should record the modulation source but not the modulation target.
To do that the plugin uses ``clap_event_param->is_recordable``.

Graphical User Interface
------------------------

Showing the GUI
~~~~~~~~~~~~~~~

To show the plugin's GUI, you need to use the gui extension: ``gui->open(plugin)``.
If the plugin succeed to show the GUI, it returns ``true``, ``false``
otherwise.

.. code:: c

  #include <clap/ext/gui.h>

  struct clap_plugin_gui *gui = plugin->extension(plugin, CLAP_EXT_GUI);
  if (gui)
    gui->open(plugin);

See `clap/ext/gui.h`_.

Sending events to the host
~~~~~~~~~~~~~~~~~~~~~~~~~~

The plugin can notify the host of parameter changes by sending events to:
``host->events(host, plugin, events);``.

Events sent to the host should be stamped:

.. code:: c

  struct clap_event ev;
  // ...
  ev.steady_time = *host->steady_time;
  host->events(host, plugin, &ev);

Hiding the GUI
~~~~~~~~~~~~~~

The plugin should hide the GUI after a call to ``gui->close(plugin)``.
If the plugin window has been closed by the user, then the plugin should
send an event ``CLAP_EVENT_GUI_CLOSED`` to the host.

.. code:: c

  struct clap_event ev;
  ev.next = NULL;
  ev.type = CLAP_EVENT_GUI_CLOSED;
  ev.sample_offset = *host->steady_time;
  host->events(host, plugin, &ev);

Embedding
~~~~~~~~~

Some host are designed to embed plugin's window.
As embedding is not a requirement and is OS specific, it is then offered as
an extension.

+------------+---------------------------+----------------------+----------------+
| GUI        | header                    | extension            | comment        |
+============+===========================+======================+================+
| Generic    | `clap/ext/embed.h`_       | ``CLAP_EMBED``       | For the host   |
+------------+---------------------------+----------------------+----------------+
| Windows    | `clap/ext/embed-win32.h`_ | ``CLAP_EMBED_WIN32`` | For the plugin |
+------------+---------------------------+----------------------+----------------+
| X11        | `clap/ext/embed-x11.h`_   | ``CLAP_EMBED_X11``   | For the plugin |
+------------+---------------------------+----------------------+----------------+
| Cocoa      | `clap/ext/embed-cocoa.h`_ | ``CLAP_EMBED_COCOA`` | For the plugin |
+------------+---------------------------+----------------------+----------------+

Example on Windows
``````````````````

.. code:: c

  #include <clap/clap.h>
  #include <clap/ext/gui.h>
  #include <clap/ext/embed-win32.h>

  struct clap_plugin_embed_win32 *embed = plugin->get_extension(plugin, CLAP_EMBED_WIN32);
  if (embed)
    embed->embed(plugin, window);

  struct clap_plugin_gui *gui = plugin->get_extension(plugin, CLAP_EXT_GUI);
  if (gui)
    gui->open(plugin);

Resizing the window
```````````````````

.. code:: c

  #include <clap/clap.h>
  #include <clap/ext/embed.h>

  // plugin code
  struct clap_host_embed *embed = host->get_extension(plugin, CLAP_EMBED);
  if (embed && embed->resize(host, width, height)) {
    // resize succeed
  }


Presets
-------

List plugin's presets
~~~~~~~~~~~~~~~~~~~~~

The host can browse the plugin's presets by using the preset extension:

.. code:: c

  #include <clap/clap.h>
  #include <clap/ext/presets.h>

  struct clap_plugin_presets *presets = plugin->get_extension(plugin, CLAP_PRESETS);
  struct clap_preset_iterator *iter = NULL;
  struct clap_preset;

  if (presets && presets->iter_begin(plugin, &iter)) {
    do {
      if (!presets->iter_get(plugin, iter, &preset))
        break;

      // XXX: do your work with preset;
    } while (presets->iter_next(plugin, iter));

    // release the iterator
    presets->iter_destroy(plugin, iter);
  }

See `clap/ext/presets.h`_.

Load a preset
~~~~~~~~~~~~~

To load a preset, the host have to send an event ``CLAP_EVENT_PRESET_SET`` to
the plugin.

When a preset is loaded from the plugin's GUI, the plugin must send a
``CLAP_EVENT_PRESET_SET`` to the host.

Save and restore plugin's state
-------------------------------

Saving the plugin's state is done by using the state extension:

.. code:: c

  #include <clap/clap.h>
  #include <clap/ext/state.h>

  void   *buffer = NULL;
  size_t  size   = 0;

  struct clap_plugin_state *state = plugin->get_extension(plugin, CLAP_EXT_STATE);
  if (state && state->save(plugin, &buffer, &size)) {
    // save succeed
  } else {
    // save failed
  }

Restoring the plugin's state is done by:

.. code:: c

  state->restore(plugin, buffer, size);

The state of the plugin should be independent of the machine: you can save a
plugin state on a little endian machine and send it through the network to a
big endian machine, it should load again successfully.

See `clap/ext/state.h`_.

Note: if the plugin do not have the state extension, the host can save the
plugin's parameters and restore them.

References
==========

clap/clap.h
-----------

.. include:: include/clap/clap.h
   :code: c

clap/ext/state.h
----------------

.. include:: include/clap/ext/state.h
   :code: c

clap/ext/audio-ports.h
----------------------

.. include:: include/clap/ext/audio-ports.h
   :code: c

clap/ext/event-ports.h
----------------------

.. include:: include/clap/ext/event-ports.h
   :code: c

clap/ext/params.h
-----------------

.. include:: include/clap/ext/params.h
   :code: c

clap/ext/gui.h
--------------

.. include:: include/clap/ext/gui.h
   :code: c

clap/ext/embed.h
----------------

.. include:: include/clap/ext/embed.h
   :code: c

clap/ext/embed-win32.h
----------------------

.. include:: include/clap/ext/embed-win32.h
   :code: c

clap/ext/embed-x11.h
---------------------

.. include:: include/clap/ext/embed-x11.h
   :code: c

clap/ext/embed-cocoa.h
----------------------

.. include:: include/clap/ext/embed-cocoa.h
   :code: c

clap/ext/presets.h
------------------

.. include:: include/clap/ext/presets.h
   :code: c
