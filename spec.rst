==========================
CLAP (CLever Audio Plugin)
==========================

--------------------------
A free audio plugin format
--------------------------

.. contents::

Goals
=====

- Make a free digital instrument and effect plugin format
- Be easy to understand and implement
- Bring new features missed in VST 2.4
- Designed to work on any operating system and processor architecture
- Be event oriented
- Be extensible
- Be easy to bridge
- Support dynamic configuration: let a modular plugin dynamically
  add new parameters, new outputs/inputs, etc...

Design choice
-------------

- Use the C language for the interface.
- The host interface must be thread-safe.
- The plugin interface is not thread-safe.

Specification
=============

How to read the specification
-----------------------------

The specification should be read along the reference headers.
https://free-audio.github.io/clap/ gives a convinient view for that.

Encoding
--------

All the strings exchanged through the CLAP interface must be encoded in UTF-8
and must be valid.

Plugins location
----------------

Common
~~~~~~

- Directories should be scanned recursively.

Linux
~~~~~

- Plugins distributed with packages should be installed to: ``/usr/lib/clap/``
- Plugins installed in the user's home should be installed to: ``${HOME}/.clap/``

Windows
~~~~~~~

TBD

Mac
~~~

TBD

Multi-architecture conventions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let's say that we have a plugin called ``DigitalDragon``. If we distribute
it for multiple architecture, then the host should be able to identify which
version is suited for the current architecture by reading its name.

For example:

+-------------+---------------------------+
| Archtecture | Filename                  |
+=============+===========================+
| x86         | DigitalDragon.x86.so      |
+-------------+---------------------------+
| x86_64      | DigitalDragon.x86_64.so   |
+-------------+---------------------------+
| alpha       | DigitalDragon.alpha.so    |
+-------------+---------------------------+
| arm         | DigitalDragon.arm.so      |
+-------------+---------------------------+
| sparc       | DigitalDragon.sparc.so    |
+-------------+---------------------------+
| hppa        | DigitalDragon.hppa.so     |
+-------------+---------------------------+
| ppc         | DigitalDragon.ppc.so      |
+-------------+---------------------------+
| ppc64       | DigitalDragon.ppc64.so    |
+-------------+---------------------------+

If the name does not contain an indicator, then the plugin should be
built for the native/current architecture.

Instantiate a plugin
--------------------

Plugin instantiation can be done in a few steps:

- load the plugin library with ``dlopen`` or symilar functions
- find the symbol ``clap_create``
- instantiate the plugin by calling ``clap_create``

Precautions
~~~~~~~~~~~

- The function ``clap_create`` must be thread-safe.
- It must not throw exceptions.
- It can return ``NULL``.

Release a plugin
~~~~~~~~~~~~~~~~

To release a plugin, call ``plugin->destroy(plugin);``.
It is not required to deactivate the plugin prior to destroy it.

Plugins collection
~~~~~~~~~~~~~~~~~~

A single shared library can contains multiple clap plugins.
To list them, you have to call ``clap_create`` with an index of 0.
``clap_create`` will store the number of plugins in the collection
into the parameter ``*plugins_count``. After that you can create any
of them by using an ``index`` between ``0`` and ``*plugins_count``.

``clap_create`` returns ``NULL`` if the plugin creation failed or if
``plugin_index >= plugin_count``.

Sample plugin loader
````````````````````

See `examples/clap-info/clap-info.c`_

Plugin description
~~~~~~~~~~~~~~~~~~

``struct clap_plugin`` only contains a interger ``clap_version`` which
indicates which version of the clap interface has been used to build the plugin, and
a few methods. This attribute must be initialized by the plugin with
``CLAP_PLUGIN_VERSION``.

Then to get plugin's attribute, you have to use ``plugin->get_attribute(plugin, ...)``.

+--------------------------------+---------------------------------------------------------------+
| Attribute                      | Description                                                   |
+================================+===============================================================+
| CLAP_ATTR_ID                   | Unique identifier of the plugin. It should never change. It   |
|                                | should be the same on any plateform.                          |
+--------------------------------+---------------------------------------------------------------+
| CLAP_ATTR_NAME                 | The name of the product.                                      |
+--------------------------------+---------------------------------------------------------------+
| CLAP_ATTR_DESCRIPTION          | A brief description of the product.                           |
+--------------------------------+---------------------------------------------------------------+
| CLAP_ATTR_MANUFACTURER         | Which company made the plugin.                                |
+--------------------------------+---------------------------------------------------------------+
| CLAP_ATTR_VERSION              | A string describing the product version.                      |
+--------------------------------+---------------------------------------------------------------+
| CLAP_ATTR_URL                  | An URL to the product homepage.                               |
+--------------------------------+---------------------------------------------------------------+
| CLAP_ATTR_LICENSE              | The plugin license type, Custom, GPLv3, MIT, ...              |
+--------------------------------+---------------------------------------------------------------+
| CLAP_ATTR_SUPPORT              | A link to the support, it can be                              |
|                                | ``mailto:support@company.com`` or                             |
|                                | ``http://company.com/support``.                               |
+--------------------------------+---------------------------------------------------------------+
| CLAP_ATTR_CATEGORIES           | A string containing a list of categories, joined with ``;``.  |
|                                | For example: ``fm;analogue;delay``.                           |
+--------------------------------+---------------------------------------------------------------+
| CLAP_ATTR_TYPE                 | Bitfield describing what the plugin does. See                 |
|                                | ``enum clap_plugin_type``.                                    |
+--------------------------------+---------------------------------------------------------------+
| CLAP_ATTR_CHUNK_SIZE           | The process buffer must have a number of sample multiple of   |
|                                | ``chunk_size``.                                               |
+--------------------------------+---------------------------------------------------------------+
| CLAP_ATTR_LATENCY              | The latency introduced by the plugin.                         |
+--------------------------------+---------------------------------------------------------------+
| CLAP_ATTR_SUPPORTS_TUNING      | ``1`` if the plugin supports tuning.                          |
+--------------------------------+---------------------------------------------------------------+
| CLAP_ATTR_IS_REMOTE_PROCESSING | ``1`` if the plugin is doing remote processing. This can help |
|                                | the DAW's task scheduling.                                    |
+--------------------------------+---------------------------------------------------------------+
| ..._IN_PLACE_PROCESSING        | ``1`` if the plugin supports in place processing.             |
+--------------------------------+---------------------------------------------------------------+

Extension system
~~~~~~~~~~~~~~~~

To extend clap's functionnality, there is a pretty simple mechanism:

.. code:: c

  void *plug_ext = plugin->extension(plug, "company/ext-name");
  void *host_ext = host->extension(host, "company/ext-name");

If the extension is not supported, the plugin should return ``NULL``.

Audio ports configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

A plugin may have multiple audio ports, and so multiple audio ports
layout or configurations.

An audio port has a type: mono, stereo, surround and a role: main
input/output or sidechain. We might add a feedback role in the futur
if there is a need for it. Also, an instrument/effect can load and host
clap effects for its feedback loops.

Pin layout
``````````

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

Configurations
``````````````

After the call to ``clap_create()`` the new plugin uses the default ports
configuration: 1 stereo input and 1 stereo output. So if you're fine with
it, there is nothing more to do.

If a plugins wants to offer more ports configuration to the host/user, it
has to use ports extension. See `clap/ext/ports.h`_.

The host can select a ports configuration only if the plugin is in
the deactivated state.

Note that if the plugin does not support the default configuration
which is stereo input and stereo output, then it must return false
during the plugin activation (``plugin->activate(plugin)``).

Here is a configuration for a stereo sidechain compressor:

+--------+----------+------------+---------------------+-----------------+
| in/out | type     | role       | buffer              | desc            |
+========+==========+============+=====================+=================+
| input  | stereo   | inout      | process->inputs[0]  | left input      |
|        |          |            +---------------------+-----------------+
|        |          |            | process->inputs[1]  | right input     |
+--------+----------+------------+---------------------+-----------------+
| input  | stereo   | sidechain  | process->inputs[2]  | left sidechain  |
|        |          |            +---------------------+-----------------+
|        |          |            | process->inputs[3]  | right sidechain |
+--------+----------+------------+---------------------+-----------------+
| output | stereo   | inout      | process->outputs[0] | left input      |
|        |          |            +---------------------+-----------------+
|        |          |            | process->outputs[1] | right input     |
+--------+----------+------------+---------------------+-----------------+

Getting the ports configurations
````````````````````````````````

.. code:: c

  #include <clap/ext/ports.h>

  struct clap_plugin_ports *ports = plugin->extension(plugin, CLAP_EXT_PORTS);
  if (!ports)
    return; // no ports extension
  uint32_t count = ports->get_configs_count(plugin);
  for (uint32_t i = 0; i < count; ++i) {
    struct clap_ports_config config;
    if (!ports->get_config(plugin, i, &config))
      continue;
    // ...
  }

It is possible to discover a plugin's port configurations by calling
``ports->get_configs_count(plugin);``. It returns the number of
configurations. Then for each configuration you have to call
``ports->get_config(plugin, config_index, &config);`` which will
tell you the number of input and output ports. Then to get the port details,
you have to call
``ports->get_info(plugin, config_index, port_index, &port);``.

Selecting a configuration
`````````````````````````

Selecting an audio configuration has to be done when the plugin is deactivated.
It is done by calling ``plugin->set_port_config(plugin, config_index)``.
If the call returns false, then the plugin is in failed state.

Repeatable channels
```````````````````

Repeatable channels are a special case. A channel can be identified as
repeatable if ``channel->is_repeatable == true``.

A useful case is for an analyzer. Imagine a spectroscope, to which you want to
plug any number of inputs. Each of those inputs can be named and displayed in
the spectrograph, so it is a convinient way analyze many tracks in the same
spectroscope.

For the special case of repeatable side chain input, the host
has to tell the plugin how many times the port should be repeated.
To do that it has to call ``plugin->set_repeat(plugin, port_index, count)``.
If it returns ``false`` then the plugin is in the same state as before
the call.

Only inputs can be repeatable.

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

Also ``deactivate()`` should not be called if the plugin is not activated.
Yet the plugin should handle a call to ``deactivate()`` even if it is
not activated.

It is recommended (but not mandatory) to de-activate the plugin before
destroying it.

Processing
----------

The processing is done in one call: ``plugin->process(plugin, process);``.
The data structure process regroup everything needed by the plugin:

- audio buffers (in, out)
- events (in)
- some time info

Once the processing is finished, the methods returns a process status
which can be:


+---------------------------+-------------------------------------------------------------+
| Status                    | Meaning                                                     |
+===========================+=============================================================+
| ``CLAP_PROCESS_ERROR``    | An error happened, and the buffers should be discarded      |
+---------------------------+-------------------------------------------------------------+
| ``CLAP_PROCESS_CONTINUE`` | Succeed, the plugins wants to process the next block        |
+---------------------------+-------------------------------------------------------------+
| ``CLAP_PROCESS_STOP``     | Succeed, every voices terminated, wake me up on a new event |
+---------------------------+-------------------------------------------------------------+

If ``process()`` returns ``CLAP_PROCESS_STOP`` and some parameters were ramping
(see ``CLAP_EVENT_PARAM_RAMP`` event), then the host must send a ``CLAP_EVENT_PARAM_SET``
or  ``CLAP_EVENT_PARAM_RAMP`` for those parameters at the next call to process.

Audio buffers
~~~~~~~~~~~~~

- The audio buffers are allocated by the host. They must be aligned by the
  maximum requirement of the vector instructions currently available.
- In-place processing is not supported by default, yet the host can use it
  if the plugin has the attribute ``CLAP_ATTR_SUPPORTS_IN_PLACE_PROCESSING``.
- The number of samples must be a multiple of the plugin chunk_size.
- See `Pin layout`_.
- See `Plugin description`_

Events
~~~~~~

- Event's time must be within the process duration:
  ``[process->steady_time .. process->steady_time + process->nb_sambles]``.
- The plugin must not modify the events.

Notes
`````

A note is identified by a key. A key correspond to the keys of a midi keyboard (128 keys).
If the plugin supports tunning then it should use the ``event->note.pitch`` as
the note frequency.

The note A4 correspond to the key 57 and the frequency 440Hz.
The note A3 correspond to the key 45 and the frequency 220Hz.

If the plugin supports tunning, then the host could play the note A4 by sending
a NOTE_ON event with key = 0 and pitch = 440. Then to stop the the host can
send a NOTE_OFF event with the same key, so 0 in our case or it can send
a NOTE_ON event on the same key (0), which would terminate the note on the key
0 and start a new note on the key 0 with the given pitch.

Here is a scenario where the plugin does not support tunning:

- NOTE_ON, key = 60, pitch = 42; starts the note C4, with the pitch 493.88Hz
- NOTE_OFF, key = 0, pitch = 493.88; ignored because no note has been started on key 0
- NOTE_ON, key = 60, pitch = 54; retrigers the note C4, with the pitch 493.88Hz
- NOTE_OFF, key = 60, pitch = 62; stops the note C4

Here is a scenario where the plugin does support tunning:

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
``CLAP_EVENT_PARAM_RAMP``.

When using ``CLAP_EVENT_PARAM_RAMP``, the parameter is set to ``ev->param.value``
and has to be incremented by ``event->param.increment`` for each samples, except
for the sample at ``ev->steady_time``, until an event ``CLAP_EVENT_PARAM_SET`` or
``CLAP_EVENT_PARAM_RAMP`` occur for this parameter.

Parameters
----------

The host can get the plugin's parameters tree by using the params extension:

- ``params->count(plugin);`` to know the number of parameters
- ``params->get(plugin, param_index, &param);`` to get the parameter
  value and description

.. code:: c

  #include <clap/ext/params.h>

  struct clap_plugin_params *params = plugin->extension(plugin, CLAP_EXT_PARAMS);
  if (!params)
    return; // no params extensions
  uint32_t count = ports->count(plugin);
  for (uint32_t i = 0; i < count; ++i) {
    struct clap_param param;
    if (!ports->get(plugin, i, &param))
      continue;
    // ...
  }

See `clap/ext/params.h`_.
 
+------------------+----------------------------------------------------------+
| Attribute        | Description                                              |
+==================+==========================================================+
| ``type``         | The type of parameter. Must never change.                |
+------------------+----------------------------------------------------------+
| ``id``           | What identifies the parameter. Must never change.        |
|                  | This field must be saved along automation.               |
+------------------+----------------------------------------------------------+
| ``name``         | The name of the parameter. This can change.              |
|                  | Meant to be displayed.                                   |
+------------------+----------------------------------------------------------+
| ``desc``         | The description of the parameter. This can change.       |
|                  | Meant to be displayed.                                   |
+------------------+----------------------------------------------------------+
| ``is_per_note``  | ``true`` if the parameter can be automated per voice.    |
+------------------+----------------------------------------------------------+
| ``display_text`` | How the value should be displayed. Only used for enum    |
|                  | types.                                                   |
+------------------+----------------------------------------------------------+
| ``is_used``      | True if the parameter is used by the current patch.      |
+------------------+----------------------------------------------------------+
| ``is_periodic``  | Means that the parameter is periodic, so                 |
|                  | ``value = value % max``.                                 |
+------------------+----------------------------------------------------------+
| ``value``        | The current value of the parameter.                      |
+------------------+----------------------------------------------------------+
| ``min``          | The minimum value of the parameter.                      |
+------------------+----------------------------------------------------------+
| ``max``          | The maximum value of the parameter.                      |
+------------------+----------------------------------------------------------+
| ``scale``        | The scale to use when exposing the parameter to the user.|
+------------------+----------------------------------------------------------+

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
  ev.steady_time = host->steady_time(host);
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
  ev.sample_offset = host->steady_time(host);
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
| X11        | `clap/ext/embed-xlib.h`_  | ``CLAP_EMBED_XLIB``  | For the plugin |
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

Examples
========

examples/clap-info/clap-info.c
------------------------------

.. include:: tools/clap-info/clap-info.c
   :code: c

References
==========

clap/clap.h
-----------

.. include:: include/clap/clap.h
   :code: c

clap/ext/ports.h
----------------

.. include:: include/clap/ext/ports.h
   :code: c

clap/ext/gui.h
--------------

.. include:: include/clap/ext/gui.h
   :code: c

clap/ext/state.h
----------------

.. include:: include/clap/ext/state.h
   :code: c

clap/ext/params.h
-----------------

.. include:: include/clap/ext/params.h
   :code: c

clap/ext/presets.h
------------------

.. include:: include/clap/ext/presets.h
   :code: c

clap/ext/embed.h
----------------

.. include:: include/clap/ext/embed.h
   :code: c

clap/ext/embed-win32.h
----------------------

.. include:: include/clap/ext/embed-win32.h
   :code: c

clap/ext/embed-xlib.h
---------------------

.. include:: include/clap/ext/embed-xlib.h
   :code: c
