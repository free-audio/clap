# Extension ID

## Naming

The extension shall be named in the form: `clap.$NAME/$REV`.
Where:
- `$NAME` is the name of the exension.
- `$REV` is the revision of the extension. This is an integer that is incremented for each iteration. It should start at 1.

For extensions made by third-parties and not officially published by the CLAP project, please use the following form: `$REVERSE_URI.$NAME/$REV`.
Where:
- `$REVERSE_URI` would be something like `com.bitwig`.

## Draft

An extension is considered a draft extension if it is in the [draft](../include/clap/ext/draft/) folder.
Make sure to also include it in [all.h](../include/clap/all.h).

When the extension is migrating from draft to stable, its extension ID must not change.
Move its inclusion from [all.h](../include/clap/all.h) into [clap.h](../include/clap/clap.h).

All extensions must go though the draft phase first.

## For factory ID

Everything about the extension id symmetrically applies to factory id.

## History

Before version 1.2.0 when this document was written, existing extensions didn't honor these rules.
We wanted to stabilize some draft extensions without breaking compatibility, yet their extension IDs contained the string `draft`.
While these strings weren't user-facing, we still wanted to remove them, so we updated the extension IDs according to this document and introduced IDs with `_COMPAT` suffixes to provide backward compatibility with the draft versions.
