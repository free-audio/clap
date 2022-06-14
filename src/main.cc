#include <clap/clap.h>

// The purpose of this file is to check that all headers compile

static const constexpr clap_version m = CLAP_VERSION;

int main(int, char **) {
   clap_param_info param_info;
   param_info.module[0] = 'm';

   return !clap_version_is_compatible(m);
}