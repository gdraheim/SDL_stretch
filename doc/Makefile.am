AUTOMAKE_OPTIONS = 1.6 foreign

docs: manpages.ar htmpages.ar

EXTRA_DIST = make-doc.py index.htm

$(PACKAGE).spec : @top_srcdir@/$(PACKAGE).spec
	- $(LN_S) $< $@

$(PACKAGE)1.html : $(PACKAGE)1.xml
$(PACKAGE)1.xml : $(PACKAGE).spec @srcdir@/Makefile.am @srcdir@/make-doc.py \
                   $(srcdir)/make-doc.py @top_srcdir@/*.h
	- @PYTHON@ $(srcdir)/make-doc.py @top_srcdir@/*.h suffix=1 
	- test -f $(PACKAGE)3.docbook && mv $(PACKAGE)3.docbook $@ ; test -s $@
	- test -f $(PACKAGE)1.docbook && mv $(PACKAGE)1.docbook $@ ; test -s $@

$(PACKAGE).xml : $(PACKAGE)1.xml
	test -s "$<" && cp "$<" "$@"

manpages.ar : $(PACKAGE).xml
	@ if test "$(XMLTO)" = ":" \
	; then echo no xmlto found - could not regenerate "$@" - skipped \
	; echo just touching the output file "$@" ; touch "$@" \
	; else echo going to regenerate "$@" in subdir "'"man"'" \
	; echo 'test ! -d man || rm man/* ; test -d man || mkdir man' \
	;       test ! -d man || rm man/* ; test -d man || mkdir man  \
	; echo '(cd man && xmlto man ../$(PACKAGE).xml)' \
	;       (cd man && xmlto man ../$(PACKAGE).xml)  \
	; echo 'test ! -f $@ || mv $@ $@.old' \
	;       test ! -f $@ || mv $@ $@.old  \
	; echo '(cd man && ar qc ../$@ *.3)' \
	;       (cd man && ar qc ../$@ *.3)  \
	; echo '(rm man/*.3 ; rmdir man)' \
	;       (rm man/*.3 ; rmdir man)  \
	; fi ; test -s $@ || mv $@.old $@ \
        || mv $(srcdir)/$@.old $@ || test -s $@.old >$@

htmpages.ar : $(PACKAGE).xml
	@ if test "$(XMLTO)" = ":" \
	; then echo no xmlto found - could not regenerate "$@" - skipped \
	; echo just touching the output file "$@" ; touch "$@" \
	; else echo going to regenerate "$@" in subdir "'"man"'" \
	; echo 'test ! -d man || rm /* ; test -d man || mkdir man' \
	;       test ! -d man || rm man/* ; test -d man || mkdir man  \
	; echo '(cd man && xmlto html ../$(PACKAGE).xml | tee written.lst)' \
	;       (cd man && xmlto html ../$(PACKAGE).xml | tee written.lst)  \
	; echo 'test ! -f $@ || mv $@ $@.old' \
	;       test ! -f $@ || mv $@ $@.old  \
	; echo '(cd man && ar qc ../$@ *.*)' \
	;       (cd man && ar qc ../$@ *.*)  \
	; echo '(rm man/*.* ; rmdir man)' \
	;       (rm man/*.* ; rmdir man)  \
	; fi ; test -s $@ || mv $@.old $@ \
        || mv $(srcdir)/$@.old $@ || test -s $@.old >$@

mandir = $(datadir)/man
install-man3 : manpages.ar
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man3
	P=`pwd` ; cd $(DESTDIR)$(mandir)/man3 \
        && ar x $$P/manpages.ar && chmod 664 *

docdir = $(datadir)/groups/s/sd/sdl-stretch/htdocs
install-htm : htmpages.ar index.html
	$(mkinstalldirs) $(DESTDIR)$(docdir)/man3
	P=`pwd` ; cd $(DESTDIR)$(docdir)/man3 \
        && ar x $$P/htmpages.ar && chmod 664 *
	cp index.html $(DESTDIR)$(docdir)

index.html : Makefile index.htm
	sed -e 's/<!--VERSION-->/$(VERSION)/' index.htm > $@

install-docs : install-man3 install-htm
