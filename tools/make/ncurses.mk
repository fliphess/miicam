#################################################################
## NCURSES													 ##
#################################################################

NCURSESVERSION := $(shell cat $(SOURCES) | jq -r '.ncurses.version' )
NCURSESARCHIVE := $(shell cat $(SOURCES) | jq -r '.ncurses.archive' )
NCURSESURI     := $(shell cat $(SOURCES) | jq -r '.ncurses.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(NCURSESARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(NCURSESURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/ncurses: $(SOURCEDIR)/$(NCURSESARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(NCURSESVERSION)
	@tar -xzf $(SOURCEDIR)/$(NCURSESARCHIVE) -C $(BUILDDIR)
	@cd $@-$(NCURSESVERSION)			&& \
	$(BUILDENV)							\
		./configure						\
			--host=$(TARGET)			\
			--prefix=$(PREFIXDIR)		\
			--with-normal				\
			--without-debug				\
			--without-ada				\
			--with-default-terminfo=/usr/share/terminfo \
			--with-terminfo-dirs="/etc/terminfo:/lib/terminfo:/usr/share/terminfo:/usr/lib/terminfo" \
			--disable-stripping         && \
		make -j$(PROCS)					&& \
		make -j$(PROCS) install
	@rm -rf $@-$(NCURSESVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
