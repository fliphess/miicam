#################################################################
## Libpopt													 ##
#################################################################

LIBPOPTVERSION := 1.16
LIBPOPTARCHIVE := popt-$(LIBPOPTVERSION).tar.gz
LIBPOPTURI     := http://rpm5.org/files/popt/popt-$(LIBPOPTVERSION).tar.gz


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LIBPOPTARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(LIBPOPTURI) || rm -f $@


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
