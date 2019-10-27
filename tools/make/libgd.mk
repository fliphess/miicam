#################################################################
## LIBGD													   ##
#################################################################

LIBGDVERSION := $(shell cat $(SOURCES) | jq -r '.libgd.version' )
LIBGDARCHIVE := $(shell cat $(SOURCES) | jq -r '.libgd.archive' )
LIBGDURI     := $(shell cat $(SOURCES) | jq -r '.libgd.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LIBGDARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(LIBGDURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/libgd: $(SOURCEDIR)/$(LIBGDARCHIVE) $(BUILDDIR)/zlib $(BUILDDIR)/libjpeg-turbo $(BUILDDIR)/libpng
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBGDVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBGDARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBGDVERSION)				&& \
	$(BUILDENV)							\
	ARCH=arm							\
		./configure						\
			--prefix=$(PREFIXDIR)		\
			--host=$(TARGET)			\
			--with-jpeg=$(PREFIXDIR)	\
			--with-png=$(PREFIXDIR)	  	\
			--with-zlib=$(PREFIXDIR)	\
			--disable-static			\
			--enable-shared				\
			--without-tiff				\
			--without-freetype			\
			--without-fontconfig		&& \
		make -j$(PROCS)					&& \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBGDVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
