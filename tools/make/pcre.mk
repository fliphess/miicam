#################################################################
## LIBPCRE													 ##
#################################################################

PCREVERSION := 8.42
PCREARCHIVE := pcre-$(PCREVERSION).zip
PCREURI     := https://ftp.pcre.org/pub/pcre/$(PCREARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(PCREARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(PCREURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/pcre: $(SOURCEDIR)/$(PCREARCHIVE) $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(PCREVERSION)
	@unzip -q $(SOURCEDIR)/$(PCREARCHIVE) -d $(BUILDDIR)
	@cd $@-$(PCREVERSION)			&& \
	$(BUILDENV)						\
		./configure					\
			--host=$(TARGET)		\
			--prefix=$(PREFIXDIR)	\
			--enable-shared			\
			--disable-static		&& \
		make -j$(PROCS)				&& \
		make -j$(PROCS) install
	@rm -rf $@-$(PCREVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
