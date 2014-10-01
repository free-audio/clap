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
- Replace old concepts by modern design
- Designed to work on any operating system
- Be event oriented
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

Locate the plugins
------------------

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

A single dynamic library can contains multiple plugins.
To list them, you have to call ``clap_create`` with an index of 0 and increment
the index until ``clap_create`` returns ``NULL``.

Sample
~~~~~~

.. include:: samples/load-plugins.c
   :code: c

Description
~~~~~~~~~~~

Both the plugin and host have a few attribute giving general plugin description.

+--------------+---------------------------------------------------------------+
| Attribute    | Description                                                   |
+==============+===============================================================+
| clap_version | Described the plugin format version implemented. Should be    |
|              | initialized with CLAP_PLUGIN_VERSION,                         |
|              | or CLAP_VERSION_MAKE(1, 0, 0) if you want to only support     |
|              | version 1.0.0                                                 |
+--------------+---------------------------------------------------------------+
| id           | Unique identifier of the plugin. It should never change. It   |
|              | should be the same on 32bits or 64bits or whatever.           |
+--------------+---------------------------------------------------------------+
| name         | The name of the product.                                      |
+--------------+---------------------------------------------------------------+
| description  | A brief description of the product.                           |
+--------------+---------------------------------------------------------------+
| manufacturer | Which company made the plugin.                                |
+--------------+---------------------------------------------------------------+
| version      | A string describing the product version.                      |
+--------------+---------------------------------------------------------------+
| url          | An URL to the product homepage.                               |
+--------------+---------------------------------------------------------------+
| license      | The plugin license type, Custom, GPLv3, MIT, ...              |
+--------------+---------------------------------------------------------------+
| support      | A link to the support, it can be                              |
|              | ``mailto:support@company.com`` or                             |
|              | ``http://company.com/support``.                               |
+--------------+---------------------------------------------------------------+
| categories   | An array of categories, the plugins fits into. Eg: analogue,  |
|              | digital, fm, delay, reverb, compressor, ...                   |
+--------------+---------------------------------------------------------------+
| plugin_type  | Bitfield describing what the plugin does. See enum            |
|              | clap_plugin_type.                                             |
+--------------+---------------------------------------------------------------+
| inputs_count | The number of input buffers.                                  |
+--------------+---------------------------------------------------------------+
| outputs_count| The number of output buffers.                                 |
+--------------+---------------------------------------------------------------+
| host_data    | Reserved pointer for the host.                                |
+--------------+---------------------------------------------------------------+
| plugin_data  | Reserved pointer for the plugin.                              |
+--------------+---------------------------------------------------------------+

Threading
---------

The plugin is not thread safe, and must not be called concurrently.

Yet, show_gui() and hide_gui() have to be called from an other thread,
and can be called concurrently.

Rational: starting the GUI requires to load resources which may be done
synchronously and can take time. So to avoid blocking the audio
processing, we choose to start the GUI from a different thread that the
audio processing thread.

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
- events (in, out)
- tempo, time, is offline? (in)
- more processing needed (out)

Audio buffers
~~~~~~~~~~~~~

The audio buffers are allocated by the host. They must be aligned by the
maximum requirement of the vector instructions currently avalaible.

In-place processing is not supported.

Events
~~~~~~

Events are relative to ``process->time_in_samples``.
Their time must be positive, and included into ``[0..process->nb_samples[``.

Parameters
``````````

Parameters can be automated by the host using ``CLAP_EVENT_PARAM_SET`` or
``CLAP_EVENT_PARAM_RAMP``.

Notes
`````

Notes are reprensented as a pair ``note, division``.
Division is the number of intervals between one note and an other note with
half or the double frequency.

Pitch
`````

The pitch is the frequency of the note A. Its default value is 440Hz.
The pitch can be changed by the host using the ``CLAP_EVENT_PITCH_SET`` event.

Parameters
----------

Graphical User Interface
------------------------

Presets
-------

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

Examples
========

References
==========

.. include:: clap.h
   :code: c

