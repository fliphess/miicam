#################################################################
## DOSFSTOOLS													##
#################################################################

DOSFSTOOLSVERSION := $(shell cat $(SOURCES) | jq -r '.dosfstools.version' )
DOSFSTOOLSARCHIVE := $(shell cat $(SOURCES) | jq -r '.dosfstools.archive' )
DOSFSTOOLSURI     := $(shell cat $(SOURCES) | jq -r '.dosfstools.uri' )

#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(DOSFSTOOLSARCHIVE): $(SOURCEDIR)
	$(call box,"Downloading dosfstools sourcecode")
	test -f $@ || $(DOWNLOADCMD) $@ $(DOSFSTOOLSURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/dosfstools: $(SOURCEDIR)/$(DOSFSTOOLSARCHIVE)
	$(call box,"Building dosfstools")
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(DOSFSTOOLSVERSION)
	@tar -xzf $(SOURCEDIR)/$(DOSFSTOOLSARCHIVE) -C $(BUILDDIR)
	@cd $@-$(DOSFSTOOLSVERSION)			\
	&& $(BUILDENV)						\
		./configure						\
			--host=$(TARGET)			\
			--prefix=$(PREFIXDIR)		\
			--disable-static			\
			--enable-shared				\
		&& make -j$(PROCS)				\
		&& make -j$(PROCS) install
	@rm -rf $@-$(DOSFSTOOLSVERSION)
	@touch $@

#################################################################
##                                                             ##
#################################################################
