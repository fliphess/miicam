#################################################################
## ZLIB														##
#################################################################

ZLIBVERSION := 1.2.11
ZLIBARCHIVE := zlib-$(ZLIBVERSION).tar.gz
ZLIBURI     := https://www.zlib.net/$(ZLIBARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(ZLIBARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(ZLIBURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/zlib: $(SOURCEDIR)/$(ZLIBARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(ZLIBVERSION)
	tar -xzf $(SOURCEDIR)/$(ZLIBARCHIVE) -C $(BUILDDIR)
	@cd $@-$(ZLIBVERSION)			&& \
	$(BUILDENV)						\
		./configure					\
			--prefix=$(PREFIXDIR)	\
			--enable-shared			&& \
		make -j$(PROCS)				&& \
		make -j$(PROCS) install
	@rm -rf $@-$(ZLIBVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
