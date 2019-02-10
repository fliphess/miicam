#################################################################
## DOSFSTOOLS													##
#################################################################

DOSFSTOOLSVERSION := 4.1
DOSFSTOOLSARCHIVE := dosfstools-$(DOSFSTOOLSVERSION).tar.gz
DOSFSTOOLSURI     := https://github.com/dosfstools/dosfstools/releases/download/v$(DOSFSTOOLSVERSION)/$(DOSFSTOOLSARCHIVE)

#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(DOSFSTOOLSARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(DOSFSTOOLSURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/dosfstools: $(SOURCEDIR)/$(DOSFSTOOLSARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(DOSFSTOOLSVERSION)
	@tar -xzf $(SOURCEDIR)/$(DOSFSTOOLSARCHIVE) -C $(BUILDDIR)
	@cd $@-$(DOSFSTOOLSVERSION)			&& \
	$(BUILDENV)							\
		./configure						\
			--host=$(TARGET)			\
			--prefix=$(PREFIXDIR)		\
			--disable-static			\
			--enable-shared				&& \
		make -j$(PROCS)					&& \
		make -j$(PROCS) install
	@rm -rf $@-$(DOSFSTOOLSVERSION)
	@touch $@

#################################################################
##                                                             ##
#################################################################
