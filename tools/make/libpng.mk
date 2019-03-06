#################################################################
## LIBPNG													  ##
#################################################################

LIBPNGVERSION := 1.6.36
LIBPNGARCHIVE := v$(LIBPNGVERSION).tar.gz
LIBPNGURI     := https://github.com/glennrp/libpng/archive/$(LIBPNGARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LIBPNGARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(LIBPNGURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/libpng: $(SOURCEDIR)/$(LIBPNGARCHIVE) $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBPNGVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBPNGARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBPNGVERSION)			&& \
	$(BUILDENV)						\
		./configure					\
			--prefix=$(PREFIXDIR)	\
			--disable-static		\
			--enable-shared			\
			--host=$(TARGET)		&& \
		make -j$(PROCS)				&& \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBPNGVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
