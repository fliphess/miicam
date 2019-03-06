#################################################################
## LIBXML													  ##
#################################################################

LIBXML2VERSION := 2.9.8
LIBXML2ARCHIVE := libxml2-$(LIBXML2VERSION).tar.gz
LIBXML2URI     := http://xmlsoft.org/sources/$(LIBXML2ARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LIBXML2ARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(LIBXML2URI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/libxml2: $(SOURCEDIR)/$(LIBXML2ARCHIVE) $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBXML2VERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBXML2ARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBXML2VERSION)			&& \
	$(BUILDENV)							\
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
			--without-lzma				&& \
		make -j$(PROCS)					&& \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBXML2VERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
