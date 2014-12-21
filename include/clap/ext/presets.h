#ifndef CLAP_EXT_PRESETS_H
# define CLAP_EXT_PRESETS_H

# include "../clap.h"

# define CLAP_EXT_PRESETS "clap/presets"

struct clap_preset
{
  char     url[CLAP_URL_SIZE];     // location to the patch
  char     name[CLAP_NAME_SIZE];   // display name
  char     desc[CLAP_DESC_SIZE];   // desc and how to use it
  char     author[CLAP_NAME_SIZE];
  char     tags[CLAP_TAGS_SIZE];   // "tag1;tag2;tag3;..."
  uint8_t  score;                  // 0 = garbage, ..., 4 = favorite
};

struct clap_preset_iterator;

struct clap_plugin_presets
{
  /* Allocate a new preset iterator positioned at the beginning of the
   * preset collection, store its pointer into (*iter).
   * Returns true on success, false otherwise.
   * If the collection is empty, then it must return false and (*iter)
   * should be NULL. */
  bool (*iter_begin)(struct clap_plugin             *plugin,
                     struct clap_preset_iterator   **iter);

  /* Iterate to the next value. If the iterator reached the end of the
   * collection, returns false.
   * Returns true on success. */
  bool (*iter_next)(struct clap_plugin              *plugin,
                    struct clap_preset_iterator     *iter);

  /* Release the iterator.
   * If iter is NULL, then do nothing. */
  void (*iter_destroy)(struct clap_plugin           *plugin,
                       struct clap_preset_iterator  *iter);

  /* Get preset at the current iterator position.
   * Returns true on success, false otherwise. */
  bool (*iter_get)(struct clap_plugin               *plugin,
                   struct clap_preset_iterator      *iter,
                   struct clap_preset               *preset);

  /* Get a preset info by its URL and returns true.
   * If the preset is not found, then it should return false. */
  bool (*get)(struct clap_plugin *plugin,
              const char         *url,
              struct clap_preset *preset);

  /* Set the preset score. */
  void (*set_score)(struct clap_plugin *plugin,
                    const char         *url,
                    uint8_t             score);
};

#endif /* !CLAP_EXT_PRESETS_H */
