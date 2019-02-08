#################################################################
## SOCAT													   ##
#################################################################

SOCATVERSION := 1.7.3.2
SOCATARCHIVE := socat-$(SOCATVERSION).tar.gz
SOCATURI     := http://www.dest-unreach.org/socat/download/$(SOCATARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(SOCATARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(SOCATURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/socat: $(SOURCEDIR)/$(SOCATARCHIVE) $(BUILDDIR)/ncurses $(BUILDDIR)/readline $(BUILDDIR)/openssl
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(SOCATVERSION)
	@tar -xzf $(SOURCEDIR)/$(SOCATARCHIVE) -C $(BUILDDIR)
	cd $@-$(SOCATVERSION)				&& \
	$(BUILDENV)							\
	CPPFLAGS="-I$(PREFIXDIR)/include -DNETDB_INTERNAL=-1" \
		./configure						\
			--host=$(TARGET)			\
			--prefix=$(PREFIXDIR)		&& \
		make -j$(PROCS)					&& \
		make -j$(PROCS) install
	@rm -rf $@-$(SOCATVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
