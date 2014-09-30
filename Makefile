all: spec.html

spec.html: spec.rst
	rst2html $< >$@
