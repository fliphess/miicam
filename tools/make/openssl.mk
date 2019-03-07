#################################################################
## OPENSSL													 ##
#################################################################

OPENSSLVERSION := "1.0.2r"
OPENSSLARCHIVE := openssl-$(OPENSSLVERSION).tar.gz
OPENSSLURI     := https://www.openssl.org/source/$(OPENSSLARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(OPENSSLARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(OPENSSLURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/openssl: $(SOURCEDIR)/$(OPENSSLARCHIVE) $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(OPENSSLVERSION)
	@tar -xzf $(SOURCEDIR)/$(OPENSSLARCHIVE) -C $(BUILDDIR)
	cd $@-$(OPENSSLVERSION)							&& \
	$(BUILDENV)										\
		./Configure no-async no-ssl2 no-ssl3 no-comp no-hw no-engine linux-armv4 -DL_ENDIAN \
			--prefix=$(PREFIXDIR)					\
			--openssldir=/tmp/sd/firmware/etc/ssl 	&& \
		make -j$(PROCS) depend						&& \
		make -j$(PROCS)								&& \
		make -j$(PROCS) install_sw
	@rm -rf $@-$(OPENSSLVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
