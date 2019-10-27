#################################################################
## ZLIB														##
#################################################################

ZLIBVERSION := $(shell cat $(SOURCES) | jq -r '.zlib.version' )
ZLIBARCHIVE := $(shell cat $(SOURCES) | jq -r '.zlib.archive' )
ZLIBURI     := $(shell cat $(SOURCES) | jq -r '.zlib.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(ZLIBARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(ZLIBURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/zlib: $(SOURCEDIR)/$(ZLIBARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(ZLIBVERSION)
	tar -xzf $(SOURCEDIR)/$(ZLIBARCHIVE) -C $(BUILDDIR)
	@cd $@-$(ZLIBVERSION)			&& \
	$(BUILDENV)						\
		./configure					\
			--prefix=$(PREFIXDIR)	\
			--enable-shared			&& \
		make -j$(PROCS)				&& \
		make -j$(PROCS) install
	@rm -rf $@-$(ZLIBVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
