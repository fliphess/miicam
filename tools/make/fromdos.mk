#################################################################
## TOFROMDOS												   ##
#################################################################

FROMDOSVERSION := 1713
FROMDOSARCHIVE := tofrodos-$(FROMDOSVERSION).zip
FROMDOSURI     := http://tofrodos.sourceforge.net/download/tfd1713.zip


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(FROMDOSARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(FROMDOSURI) || rm -f $@


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
