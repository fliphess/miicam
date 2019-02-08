#################################################################
## RSYNC													   ##
#################################################################

RSYNCVERSION := 3.1.3
RSYNCARCHIVE := rsync-$(RSYNCVERSION).tar.gz
RSYNCURI     := https://download.samba.org/pub/rsync/src/$(RSYNCARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(RSYNCARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(RSYNCURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/rsync: $(PREFIXDIR)/bin $(SOURCEDIR)/$(RSYNCARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(RSYNCVERSION)
	@tar -xzf $(SOURCEDIR)/$(RSYNCARCHIVE) -C $(BUILDDIR)
	@cd $@-$(RSYNCVERSION)					&& \
	$(BUILDENV)								\
		./configure							\
			--host=$(TARGET)				\
			--target=$(TARGET)				&& \
		make -j$(PROCS)						&& \
	cp rsync $(PREFIXDIR)/bin/rsync
	@rm -rf $(BUILDDIR)/$@-$(RSYNCVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
