add_library(thyns SHARED
  defs.h
  env.h
  filt.h
  osc.h
  thyns.h

  plugin.c)
target_link_libraries(thyns m)
