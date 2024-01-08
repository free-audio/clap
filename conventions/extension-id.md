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

When the extension is migrating from draft to stable, its extension ID must not change.

All extensions must go though the draft phase first.

## For factory ID

Everything about the extension id symmetrically applies to factory id.

## History

Before this document was written, existing extensions didn't honor these rules.
Some stable extensions include the string `draft` in their ID.
We decided to keep those in order to maintain binary compatibility.