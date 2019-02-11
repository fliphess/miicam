#################################################################
## WGET													       ##
#################################################################

WGETVERSION := 1.20
WGETARCHIVE := wget-$(WGETVERSION).tar.gz
WGETURI     := http://ftp.gnu.org/gnu/wget/$(WGETARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(WGETARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(WGETURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/wget: $(SOURCEDIR)/$(WGETARCHIVE) $(BUILDDIR)/openssl $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(WGETVERSION)
	@tar -xzf $(SOURCEDIR)/$(WGETARCHIVE) -C $(BUILDDIR)
	@cd $@-$(WGETVERSION)			          && \
	export LIBS="-lc -lssl -lcrypto -lz -ldl" && \
	$(BUILDENV)						             \
	CPPFLAGS="-I$(PREFIXDIR)/include/openssl -I$(PREFIXDIR)/include -L$(PREFIXDIR)/lib" \
	LDFLAGS=" -I$(PREFIXDIR)/include/openssl -I$(PREFIXDIR)/include -L$(PREFIXDIR)/lib -Wl,-rpath -Wl,/tmp/sd/firmware/lib -Wl,--enable-new-dtags" \
		./configure					   	    \
			--host=$(TARGET)		   	    \
			--prefix=$(PREFIXDIR)	   	    \
			--with-ssl=openssl              \
			--with-zlib                     \
			--with-metalink                 \
			--without-included-regex        \
			--enable-nls                    \
			--enable-dependency-tracking && \
		make -j$(PROCS)					 && \
		make -j$(PROCS) install
	@rm -rf $@-$(WGETVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
