#################################################################
## LIBPNG													  ##
#################################################################

LIBPNGVERSION := 1.6.36
LIBPNGARCHIVE := libpng-$(LIBPNGVERSION).tar.gz
LIBPNGURI     := https://prdownloads.sourceforge.net/libpng/$(LIBPNGARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LIBPNGARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(LIBPNGURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/libpng: $(SOURCEDIR)/$(LIBPNGARCHIVE)
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
