#################################################################
## Libpopt													 ##
#################################################################

LIBPOPTVERSION := $(shell cat $(SOURCES) | jq -r '.libpopt.version' )
LIBPOPTARCHIVE := $(shell cat $(SOURCES) | jq -r '.libpopt.archive' )
LIBPOPTURI     := $(shell cat $(SOURCES) | jq -r '.libpopt.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LIBPOPTARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(LIBPOPTURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/popt: $(SOURCEDIR)/$(LIBPOPTARCHIVE) $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBPOPTVERSION)
	tar -xzf $(SOURCEDIR)/$(LIBPOPTARCHIVE) -C $(BUILDDIR)
	cd $@-$(LIBPOPTVERSION)			&& \
	$(BUILDENV)						\
		./configure					\
			--host=$(TARGET)		\
			--prefix=$(PREFIXDIR)	\
			--enable-shared			\
			--disable-static		&& \
		make -j$(PROCS)				&& \
		make -j$(PROCS) install
	rm -rf $@-$(LIBPOPTVERSION)
	touch $@


#################################################################
##                                                             ##
#################################################################
