#################################################################
## SOCAT													   ##
#################################################################

SOCATVERSION := $(shell cat $(SOURCES) | jq -r '.socat.version' )
SOCATARCHIVE := $(shell cat $(SOURCES) | jq -r '.socat.archive' )
SOCATURI     := $(shell cat $(SOURCES) | jq -r '.socat.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(SOCATARCHIVE): $(SOURCEDIR)
	$(call box,"Downloading socat source code")
	test -f $@ || $(DOWNLOADCMD) $@ $(SOCATURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/socat: $(SOURCEDIR)/$(SOCATARCHIVE) $(BUILDDIR)/ncurses $(BUILDDIR)/readline $(BUILDDIR)/openssl
	$(call box,"Building socat")
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(SOCATVERSION)
	@tar -xzf $(SOURCEDIR)/$(SOCATARCHIVE) -C $(BUILDDIR)
	@cd $@-$(SOCATVERSION)				\
	&& $(BUILDENV)						\
	CPPFLAGS="-I$(PREFIXDIR)/include -DNETDB_INTERNAL=-1" \
		./configure						\
			--host=$(TARGET)			\
			--prefix=$(PREFIXDIR)		\
		&& make -j$(PROCS)				\
		&& make -j$(PROCS) install
	@rm -rf $@-$(SOCATVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
