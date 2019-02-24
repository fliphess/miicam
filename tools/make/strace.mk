#################################################################
## STRACE													   ##
#################################################################

STRACEVERSION := 4.26
STRACEARCHIVE := strace-$(STRACEVERSION).tar.xz
STRACEURI     := https://github.com/strace/strace/releases/download/v$(STRACEVERSION)/$(STRACEARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(STRACEARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(STRACEURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/strace: $(SOURCEDIR)/$(STRACEARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(STRACEVERSION)
	@tar -xf $(SOURCEDIR)/$(STRACEARCHIVE) -C $(BUILDDIR)
	@cd $@-$(STRACEVERSION)					&& \
	$(BUILDENV)								\
		./configure							\
			--prefix=$(PREFIXDIR)			\
			--host=$(TARGET)				&& \
		make -j$(PROCS)						&& \
		make -j$(PROCS) install
	@rm -rf $@-$(STRACEVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
