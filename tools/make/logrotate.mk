#################################################################
## LOGROTATE												   ##
#################################################################

LOGROTATEVERSION := $(shell cat $(SOURCES) | jq -r '.logrotate.version' )
LOGROTATEARCHIVE := $(shell cat $(SOURCES) | jq -r '.logrotate.archive' )
LOGROTATEURI     := $(shell cat $(SOURCES) | jq -r '.logrotate.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LOGROTATEARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(LOGROTATEURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/logrotate: $(SOURCEDIR)/$(LOGROTATEARCHIVE) $(BUILDDIR)/popt
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LOGROTATEVERSION)
	@tar -xzf $(SOURCEDIR)/$(LOGROTATEARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LOGROTATEVERSION)		  	&& \
	$(BUILDENV)							\
		./configure						\
			--host=$(TARGET)			\
			--prefix="$(PREFIXDIR)"	 	&& \
		make -j$(PROCS)				 	&& \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBPCAPVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
