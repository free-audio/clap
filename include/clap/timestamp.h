#pragma once

#include "private/std.h"
#include "private/macros.h"

// This type defines a timestamp: the number of seconds since UNIX EPOCH.
// See C's time_t time(time_t *).
typedef uint64_t clap_timestamp_t;

// Value for unknown timestamp.
static const CLAP_CONSTEXPR clap_timestamp_t CLAP_TIMESTAMP_UNKNOWN = 0;
