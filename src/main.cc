#include <clap/all.h>

// The purpose of this file is to check that all headers compile

#if CLAP_VERSION_LT(CLAP_VERSION_MAJOR, CLAP_VERSION_MINOR, CLAP_VERSION_REVISION)
#error CLAP_VERSION_LT is inconsistent
#endif

#if !CLAP_VERSION_EQ(CLAP_VERSION_MAJOR, CLAP_VERSION_MINOR, CLAP_VERSION_REVISION)
#error CLAP_VERSION_EQ is inconsistent
#endif

#if CLAP_VERSION_EQ(CLAP_VERSION_MAJOR + 1, CLAP_VERSION_MINOR, CLAP_VERSION_REVISION)
#error CLAP_VERSION_EQ is inconsistent (MAJOR)
#endif
#if CLAP_VERSION_EQ(CLAP_VERSION_MAJOR, CLAP_VERSION_MINOR + 1, CLAP_VERSION_REVISION)
#error CLAP_VERSION_EQ is inconsistent (MINOR)
#endif
#if CLAP_VERSION_EQ(CLAP_VERSION_MAJOR, CLAP_VERSION_MINOR, CLAP_VERSION_REVISION + 1)
#error CLAP_VERSION_EQ is inconsistent (REVISION)
#endif

#if !CLAP_VERSION_GE(CLAP_VERSION_MAJOR, CLAP_VERSION_MINOR, CLAP_VERSION_REVISION)
#error CLAP_VERSION_GE is inconsistent
#endif

#if CLAP_VERSION_LT(1,1,5)
#error CLAP_VERSION_GE was inroduced in 1.1.5 so we should be later than that version.
#endif

#if !CLAP_VERSION_LT(CLAP_VERSION_MAJOR + 1, CLAP_VERSION_MINOR, CLAP_VERSION_REVISION)
#error CLAP_VERSION_LT is inconsistent (MAJOR)
#endif
#if !CLAP_VERSION_LT(CLAP_VERSION_MAJOR, CLAP_VERSION_MINOR + 1, CLAP_VERSION_REVISION)
#error CLAP_VERSION_LT is inconsistent (MINOR)
#endif
#if !CLAP_VERSION_LT(CLAP_VERSION_MAJOR, CLAP_VERSION_MINOR, CLAP_VERSION_REVISION + 1)
#error CLAP_VERSION_LT is inconsistent (REVISION)
#endif

#if CLAP_VERSION_LT(CLAP_VERSION_MAJOR - 1, CLAP_VERSION_MINOR, CLAP_VERSION_REVISION)
#error CLAP_VERSION_LT is inconsistent (MAJOR)
#endif
#if CLAP_VERSION_LT(CLAP_VERSION_MAJOR, CLAP_VERSION_MINOR - 1, CLAP_VERSION_REVISION)
#error CLAP_VERSION_LT is inconsistent (MINOR)
#endif
#if CLAP_VERSION_LT(CLAP_VERSION_MAJOR, CLAP_VERSION_MINOR, CLAP_VERSION_REVISION - 1)
#error CLAP_VERSION_LT is inconsistent (REVISION)
#endif

#if (CLAP_COMPILE_TEST_CXX_VERSION >= 11) && ! defined(CLAP_HAS_CXX11)
#error CLAP_HAS_CXX11 is not defined correctly
#endif

#if (CLAP_COMPILE_TEST_CXX_VERSION >= 17) && ! defined(CLAP_HAS_CXX17)
#error CLAP_HAS_CXX17 is not defined correctly
#endif

#if (CLAP_COMPILE_TEST_CXX_VERSION >= 20) && ! defined(CLAP_HAS_CXX20)
#error CLAP_HAS_CXX20 is not defined correctly
#endif

static const CLAP_CONSTEXPR clap_version m = CLAP_VERSION;

int main(int, char **) {
   return !clap_version_is_compatible(m);
}
