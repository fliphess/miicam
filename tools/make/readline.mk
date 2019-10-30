#################################################################
## READLINE													##
#################################################################

READLINEVERSION := $(shell cat $(SOURCES) | jq -r '.readline.version' )
READLINEARCHIVE := $(shell cat $(SOURCES) | jq -r '.readline.archive' )
READLINEURI     := $(shell cat $(SOURCES) | jq -r '.readline.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(READLINEARCHIVE): $(SOURCEDIR)
	$(call box,"Downloading readline source code")
	test -f $@ || $(DOWNLOADCMD) $@ $(READLINEURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/readline: $(SOURCEDIR)/$(READLINEARCHIVE)
	$(call box,"Building readline")
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(READLINEVERSION)
	@tar -xzf $(SOURCEDIR)/$(READLINEARCHIVE) -C $(BUILDDIR)
	@cd $@-$(READLINEVERSION)			\
	&& $(BUILDENV)						\
		./configure						\
			--host=$(TARGET)			\
			--prefix=$(PREFIXDIR)		\
			--disable-static			\
			--enable-shared				\
		&& make -j$(PROCS)				\
		&& make -j$(PROCS) install
	@rm -rf $@-$(READLINEVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
