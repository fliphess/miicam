#################################################################
## LIBXML													  ##
#################################################################

LIBXML2VERSION := $(shell cat $(SOURCES) | jq -r '.libxml.version' )
LIBXML2ARCHIVE := $(shell cat $(SOURCES) | jq -r '.libxml.archive' )
LIBXML2URI     := $(shell cat $(SOURCES) | jq -r '.libxml.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LIBXML2ARCHIVE): $(SOURCEDIR)
	$(call box,"Downloading libxml source code")
	test -f $@ || $(DOWNLOADCMD) $@ $(LIBXML2URI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/libxml2: $(SOURCEDIR)/$(LIBXML2ARCHIVE) $(BUILDDIR)/zlib
	$(call box,"Building libxml2")
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBXML2VERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBXML2ARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBXML2VERSION)			\
	&& $(BUILDENV)						\
		ARCH=arm						\
		Z_CFLAGS="-DHAVE_ZLIB_H=1 -DHAVE_LIBZ=1 -I$(PREFIXDIR)/include" \
		./configure						\
			--prefix=$(PREFIXDIR)		\
			--host=$(TARGET)			\
			--disable-static			\
			--enable-shared				\
			--with-zlib=$(PREFIXDIR)	\
			--without-python			\
			--without-iconv				\
		&& make -j$(PROCS)				\
		&& make -j$(PROCS) install
	@rm -rf $@-$(LIBXML2VERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
