#################################################################
## NANO														##
#################################################################

NANOVERSION := $(shell cat $(SOURCES) | jq -r '.nano.version' )
NANOARCHIVE := $(shell cat $(SOURCES) | jq -r '.nano.archive' )
NANOURI     := $(shell cat $(SOURCES) | jq -r '.nano.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(NANOARCHIVE): $(SOURCEDIR)
	$(call box,"Downloading nano source code")
	test -f $@ || $(DOWNLOADCMD) $@ $(NANOURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/nano: $(SOURCEDIR)/$(NANOARCHIVE) $(BUILDDIR)/ncurses
	$(call box,"Building nano")
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(NANOVERSION)
	@tar -xzf $(SOURCEDIR)/$(NANOARCHIVE) -C $(BUILDDIR)
	@cd $@-$(NANOVERSION)				\
	&& $(BUILDENV)						\
	CFLAGS="-O2 -Wall"					\
	CPPFLAGS="-P -I$(PREFIXDIR)/include -I$(PREFIXDIR)/include/ncurses -L$(PREFIXDIR)/lib/" \
		./configure				\
			--host=$(TARGET)		\
			--prefix=$(PREFIXDIR)		\
			--disable-mouse			\
			--disable-browser		\
			--disable-nls			\
			--disable-extra                 \
			--disable-dependency-tracking 	\
			--disable-utf8                  \
		&& make -j$(PROCS)			\
		&& make -j$(PROCS) install
	@rm -rf $@-$(NANOVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
