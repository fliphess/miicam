#################################################################
## LSOF														##
#################################################################

LSOFVERSION := master
LSOFARCHIVE := $(LSOFVERSION).tar.gz
LSOFURI     := https://github.com/Distrotech/lsof/archive/$(LSOFVERSION).tar.gz


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LSOFARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(LSOFURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/lsof: $(PREFIXDIR)/bin $(SOURCEDIR)/$(LSOFARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LSOFVERSION)
	@tar -xzf $(SOURCEDIR)/$(LSOFARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LSOFVERSION)								&& \
		export LSOF_CC="$(TOOLCHAINDIR)/$(TARGET)-gcc"	&& \
		export LSOF_ARCH="$(TARGET)"					&& \
		$(BUILDENV)										\
		./Configure -n linux							&& \
		make -j$(PROCS)									&& \
		cp lsof $(PREFIXDIR)/bin/lsof
	@rm -rf $@-$(LSOFVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
