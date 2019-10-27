#################################################################
## LIBPCRE													 ##
#################################################################

PCREVERSION := $(shell cat $(SOURCES) | jq -r '.pcre.version' )
PCREARCHIVE := $(shell cat $(SOURCES) | jq -r '.pcre.archive' )
PCREURI     := $(shell cat $(SOURCES) | jq -r '.pcre.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(PCREARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(PCREURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/pcre: $(SOURCEDIR)/$(PCREARCHIVE) $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(PCREVERSION)
	@unzip -q $(SOURCEDIR)/$(PCREARCHIVE) -d $(BUILDDIR)
	@cd $@-$(PCREVERSION)			&& \
	$(BUILDENV)						\
		./configure					\
			--host=$(TARGET)		\
			--prefix=$(PREFIXDIR)	\
			--enable-shared			\
			--disable-static		&& \
		make -j$(PROCS)				&& \
		make -j$(PROCS) install
	@rm -rf $@-$(PCREVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
