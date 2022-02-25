#pragma once

#include "private/std.h"
#include "private/macros.h"

/// We use fixed point representation of beat time and seconds time
/// Usage:
///   double x = ...; // in beats
///   clap_beattime y = round(CLAP_BEATTIME_FACTOR * x);

// This will never change
static const CLAP_CONSTEXPR int64_t CLAP_BEATTIME_FACTOR = 1LL << 31;
static const CLAP_CONSTEXPR int64_t CLAP_SECTIME_FACTOR = 1LL << 31;

typedef int64_t clap_beattime;
typedef int64_t clap_sectime;
