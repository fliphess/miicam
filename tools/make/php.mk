#################################################################
## PHP														 ##
#################################################################

PHPVERSION := $(shell cat $(SOURCES) | jq -r '.php.version' )
PHPARCHIVE := $(shell cat $(SOURCES) | jq -r '.php.archive' )
PHPURI     := $(shell cat $(SOURCES) | jq -r '.php.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(PHPARCHIVE): $(SOURCEDIR)
	$(call box,"Downloading php source code")
	test -f $@ || $(DOWNLOADCMD) $@ $(PHPURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/php: $(SOURCEDIR)/$(PHPARCHIVE) $(BUILDDIR)/zlib $(BUILDDIR)/libxml2 $(BUILDDIR)/libjpeg-turbo $(BUILDDIR)/libpng $(BUILDDIR)/pcre $(BUILDDIR)/libgd
	$(call box,"Building php")
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(PHPVERSION)
	@tar -xjf $(SOURCEDIR)/$(PHPARCHIVE) -C $(BUILDDIR)
	@sed -i -e '/.*hp_ini_register_extensions.*/d' $@-$(PHPVERSION)/main/main.c
	@cd $@-$(PHPVERSION)					\
	&& $(BUILDENV)							\
	LIBS='-ldl'								\
		./configure							\
			--prefix=$(PREFIXDIR)			\
			--host=$(TARGET)				\
			--target=$(TARGET)				\
			--program-prefix="arm-"			\
			--with-config-file-path=/etc	\
			--with-libxml-dir=$(PREFIXDIR)	\
			--with-jpeg-dir=$(PREFIXDIR)	\
			--with-png-dir=$(PREFIXDIR)		\
			--with-zlib-dir=$(PREFIXDIR)	\
			--with-mhash					\
			--with-pdo-mysql				\
			--with-sqlite3					\
			--with-pdo-sqlite				\
			--with-xmlrpc					\
			--with-zlib						\
			--with-pcre-regex				\
			--with-pcre-jit					\
			--with-gd						\
			--with-xpm-dir=no				\
			--without-pear					\
			--without-libzip                \
			--without-xsl					\
			--enable-pdo					\
			--enable-simplexml				\
			--enable-json					\
			--enable-sockets				\
			--enable-fpm					\
			--enable-libxml					\
			--enable-ftp					\
			--enable-mbstring				\
			--enable-mbregex				\
			--enable-mbregex-backtrack  	\
			--enable-hash					\
			--enable-xml					\
			--enable-session				\
			--enable-soap					\
			--enable-tokenizer				\
			--enable-xmlreader				\
			--enable-xmlwriter				\
			--enable-dom					\
			--enable-zip					\
			--disable-mbregex				\
			--disable-opcache				\
			--disable-all					\
		&& make -j$(PROCS)					\
		&& make -j$(PROCS) install-binaries
	@rm -rf $@-$(PHPVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
