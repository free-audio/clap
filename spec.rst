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
  - No C++ exceptions
  - No macro obfuscation
  - No object file to compile in the SDK, CLAP is an interface only.
  - Simple resource management

- Designed to work on any operating system and processor architecture
- Be event oriented
- Be extensible
- Be easy to bridge
- Be dynamic
- Full MIDI access
- Use flexible designs, give the option:

  - Today most plugin and host manufacturer have their own ideas,
    and supports multiple formats. CLAP should not force one into
    a particular idea/design. For example, an host which do not
    want to send MIDI data to the plugin, should be able to translate
    MIDI CCs into parameter automation. On the other hand, an host
    which wants to give full MIDI access to the plugins, should be
    able to as well.
  - 32 bits or 64 bits audio? 32 bits is mandatory and 64 bits is
    an option.

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

A CLAP interface must not send exception. It is supposed to work width
C programs.

Multi-Threading
---------------

Every function should have a specifier which tells you if it is thread safe,
from which thread it can be used and if it must be lock-free and wait-free.

The design is pretty simple. Every thing which is closely related to the
audio processing, has to be realtime; **so it can't block or wait!**

Functions marked with **[audio-thread]**, can only be called from the audio
processing context and can not lock mutexes, or use any non deterministic
synchronization method. It is implicit that calling a **[thread-safe]**
might lead to wait on synchronization and it is forbidden.

Then the other functions, shall be marked as **[thread-safe]**, which indicates
that it can be called from multiple threads concurrently.

**[thread-safe,lock-wait-free]** means that the function can be called from
anywhere and must not lock or wait. It is suitable to use from an audio thread.

**[main-thread]** is the thread from which the user interacts with the GUI,
and most of the application happens. You can use synchronization there.

Naming conventions
------------------

Extensions name as ``struct clap_plugin_...`` are meant to be provided by the
plugin, while extensions name as ``struct clap_host_...`` are meant to be
provided by the host.

Plugins location
----------------

Common
~~~~~~

- Directories should be scanned recursively.

Installation Paths
~~~~~~~~~~~~~~~~~~

+----------+----------------------------+
| OS       | path                       |
+==========+============================+
| Linux    | ``/usr/lib/clap``          |
+          +----------------------------+
|          | ``~/.local/lib/clap``      |
+----------+----------------------------+
| Windows  | ``C:\Program Files\clap\`` |
+----------+----------------------------+
| OSX      | ``TBD``                    |
+----------+----------------------------+

Instantiate a plugin
--------------------

Plugin instantiation can be done in a few steps:

- load the dynamic library with ``dlopen()`` or symilar functions
- find the symbol ``clap_plugin_factory``
- call ``factory->init(plugin_path);``
- use the factory to:

  - get the number of plugins available ``factory->get_plugin_count();``
  - create plugins by index to enumerate the collection ``factory->create_plugin_by_index(host, index);``
  - create plugins by identifier to create a specific one ``factory->create_plugin_by_id(host, id);``

Release a plugin
~~~~~~~~~~~~~~~~

To release a plugin, call ``plugin->destroy(plugin);``.

Plugin description
~~~~~~~~~~~~~~~~~~

``struct clap_plugin`` contains an interger ``clap_version`` which
indicates which version of the clap interface has been used to build the plugin, and
a few methods. The attribute ``clap_version`` must be initialized by the plugin with
``CLAP_PLUGIN_VERSION``.

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


If the extension is not supported, the plugin must return ``NULL``.

Extensions are interface, and **there is no need for the caller to free the pointer**.

By convention, extensions should provide a define for the extension name.

.. code:: c

  // defined in <clap/ext/ports.h>
  # define CLAP_EXT_AUDIO_PORTS "clap/audio-ports"

Audio ports configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

A plugin may have multiple audio ports.

An audio port has:
 - a direction (input or output)
 - a channel count: the number of pin in the bus
 - a channel mapping (eg: stereo left then right)
 - a name

There can be only one main input and one main output.

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

``activate()`` **must not** be called if the plugin is already activated.

The plugin activation could be nothing, or could be a task which takes time,
like allocating and initializing buffers.
So the host **must not** activate plugins in the audio processing thread.

To deactivate the plugin, just call ``plugin->deactivate(plugin)``. Like
``activate()``, ``deactivate()`` should not be called from the audio processing
thread as it may take time.

``deactivate()`` **must not** be called if the plugin is not activated.

The host **must** de-activate the plugin before destroying it.

Once activated the plugin can't change:
 - its parameter list
 - its latency
 - its audio ports list

Before activating the plugin the host should cache:
 - the parameter list
 - the latency
 - the audio ports list

Once the plugin is activated, when the plugin changes a parameter from its GUI.
It should inform the host by using ``clap_host_params`` extension and the host
will update the audio processor with a ``CLAP_EVENT_PARAM_SET`` event. At the
same time, when a parameter changes from the DAW, it will inform the plugin GUI
by using ``clap_plugin_params->set_param_value(...)`` and the processor using
``CLAP_EVENT_PARAM_SET`` event.

Processing
----------

The processing is done in one call: ``plugin->process(plugin, process);``.
The data structure process regroup everything needed by the plugin:

- number of frames
- transport info
- audio buffers
- events stream

Once the processing is finished, the methods returns a process status
which can be:

+---------------------------+-------------------------------------------------------------+
| Status                    | Meaning                                                     |
+===========================+=============================================================+
| ``CLAP_PROCESS_ERROR``    | An error happened, and the output should be discarded       |
+---------------------------+-------------------------------------------------------------+
| ``CLAP_PROCESS_CONTINUE`` | Succeed, the plugins wants to process the next block        |
+---------------------------+-------------------------------------------------------------+
| ``CLAP_PROCESS_SLEEP``    | Succeed, I'm quiet and can go to sleep                      |
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
- The plugin must not modify the events input events.
- The host output event stream must make a copy of the plugin events.

Parameters Events
`````````````````

Parameters can be automated by the host using ``CLAP_EVENT_PARAM_SET``.
The ramp only applies to float parameters, and applies until an other
``CLAP_EVENT_PARAM_SET`` event is received.

Parameters
----------

The host can get the plugin's parameters tree by using the params extension:

- ``params->count(plugin);`` to know the number of parameters
- ``params->get_param_info(plugin, param_index, &param_info);`` to get the parameter
  value and description

.. code:: c

  #include <clap/ext/params.h>

  const struct clap_plugin_params *params = plugin->extension(plugin, CLAP_EXT_PARAMS);
  if (!params)
    return; // no params extensions
  uint32_t count = params->count(plugin);
  for (uint32_t i = 0; i < count; ++i) {
    struct clap_param_info param_info;
    if (!params->get_param_info(plugin, i, &param_info))
      continue;
    // ...
  }

See `clap/ext/params.h`_.

Types
~~~~~

There are a few parameter types:

+--------+-----------------+------------------------------------------------------+
| type   | value attribute | description                                          |
+========+=================+======================================================+
| bool   | ``value.b``     | a boolean value, can be true or false                |
+--------+-----------------+------------------------------------------------------+
| double | ``value.d``     | a double value                                       |
+--------+-----------------+------------------------------------------------------+
| int    | ``value.i``     | an integer value                                     |
+--------+-----------------+------------------------------------------------------+
| enum   | ``value.i``     | an enumeration, it uses integer values, and the host |
|        |                 | should rely on ``display_text`` to show its value.   |
+--------+-----------------+------------------------------------------------------+

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
| Generic    | `clap/ext/gui.h`_         | ``CLAP_EMBED``       | For the host   |
+------------+---------------------------+----------------------+----------------+
| Windows    | `clap/ext/gui-win32.h`_   | ``CLAP_EMBED_WIN32`` | For the plugin |
+------------+---------------------------+----------------------+----------------+
| X11        | `clap/ext/gui-x11.h`_     | ``CLAP_EMBED_X11``   | For the plugin |
+------------+---------------------------+----------------------+----------------+
| Cocoa      | `clap/ext/gui-cocoa.h`_   | ``CLAP_EMBED_COCOA`` | For the plugin |
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

Resizing the window
```````````````````

.. code:: c

  #include <clap/clap.h>
  #include <clap/ext/embed.h>

  // plugin code
  struct clap_host_gui *host_gui = host->get_extension(plugin, CLAP_EXT_GUI);
  if (host_gui && host_gui->resize(host, width, height)) {
    // resize succeed
  }

Save and restore plugin's state
-------------------------------

Saving the plugin's state is done by using the state extension:

.. code:: c

  #include <clap/clap.h>
  #include <clap/ext/state.h>

  struct clap_ostream *stream = ...;

  struct clap_plugin_state *state = plugin->get_extension(plugin, CLAP_EXT_STATE);
  if (state && state->save(plugin, stream)) {
    // save succeed
  } else {
    // save failed
  }

Restoring the plugin's state is done by:

.. code:: c

  state->restore(plugin, stream);

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

clap/ext/params.h
-----------------

.. include:: include/clap/ext/params.h
   :code: c

clap/ext/gui.h
--------------

.. include:: include/clap/ext/gui.h
   :code: c

clap/ext/gui-win32.h
--------------------

.. include:: include/clap/ext/gui-win32.h
   :code: c

clap/ext/gui-x11.h
------------------

.. include:: include/clap/ext/gui-x11.h
   :code: c

clap/ext/gui-cocoa.h
--------------------

.. include:: include/clap/ext/gui-cocoa.h
   :code: c


clap/ext/draft/key-name.h
-------------------------

.. include:: include/clap/ext/draft/key-name.h
   :code: c
