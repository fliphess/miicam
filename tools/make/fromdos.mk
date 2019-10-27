#################################################################
## TOFROMDOS												   ##
#################################################################

FROMDOSVERSION := $(shell cat $(SOURCES) | jq -r '.fromdos.version' )
FROMDOSARCHIVE := $(shell cat $(SOURCES) | jq -r '.fromdos.archive' )
FROMDOSURI     := $(shell cat $(SOURCES) | jq -r '.fromdos.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(FROMDOSARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(FROMDOSURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/fromdos: $(PREFIXDIR)/bin $(SOURCEDIR)/$(FROMDOSARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(FROMDOSVERSION)
	@unzip -q $(SOURCEDIR)/$(FROMDOSARCHIVE) -d $@-$(FROMDOSVERSION)
	cd $@-$(FROMDOSVERSION)/src								&& \
	fromdos Makefile										&& \
	patch --binary -p2 < /env/tools/patches/tofrodos.patch	&& \
		$(BUILDENV)											\
		make -j$(PROCS) all									&& \
		cp fromdos todos $(PREFIXDIR)/bin/
	@rm -rf $@-$(FROMDOSVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
