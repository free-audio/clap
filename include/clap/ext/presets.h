#ifndef CLAP_EXT_PRESETS_H
# define CLAP_EXT_PRESETS_H

# include "../clap.h"

# define CLAP_EXT_PRESETS "clap/presets"

struct clap_preset_info
{
  char     plugin_id[CLAP_ID_SIZE];    // used to identify which plugin can load the preset
  char     id[CLAP_ID_SIZE];           // used to identify a preset in a preset bank
  char     name[CLAP_NAME_SIZE];       // display name
  char     desc[CLAP_DESC_SIZE];       // desc and how to use it
  char     author[CLAP_NAME_SIZE];
  char     categories[CLAP_TAGS_SIZE]; // "cat1;cat2;cat3;..."
  char     tags[CLAP_TAGS_SIZE];       // "tag1;tag2;tag3;..."
  uint8_t  score;                      // 0 = garbage, ..., 100 = best
};

struct clap_plugin_preset
{
  /* Get the current preset info */
  bool (*get_current_preset_info)(struct clap_plugin      *plugin,
                                  struct clap_preset_info *preset);

  /* Load a preset from a bank file */
  bool (*load_preset)(struct clap_plugin *plugin,
                      const char         *path,
                      const char         *preset_id);
};

/* The principle behind this extension is that the host gets a list of
 * directories to scan recursively, and then for each files, it can ask
 * the interface to load the preset. */
struct clap_preset_library
{
  /* Copies at most *path_size bytes into path.
   * If directory_index is bigger than the number of directories,
   * then return false. */
  bool (*get_directory)(struct clap_preset_library *library,
                        int                         directory_index,
                        char                       *path,
                        int32_t                    *path_size);

  bool (*open_bank)(struct clap_plugin_preset_reader *reader,
                      const char *path,
                      struct clap_preset_handle *handle);

  void (*close_bank)(struct clap_plugin_preset_reader *reader,
                     struct clap_preset_handle *handle);

  int32_t (*get_bank_size)(struct clap_preset_library *library,
                           void *bank_handle);

  /* Get a preset info from its path and returns true.
   * In case of a preset bank file, index is used, and *has_next
   * should be set to false when index reaches the last preset.
   * If the preset is not found, then it should return false. */
  bool (*get_preset_info)(struct clap_plugin_presets *presets,
                          void *bank_handle,
                          struct clap_preset_info    *preset_info,
                          int32_t                     index);
};

#endif /* !CLAP_EXT_PRESETS_H */
