#################################################################
## NANO														##
#################################################################

NANOVERSION := 2.9.8
NANOARCHIVE := nano-$(NANOVERSION).tar.gz
NANOURI     := https://www.nano-editor.org/dist/v2.9/$(NANOARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(NANOARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(NANOURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/nano: $(SOURCEDIR)/$(NANOARCHIVE) $(BUILDDIR)/ncurses
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(NANOVERSION)
	@tar -xzf $(SOURCEDIR)/$(NANOARCHIVE) -C $(BUILDDIR)
	@cd $@-$(NANOVERSION)				&& \
	$(BUILDENV)							\
	CFLAGS="-O2 -Wall"					\
	CPPFLAGS="-P -I$(PREFIXDIR)/include -I$(PREFIXDIR)/include/ncurses -L$(PREFIXDIR)/lib/" \
		./configure						\
			--host=$(TARGET)			\
			--prefix=$(PREFIXDIR)		\
			--disable-mouse				\
			--disable-browser			\
			--disable-nls				\
			--disable-dependency-tracking && \
		make -j$(PROCS)					&& \
		make -j$(PROCS) install
	@rm -rf $@-$(NANOVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
