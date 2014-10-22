#include <clap/clap.h>

#define _GNU_SOURCE

#include "env.h"
#include "filt.h"
#include "osc.h"

struct thyns
{
  uint32_t sr; // sample rate
  double   pi_sr; // M_PI / sample_rate

  // osc part
  struct thyns_osc  osc1;
  struct thyns_osc  osc2;
  float             osc_mix;

  // filter
  struct thyns_filt filt;
  struct thyns_env  filt_env;
  float             filt_end_depth;

  // amp
  struct thyns_env  amp_env;
  float             amp;
};

struct clap_plugin *
clap_create(uint32_t          plugin_index,
            struct clap_host *host,
            uint32_t          sample_rate,
            uint32_t         *plugins_count)
{
  if (plugins_count)
    *plugins_count = 0;
  return NULL;
}
