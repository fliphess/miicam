#################################################################
## LIGHTTPD													##
#################################################################

LIGHTTPDVERSION := 1.4.52
LIGHTTPDARCHIVE := lighttpd-$(LIGHTTPDVERSION).tar.gz
LIGHTTPDURI     := https://download.lighttpd.net/lighttpd/releases-1.4.x/$(LIGHTTPDARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LIGHTTPDARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(LIGHTTPDURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/lighttpd: $(SOURCEDIR)/$(LIGHTTPDARCHIVE) $(BUILDDIR)/zlib $(BUILDDIR)/pcre
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIGHTTPDVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIGHTTPDARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIGHTTPDVERSION)			&& \
	$(BUILDENV)							\
		./configure					 	\
			--prefix=$(PREFIXDIR)		\
			--host=$(TARGET)			\
			--with-zlib=$(PREFIXDIR)	\
			--with-pcre=$(PREFIXDIR)	\
			--with-openssl=$(PREFIXDIR) \
			--with-openssl-libs=$(PREFIXDIR)/lib \
			--disable-static			\
			--enable-shared				\
			--without-mysql				\
			--without-bzip2				&& \
		make -j$(PROCS)					&& \
		make -j$(PROCS) install
	@rm -rf $@-$(LIGHTTPDVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
