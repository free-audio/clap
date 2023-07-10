#pragma once

// Define CLAP_EXPORT
#if !defined(CLAP_EXPORT)
#   if defined _WIN32 || defined __CYGWIN__
#      ifdef __GNUC__
#         define CLAP_EXPORT __attribute__((dllexport))
#      else
#         define CLAP_EXPORT __declspec(dllexport)
#      endif
#   else
#      if __GNUC__ >= 4 || defined(__clang__)
#         define CLAP_EXPORT __attribute__((visibility("default")))
#      else
#         define CLAP_EXPORT
#      endif
#   endif
#endif

#if !defined(CLAP_ABI)
#   if defined _WIN32 || defined __CYGWIN__
#      define CLAP_ABI __cdecl
#   else
#      define CLAP_ABI
#   endif
#endif

#if defined(_MSVC_LANG)
#   define CLAP_CPLUSPLUS _MSVC_LANG
#elif defined(__cplusplus)
#   define CLAP_CPLUSPLUS __cplusplus
#endif

#if defined(CLAP_CPLUSPLUS) && CLAP_CPLUSPLUS >= 201103L
#   define CLAP_HAS_CXX11
#   define CLAP_CONSTEXPR constexpr
#else
#   define CLAP_CONSTEXPR
#endif

#if defined(CLAP_CPLUSPLUS) && CLAP_CPLUSPLUS >= 201703L
#   define CLAP_HAS_CXX17
#   define CLAP_NODISCARD [[nodiscard]]
#else
#   define CLAP_NODISCARD
#endif

#if defined(CLAP_CPLUSPLUS) && CLAP_CPLUSPLUS >= 202002L
#   define CLAP_HAS_CXX20
#endif
