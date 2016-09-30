#ifndef CLAP_EXT_PRESETS_H
# define CLAP_EXT_PRESETS_H

# include "../clap.h"

# define CLAP_EXT_PRESETS "clap/presets"

struct clap_preset_info
{
  char     name[CLAP_NAME_SIZE];       // display name
  char     desc[CLAP_DESC_SIZE];       // desc and how to use it
  char     author[CLAP_NAME_SIZE];
  char     categories[CLAP_TAGS_SIZE]; // "cat1;cat2;cat3;..."
  char     tags[CLAP_TAGS_SIZE];       // "tag1;tag2;tag3;..."
  uint8_t  score;                      // 0 = garbage, ..., 100 = best
};

struct clap_preset_handle
{
  void *priv;
  int32_t num_presets;
  // TODO cookie for fast rescan of huge bank file
};

/* The principle behind this extension is that the host gets a list of
 * directories to scan recursively, and then for each files, it can ask
 * the interface to load the preset. */
struct clap_preset_library
{
  /* Copies at most *path_size bytes into path.
   * If directory_index is bigger than the number of directories,
   * then return false. */
  bool (*get_directory)(struct clap_preset_library *reader,
                        int                         directory_index,
                        char                       *path,
                        int32_t                    *path_size);

  bool (*open_bank)(struct clap_plugin_preset_reader *reader,
                      const char *path,
                      struct clap_preset_handle *handle);

  void (*close_bank)(struct clap_plugin_preset_reader *reader,
                     struct clap_preset_handle *handle);

  /* Get a preset info from its path and returns true.
   * In case of a preset bank file, index is used, and *has_next
   * should be set to false when index reaches the last preset.
   * If the preset is not found, then it should return false. */
  bool (*get_preset_info)(struct clap_plugin_presets *presets,
                          void *cookie,
                          struct clap_preset_info    *preset_info,
                          int32_t                     index,
                          bool                       *has_next);

  /* Get the current preset info */
  bool (*get_current_preset_info)(struct clap_plugin      *plugin,
                                  struct clap_preset_info *preset);

  /* Loads the preset at path, and in case of a preset bank,
   * the one at index. */
  bool (*load_preset)(struct clap_plugin *plugin,
                      const char         *path,
                      int32_t             index);
};

#endif /* !CLAP_EXT_PRESETS_H */
