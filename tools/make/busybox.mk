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
	test -f $@ || $(DOWNLOADCMD) $@ $(BUSYBOXURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/busybox: $(SOURCEDIR)/$(BUSYBOXARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(BUSYBOXVERSION)
	tar -xjf $(SOURCEDIR)/$(BUSYBOXARCHIVE) -C $(BUILDDIR)
	cp /env/tools/patches/busybox.config $@-$(BUSYBOXVERSION)/.config && \
	cd $@-$(BUSYBOXVERSION) && \
	$(BUILDENV)				    \
		make ARCH=arm CROSS_COMPILE=$(TARGET)- -j$(PROCS)           && \
		make ARCH=arm CROSS_COMPILE=$(TARGET)- -j$(PROCS) install
	@touch $@
	@rm -rf $@-$(BUSYBOXVERSION)


#################################################################
##                                                             ##
#################################################################
