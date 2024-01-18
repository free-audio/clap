#pragma once

#include "private/std.h"
#include "private/macros.h"

// This type defines a timestamp: the number of seconds since UNIX EPOCH.
// See C's time_t time(time_t *).
typedef uint64_t clap_timestamp;

// Value for unknown timestamp.
static const CLAP_CONSTEXPR clap_timestamp CLAP_TIMESTAMP_UNKNOWN = 0;
