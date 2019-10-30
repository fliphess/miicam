#################################################################
## BUSYBOX														##
#################################################################

BUSYBOXVERSION := $(shell cat $(SOURCES) | jq -r '.busybox.version' )
BUSYBOXARCHIVE := $(shell cat $(SOURCES) | jq -r '.busybox.archive' )
BUSYBOXURI     := $(shell cat $(SOURCES) | jq -r '.busybox.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(BUSYBOXARCHIVE): $(SOURCEDIR)
	$(call box,"Downloading busybox source code")
	test -f $@ || $(DOWNLOADCMD) $@ $(BUSYBOXURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/busybox: $(SOURCEDIR)/$(BUSYBOXARCHIVE)
	$(call box,"Building busybox")
	@mkdir -p $(BUILDDIR) $(PREFIXDIR)/bin $(PREFIXDIR)/sbin && rm -rf $@-$(BUSYBOXVERSION)
	@tar -xjf $(SOURCEDIR)/$(BUSYBOXARCHIVE) -C $(BUILDDIR)
	@cd $@-$(BUSYBOXVERSION) && \
	cp /env/tools/patches/busybox.config $@-$(BUSYBOXVERSION)/.config 	\
	&& $(BUILDENV)				    									\
		make ARCH=arm CROSS_COMPILE=$(TARGET)- -j$(PROCS)         		\
		&& make ARCH=arm CROSS_COMPILE=$(TARGET)- -j$(PROCS) install 	\
		&& mv $@-$(BUSYBOXVERSION)/_install/bin/busybox $(PREFIXDIR)/bin
	@touch $@
	@rm -rf $@-$(BUSYBOXVERSION)


#################################################################
##                                                             ##
#################################################################
