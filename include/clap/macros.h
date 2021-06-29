#pragma once

// Define CLAP_EXPORT
#if defined _WIN32 || defined __CYGWIN__
#   ifdef __GNUC__
#      define CLAP_EXPORT __attribute__((dllexport))
#   else
#      define CLAP_EXPORT __declspec(dllexport)
#   endif
#else
#   if __GNUC__ >= 4 || defined(__clang__)
#      define CLAP_EXPORT __attribute__((visibility("default")))
#   else
#      define CLAP_EXPORT
#   endif
#endif

#if defined(__cplusplus) && __cplusplus >= 201103L
#   define CLAP_CONSTEXPR constexpr
#else
#   define CLAP_CONSTEXPR
#endif