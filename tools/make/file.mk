#################################################################
## FILE                                                        ##
#################################################################

FILEVERSION := $(shell cat $(SOURCES) | jq -r '.file.version' )
FILEARCHIVE := $(shell cat $(SOURCES) | jq -r '.file.archive' )
FILEURI     := $(shell cat $(SOURCES) | jq -r '.file.uri' )

#################################################################
##     														   ##
#################################################################

$(SOURCEDIR)/$(FILEARCHIVE): $(SOURCEDIR)
	$(call box,"Downloading file sourcecode")
	test -f $@ || $(DOWNLOADCMD) $@ $(FILEURI) || rm -f $@


#################################################################
##     														   ##
#################################################################

$(BUILDDIR)/file: $(SOURCEDIR)/$(FILEARCHIVE) $(BUILDDIR)/zlib
	$(call box,"Building file")
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(FILEVERSION)
	@tar -xzf $(SOURCEDIR)/$(FILEARCHIVE) -C $(BUILDDIR)
	@cd $@-$(FILEVERSION)					\
	&& autoreconf -fi 					 	\
	&& $(BUILDENV)							\
		./configure							\
			--disable-dependency-tracking   \
			--prefix=$(PREFIXDIR)			\
			--host=$(TARGET)				\
		&& make -j$(PROCS)					\
		&& make -j$(PROCS) install
	@rm -rf $@-$(FILEVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
