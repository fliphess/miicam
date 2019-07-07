#################################################################
## Libpopt													 ##
#################################################################

LIBPOPTVERSION := 1.16
LIBPOPTARCHIVE := popt-$(LIBPOPTVERSION).tar.gz
LIBPOPTURI     := http://ftp.rpm.org/mirror/popt/popt-$(LIBPOPTVERSION).tar.gz


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
