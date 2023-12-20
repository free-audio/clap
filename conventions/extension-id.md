# Extension ID

## Naming

The extension shall be named in the form: `clap.$NAME/$REV`.
Where:
- `$NAME` is the name of the exension.
- `$REV` is the revision of the extension. This is an integer that is incremented for each iteration.

## Draft

An extension is considered a draft extension if it is in the [draft](../include/clap/ext/draft/) folder.

When the extension is migrating from draft to stable, its extension ID must not change.

All extensions must go first though a draft phase.

## History

Before this document was written, existing extension didn't honor those rules.
Some stable extension include the string `draft` in their ID.
We decided to keep those in order to maintain the binary compatibility.