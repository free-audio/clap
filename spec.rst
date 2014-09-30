==========================
CLAP (CLever Audio Plugin)
==========================

--------------------------
A free audio plugin format
--------------------------

Design goals
============

- Make a free audio plugin format
- Be easy to understand and implement
- Bring new features missed in VST 2.4

Specification
=============

Threading
---------

The plugin is not thread safe, and must not be called concurrently.

Yet, show_gui() and hide_gui() should be called from an other thread, and can be called concurrently.
Rational: starting the GUI requires to load resources which may be done
synchronously and can take time. So to avoid blocking the audio
processing, we choose to start the GUI from an other thread.

Examples
========

References
==========

.. include:: clap.h
   :code: c
