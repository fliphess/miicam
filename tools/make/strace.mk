#################################################################
## STRACE													   ##
#################################################################

STRACEVERSION := $(shell cat $(SOURCES) | jq -r '.strace.version' )
STRACEARCHIVE := $(shell cat $(SOURCES) | jq -r '.strace.archive' )
STRACEURI     := $(shell cat $(SOURCES) | jq -r '.strace.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(STRACEARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(STRACEURI) || rm -f $@


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
