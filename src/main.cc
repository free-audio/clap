#include <clap/clap.h>

// The purpose of this file is to check that all headers compile

clap_version m = CLAP_VERSION;

int main(int, char **) { return !clap_version_is_compatible(m); }