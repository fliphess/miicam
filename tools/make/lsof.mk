#################################################################
## LSOF														##
#################################################################

LSOFVERSION := $(shell cat $(SOURCES) | jq -r '.lsof.version' )
LSOFARCHIVE := $(shell cat $(SOURCES) | jq -r '.lsof.archive' )
LSOFURI     := $(shell cat $(SOURCES) | jq -r '.lsof.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LSOFARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(LSOFURI) || rm -f $@


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
