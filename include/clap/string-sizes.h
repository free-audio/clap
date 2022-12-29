#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum {
   // String capacity for names that can be displayed to the user.
   CLAP_NAME_SIZE = 256,

   // String capacity for describing a path, like a parameter in a module hierarchy or path within a
   // set of nested track groups.
   //
   // This is not suited for describing a file path on the disk, as NTFS allows up to 32K long
   // paths.
   CLAP_PATH_SIZE = 1024,

   // String capacity for descriptions
   CLAP_DESCRIPTION_SIZE = 1024,

   // String capacity for describing a filesystem file path, an web URL, ...
   CLAP_URI_SIZE = 8096,
};

#ifdef __cplusplus
}
#endif
