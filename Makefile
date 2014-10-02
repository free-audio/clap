all: spec.html

.PHONY: spec.html

syntax.css:
	pygmentize -S monokai -f html -a .code >$@

spec.html: spec.rst syntax.css style.css
	rst2html --syntax-highlight=short --stylesheet=style.css,syntax.css $< >$@
