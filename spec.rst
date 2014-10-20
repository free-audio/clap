==========================
CLAP (CLever Audio Plugin)
==========================

--------------------------
A free audio plugin format
--------------------------

.. contents::

Goals
=====

- Make a free audio plugin format
- Be easy to understand and implement
- Bring new features missed in VST 2.4
- Replace old concepts by modern ones. For example the specification tries
  to not use MIDI as its internal event systems, which let us have
  micro-tones. Yet MIDI events are possible.
- Designed to work on any operating system
- Be event oriented
- Be extensible without breaking existing plugins
- Be easy to bridge

Later goals
-----------

- Provide a reference host
- Provide some reference plugins
- Provide a validation plugin, which should signal anything wrong the host does
- Provide a validation host, which should give hard time to the plugin and
  ensure that basic functionnality are working

Design choice
-------------

- The plugin and the host interface must be thread-safe.
- Avoid pointer exchange between the host and the plugin
  whenever it is possible. The chosen way is to pass a buffer
  as a parameter and let the host/plugin copy/read data from it.
  Rationale: as the host and the plugin can be multi-threaded,
  keeping pointers to the plugin or host internal memory can lead
  to race conditions. Also it can lead to ambiguities about who's
  responsible to free the memory. Also the host and the plugin
  may use custom allocator.
- Use the C language.
- Have support for dynamic configuration, to let a modular plugin
  add new parameters, new outputs/inputs, etc... dynamically.

Specification
=============

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

Plugin instantiating can be done in a few steps:

- load the plugin library
- find the symbol ``clap_create``
- instantiate the plugin by calling ``clap_create``

Precautions
~~~~~~~~~~~

- The function ``clap_create`` must be thread-safe.
- It must not throw exceptions.
- It can return ``NULL``.

Plugins collection
~~~~~~~~~~~~~~~~~~

A single dynamic library can contains multiple clap plugins.
To list them, you have to call ``clap_create`` with an index of 0 and increment
the index until you reach ``plugin_count``.
``clap_create`` returns ``NULL`` if the plugin creation failed or if
``plugin_index >= plugin_count``.

Sample plugin loader
````````````````````

See `samples/clap-info.c`_

Description
~~~~~~~~~~~

Both the plugin and host have a few attribute giving general plugin description.

+---------------------+---------------------------------------------------------------+
| Attribute           | Description                                                   |
+=====================+===============================================================+
| clap_version        | Described the plugin format version implemented. Should be    |
|                     | initialized with CLAP_PLUGIN_VERSION,                         |
|                     | or CLAP_VERSION_MAKE(1, 0, 0) if you want to only support     |
|                     | version 1.0.0                                                 |
+---------------------+---------------------------------------------------------------+
| id                  | Unique identifier of the plugin. It should never change. It   |
|                     | should be the same on 32bits or 64bits or whatever.           |
+---------------------+---------------------------------------------------------------+
| name                | The name of the product.                                      |
+---------------------+---------------------------------------------------------------+
| description         | A brief description of the product.                           |
+---------------------+---------------------------------------------------------------+
| manufacturer        | Which company made the plugin.                                |
+---------------------+---------------------------------------------------------------+
| version             | A string describing the product version.                      |
+---------------------+---------------------------------------------------------------+
| url                 | An URL to the product homepage.                               |
+---------------------+---------------------------------------------------------------+
| license             | The plugin license type, Custom, GPLv3, MIT, ...              |
+---------------------+---------------------------------------------------------------+
| support             | A link to the support, it can be                              |
|                     | ``mailto:support@company.com`` or                             |
|                     | ``http://company.com/support``.                               |
+---------------------+---------------------------------------------------------------+
| categories          | A string containing a list of categories, joined with ``;``.  |
|                     | For example: ``fm;analogue;delay``.                           |
+---------------------+---------------------------------------------------------------+
| type                | Bitfield describing what the plugin does. See                 |
|                     | ``enum clap_plugin_type``.                                    |
+---------------------+---------------------------------------------------------------+
| chunk_size          | The process buffer, must have a number of sample multiple of  |
|                     | ``chunk_size``.                                               |
+---------------------+---------------------------------------------------------------+
| latency             | The latency introduced by the plugin.                         |
+---------------------+---------------------------------------------------------------+
| has_gui             | True if the plugin can show a graphical user interface        |
+---------------------+---------------------------------------------------------------+
| supports_tuning     | True if the plugin supports tuning                            |
+---------------------+---------------------------------------------------------------+
| supports_microtones | True if the plugin supports micro tones                       |
+---------------------+---------------------------------------------------------------+
| host_data           | Reserved pointer for the host.                                |
+---------------------+---------------------------------------------------------------+
| plugin_data         | Reserved pointer for the plugin.                              |
+---------------------+---------------------------------------------------------------+

Audio ports configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

A plugin may have multiple audio ports, and so multiple audio ports
layout or configurations.

An audio port has a type: mono, stereo, surround and a role: main
input/output, sidechain, feedback.

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

So for the following configuration:

+--------+----------+------------+---------------------+-----------------+
| in/out | type     | role       | buffer              | desc            |
+========+==========+============+=====================+=================+
| input  | stereo   | inout      | process->inputs[0]  | left input      |
| input  |          |            | process->inputs[1]  | right input     |
+--------+----------+------------+---------------------+-----------------+
| input  | stereo   | sidechain  | process->inputs[2]  | left sidechain  |
| input  |          |            | process->inputs[3]  | right sidechain |
+--------+----------+------------+---------------------+-----------------+
| input  | stereo   | feedback   | process->inputs[4]  | left feedback   |
| input  |          |            | process->inputs[5]  | right feedback  |
+--------+----------+------------+---------------------+-----------------+
| output | stereo   | inout      | process->outputs[0] | left input      |
| output |          |            | process->outputs[1] | right input     |
+--------+----------+------------+---------------------+-----------------+
| output | stereo   | feedback   | process->outputs[2] | left feedback   |
| output |          |            | process->outputs[3] | right feedback  |
+--------+----------+------------+---------------------+-----------------+

Available configurations
````````````````````````

It is possible to discover a plugin's port configurations by calling
``plugin->get_ports_configs_count(plugin);``. It returns the number of
configurations. Then for each configuration you have to call
``plugin->get_ports_config(plugin, config_index, &config);`` which will
tell you the number of input and output ports. Then to get the port details,
you have to call
``plugin->get_port_info(plugin, config_index, port_index, &port);``.

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
To do that it has to call ``plugin->set_port_repeat(plugin, port_index, count)``.
If it returns ``false`` then the plugin is in the same state as before
the call.

Feedback stream
```````````````

Feedback stream are used to plug external audio processing into one
of the plugin feedback loop.

A practical usage is to put an effect in a delay feedback loop.

A feedback loop has it's both ends identified by ``clap_channel->stream_id``.

During the audio processing, ``struct clap_process`` contains a callback which
is used to process the feedback stream:

.. code:: c

  void my_plugin_process(struct clap_plugin *plugin, struct clap_process *process)
  {
    uint32_t fb_in;     // index to the stereo feedback input buffer
    uint32_t fb_out;    // index to the stereo feedback output buffer
    uint32_t stream_id; // the feedback stream id
    uint32_t offset;

    // process all the buffer 
    for (offset = 0; offset < process->nb_samples;
         offset += process->feedback_chunk_size)
    {

      // ...

      // prepare feedback output buffer
      for (uint32_t i = 0; i < process->feedback_chunk_size; ++i) {
        process->output[fb_out][offset + i]     = XXX;
        process->output[fb_out + 1][offset + i] = XXX;
      }

      // process one sample feedback
      process->feedback(process, stream_id, process->feedback_chunk_size);

      // audio processing of the feedback values:
      //   process->input[fb_in][offset + i]
      //   process->input[fb_in + 1][offset + i]
    }
  }

Threading
---------

The plugin must be thread safe.

Activation
----------

Before doing any processing, the plugin must be activated by calling
``bool succeed = plugin->activate(plugin);``.

If ``succeed == true`` then the activation succeed. If the activation failed,
then the plugin is unusable.

The host must not call ``activate()`` if the plugin is already activated.
Yet the plugin should handle correctly double calls to ``activate()``.

The plugin activation could be nothing, or could be a task which takes time,
like connecting a remote server. So the host should not activate plugins in
the audio processing thread.

To deactivate the plugin, just call ``plugin->deactivate(plugin)``. Like
``activate()``, ``deactivate()`` should not be called from the audio processing
thread as it may take time.

Also ``deactivate()`` should not be called if the plugin is not activated.
Yet the plugin should handle a call to ``deactivate()`` even if it is
not activated.

It is preferable to de-activate the plugin before destroying it.

Processing
----------

The processing is done in one call: ``plugin->process(plugin, process);``.
The data structure process regroup everything needed by the plugin:

- audio buffers (in, out)
- feedback process callback
- events (in)
- tempo, time, is offline? (in)
- more processing needed (out)

Audio buffers
~~~~~~~~~~~~~

- The audio buffers are allocated by the host. They must be aligned by the
  maximum requirement of the vector instructions currently available.
- In-place processing is not supported.
- The number of samples must be a multiple of ``plugin->chunk_size``.
- See `Pin layout`_.

Events
~~~~~~

- Event's time must be within the process duration:
  ``[process->steady_time .. process->steady_time + process->nb_sambles]``.
- The plugin must not modify the input events (``in_events``).

Notes
`````

Notes are represented as a pair ``note, division``.
Division is the number of intervals between one note and an other note with
half or the double frequency. A division by 12 must be supported.

If a plugin plugin does not support micro tones, it should ignore
micro-tuned notes.

The host should not send micro tones to the plugin if
``plugin->supports_microtones == false``.

Parameters
``````````

Parameters can be automated by the host using ``CLAP_EVENT_PARAM_SET`` or
``CLAP_EVENT_PARAM_RAMP``.

When using ``CLAP_EVENT_PARAM_RAMP``, the value of the parameter has to be
incremented by ``event->param.increment`` each steps until an other event
occur on this parameter.

Pitch
`````

The pitch is the frequency of the note A4. Its default value is 440Hz.
The pitch can be changed by the host using the ``CLAP_EVENT_PITCH_SET`` event.

Parameters
----------

The host can get the plugin's parameters tree by calling:

- ``plugin->get_params_count(plugin);`` to know the number of parameters
- ``plugin->get_param(plugin, param_index, &param);`` to get the parameter
  value and description

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

When a parameter is modified by an other parameter, for example imagine you
have a parameter modulating "absolutely" an other one through an XY mapping.
The host should record the modulation source but not the modulation target.
To do that the plugin uses ``clap_event_param->is_recordable``.

Graphical User Interface
------------------------

Showing the GUI
~~~~~~~~~~~~~~~

The plugin should show the GUI after a call to ``plugin->show_gui(plugin)``.
If the plugin could successfully show the GUI, it returns ``true``, ``false``
otherwise.

Sending events to the host
~~~~~~~~~~~~~~~~~~~~~~~~~~

The plugin can notify the host of parameter changes by sending events to:
``host->events(host, plugin, events);``.

Events sent to the host should be stamped:

.. code:: c

  struct clap_event ev;
  // ...
  ev.sample_offset = host->steady_time(host);
  host->events(host, plugin, &ev);

Hiding the GUI
~~~~~~~~~~~~~~

The plugin should hide the GUI after a call to ``plugin->hide_gui(plugin)``.
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
As embedding is not a Clap requirement, it is offered as an extension.
Also the OS dependency brought by this feature makes it ideal as an extension.

+------------+-----------------------+----------------------+----------------+
| GUI        | header                | extension            | comment        |
+============+=======================+======================+================+
| Generic    | `clap-embed.h`_       | ``CLAP_EMBED``       | For the host   |
+------------+-----------------------+----------------------+----------------+
| Windows    | `clap-embed-win32.h`_ | ``CLAP_EMBED_WIN32`` | For the plugin |
+------------+-----------------------+----------------------+----------------+
| X11        | `clap-embed-xlib.h`_  | ``CLAP_EMBED_XLIB``  | For the plugin |
+------------+-----------------------+----------------------+----------------+

Sample on Windows
`````````````````

.. code:: c

  #include <clap/clap.h>
  #include <clap/clap-embed-win32.h>

  struct clap_embed_win32 *embed = plugin->get_extension(CLAP_EMBED_WIN32);
  if (embed) {
    // the plugin can embed
    embed->embed(plugin, window);
  }
  plugin->show_gui(plugin);


Presets
-------

List plugin's presets
~~~~~~~~~~~~~~~~~~~~~

The host can browse the plugin's preset by calling:

- ``plugin->get_presets_count(plugin);`` to know how many presets it has.
- ``plugin->get_preset(plugin, preset_index, &preset);`` to get a preset
  details.

Load a preset
~~~~~~~~~~~~~

To load a preset, the host have to send an event ``CLAP_EVENT_PRESET_SET`` to
the plugin.

When a preset is loaded from the plugin's GUI, the plugin must send a
``CLAP_EVENT_PRESET_SET`` to the host.

Save and restore plugin's state
-------------------------------

Saving the plugin's state is done by:

.. code:: c

  void *buffer = NULL;
  size_t size = 0;
  if (!plugin->save(plugin, &buffer, &size)) {
    // save failed
  } else {
    // save succeed
  }

Restoring the plugin's state is done by:

.. code:: c

  plugin->restore(plugin, buffer, size);

The state of the plugin should be independent of the machine: you can save a
plugin state on a little endian machine and send it through the network to a
big endian machine, it should load again successfully.

Extension system
----------------

To extend clap's functionnality, there is a pretty simple mechanism:

.. code:: c

  void *plug_ext = plugin->extension(plug, "company/ext-name");
  void *host_ext = host->extension(host, "company/ext-name");

If the extension is not supported, the plugin should return ``NULL``.

Examples
========

samples/clap-info.c
-------------------

.. include:: samples/clap-info.c
   :code: c

References
==========

clap.h
------

.. include:: include/clap/clap.h
   :code: c

clap-embed.h
------------

.. include:: include/clap/clap-embed.h
   :code: c

clap-embed-win32.h
------------------

.. include:: include/clap/clap-embed-win32.h
   :code: c

clap-embed-xlib.h
-----------------

.. include:: include/clap/clap-embed-xlib.h
   :code: c

