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
- Replace old concepts by modern one
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

- Directories should be scanned recursively

Linux
~~~~~

- Plugins distrubuted with packages should be installed to: ``/usr/lib/clap/``
- Plugins installed in the user's home should be installed to: ``${HOME}/.clap/``

Windows
~~~~~~~

TBD

Mac
~~~

TBD

Instantiate a plugin
--------------------

Plugin instanciation can be done in a few steps:

- load the plugin library
- find the symbol ``clap_create``
- instantiate the plugin by calling ``clap_create``

Precautions
~~~~~~~~~~~

- The function ``clap_create`` must be thread-safe.
- It must not throw exceptions.
- It can return ``NULL``.

Shell plugins
~~~~~~~~~~~~~

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
| categories          | An array of categories, the plugins fits into. Eg: analogue,  |
|                     | digital, fm, delay, reverb, compressor, ...                   |
+---------------------+---------------------------------------------------------------+
| plugin_type         | Bitfield describing what the plugin does. See                 |
|                     | ``enum clap_plugin_type``.                                    |
+---------------------+---------------------------------------------------------------+
| has_gui             | True if the plugin can show a graphical user interface        |
+---------------------+---------------------------------------------------------------+
| supports_tunning    | True if the plugin supports tunning                           |
+---------------------+---------------------------------------------------------------+
| supports_microtones | True if the plugin supports micro tones                       |
+---------------------+---------------------------------------------------------------+
| host_data           | Reserved pointer for the host.                                |
+---------------------+---------------------------------------------------------------+
| plugin_data         | Reserved pointer for the plugin.                              |
+---------------------+---------------------------------------------------------------+

Audio channel configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~

A plugin may have multiple audio channels, and so multiple audio channels
layout or configurations.

An audio channel has a type: mono, stereo, surround and a role: main
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

It is possible to discover a plugin's channel configurations by calling
``plugin->get_channels_configs(plugin);``. It returns a newly allocated linked
list of configurations. It is the responsability of the host to free the list.

Selecting a configuration
`````````````````````````

Selecting an audio configuration has to be done when the plugin is deactivated.
It is done by calling ``plugin->set_channels_config(plugin, config)``.

The host should make sure that the config stays allocated/available until
an other call to ``plugin->set_channel_config()`` or
``plugin->destroy(plugin)``.

``plugin->set_channels_config(plugin, config)`` returns ``true`` if the
confiugration is successful, ``false`` otherwise.

Repeatable channels
```````````````````

Repeatable channels are a special case. A channel can be identified as
repeatable if ``channel->is_repeatable == true``.

A usefull case is for an analyzer. Imagine a spectroscope, to which you want to
plug any number of inputs. Each of those inputs can be named and displayed in
the spectrograph, so it is a convinient way analyze many tracks in the same
spectroscope.

To repeat a channel, just duplicate it and insert it between the original and
its next channel. Then call ``plugin->set_channels_config(plugin, config);``.

Feedback stream
```````````````

Feedback stream are used to plug external audio processing into one
of the plugin feedback loop.

A practical usage is to put an effect in a delay feedback loop.

A feedback loop has it's both ends identified by ``clap_channel->stream_id``.

During the audio processing, ``struct clap_process`` contains a callback which
is used to process the feeback stream:

.. code:: c

  void my_plugin_process(struct clap_plugin *plugin, struct clap_process *process)
  {
    uint32_t fb_in;     // index to the stereo feedback input buffer
    uint32_t fb_out;    // index to the stereo feedback output buffer
    uint32_t stream_id; // the feedback stream id

    // one sample process loop
    for (uint32_t i = 0; i < process->nb_samples; ++i) {
      // audio processing

      // prepare feedback output buffer
      process->output[fb_out][i]     = XXX;
      process->output[fb_out + 1][i] = XXX;

      // process one sample feedback
      process->feedback(process, stream_id, 1);

      // audio processing of the feedback values:
      //   process->input[fb_in][i]
      //   process->input[fb_in + 1][i]
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
- events (in, out)
- tempo, time, is offline? (in)
- more processing needed (out)

Audio buffers
~~~~~~~~~~~~~

The audio buffers are allocated by the host. They must be aligned by the
maximum requirement of the vector instructions currently avalaible.

In-place processing is not supported.

There is no minimum number of samples.

See `Pin layout`_.

Events
~~~~~~

- Events are relative to ``process->time_in_samples``.
- Their time must be within the process duration: positive and included
  into ``[0..process->nb_samples[`` or equal to ``0``.
- The host is responsible to allocate to allocate input events (``in_events``).
- The plugin must not modify the input events (``in_events``).
- The plugin is responsible to allocate the output events (``out_events``).
- The host is responsible to free both input and output events
  (``in_events`` and ``out_events``).

Notes
`````

Notes are reprensented as a pair ``note, division``.
Division is the number of intervals between one note and an other note with
half or the double frequency. A division by 12 must be supported.

If a plugin plugin does not support micro tones, it should ignore
micro-tunned notes.

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

Hiding the GUI
~~~~~~~~~~~~~~

The plugin should hide the GUI after a call to ``plugin->hide_gui(plugin)``.
If the plugin window has been closed by the user, then the plugin should
send an event ``CLAP_EVENT_GUI_CLOSED`` to the host.

.. code:: c

  struct clap event ev;
  ev.next = NULL;
  ev.type = CLAP_EVENT_GUI_CLOSED;
  host->events(host, plugin, &ev);


Presets
-------

List plugin's presets
~~~~~~~~~~~~~~~~~~~~~

The host can browse the plugin's preset by callign ``plugin->get_presets(plugin);``.
This function returns a newly allocated preset linked list.
It is the responsibility of the host to free the linked list.

Load a preset
~~~~~~~~~~~~~

To load a preset, the host should send an event ``CLAP_EVENT_PRESET_SET`` to
the plugin.

Save and restore plugin's state
-------------------------------

Saving the plugin's state is done by:

.. code:: c

  void *buffer = NULL;
  size_t size = 0;
  plugin->save(plugin, &buffer, &size);
  // do what you want
  free(buffer);

Restoring the plugin's state is done by:

.. code:: c

  plugin->restore(plugin, buffer, size);

The state of the plugin should be indepentant of the machine: you can save a
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

References
==========

clap.c
------

.. include:: include/clap/clap.h
   :code: c

samples/clap-info.c
-------------------

.. include:: samples/clap-info.c
   :code: c
