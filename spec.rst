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
- Provide a reference host
- Provide some reference plugins
- Provide a validation plugin, which should signal anything wrong the host does
- Provide a validation host, which should give hard time to the plugin and
  ensure that basic functionnality are working

Specification
=============

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

- The function ``clap_create`` has to be thread-safe.
- It should not throw exceptions.
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


Threading
---------

The plugin is not thread safe, and must not be called concurrently.

Yet, show_gui() and hide_gui() should be called from an other thread, and can be called concurrently.
Rational: starting the GUI requires to load resources which may be done
synchronously and can take time. So to avoid blocking the audio
processing, we choose to start the GUI from an other thread.

Processing
----------

Audio buffers
~~~~~~~~~~~~~

Events
~~~~~~

Parameters
``````````

Notes
`````

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

