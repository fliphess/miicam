#################################################################
## OPENSSL													 ##
#################################################################

OPENSSLVERSION := $(shell cat $(SOURCES) | jq -r '.openssl.version' )
OPENSSLARCHIVE := $(shell cat $(SOURCES) | jq -r '.openssl.archive' )
OPENSSLURI     := $(shell cat $(SOURCES) | jq -r '.openssl.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(OPENSSLARCHIVE): $(SOURCEDIR)
	$(call box,"Downloading openssl source code")
	test -f $@ || $(DOWNLOADCMD) $@ $(OPENSSLURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/openssl: $(SOURCEDIR)/$(OPENSSLARCHIVE) $(BUILDDIR)/zlib
	$(call box,"Building openssl")
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(OPENSSLVERSION)
	@tar -xzf $(SOURCEDIR)/$(OPENSSLARCHIVE) -C $(BUILDDIR)
	@cd $@-$(OPENSSLVERSION)						\
	&& $(BUILDENV)									\
		./Configure no-async no-ssl2 no-ssl3 no-comp no-hw no-engine linux-armv4 -DL_ENDIAN \
			--prefix=$(PREFIXDIR)					\
			--openssldir=/tmp/sd/firmware/etc/ssl 	\
		&& make -j$(PROCS) depend					\
		&& make -j$(PROCS)							\
		&& make -j$(PROCS) install_sw
	@rm -rf $@-$(OPENSSLVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
