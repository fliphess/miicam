#################################################################
## LOGROTATE												   ##
#################################################################

LOGROTATEVERSION := 3.15.0
LOGROTATEARCHIVE := logrotate-$(LOGROTATEVERSION).tar.gz
LOGROTATEURI     := https://github.com/logrotate/logrotate/releases/download/$(LOGROTATEVERSION)/$(LOGROTATEARCHIVE)


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
