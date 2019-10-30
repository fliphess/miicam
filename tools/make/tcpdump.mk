#################################################################
## LIBPCAP/TCPDUMP											 ##
#################################################################

LIBPCAPVERSION := $(shell cat $(SOURCES) | jq -r '.libpcap.version' )
LIBPCAPARCHIVE := $(shell cat $(SOURCES) | jq -r '.libpcap.archive' )
LIBPCAPURI     := $(shell cat $(SOURCES) | jq -r '.libpcap.uri' )

TCPDUMPVERSION := $(shell cat $(SOURCES) | jq -r '.tcpdump.version' )
TCPDUMPARCHIVE := $(shell cat $(SOURCES) | jq -r '.tcpdump.archive' )
TCPDUMPURI     := $(shell cat $(SOURCES) | jq -r '.tcpdump.uri' )


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LIBPCAPARCHIVE): $(SOURCEDIR)
	$(call box,"Downloading libpcap source code")
	test -f $@ || $(DOWNLOADCMD) $@ $(LIBPCAPURI) || rm -f $@


$(SOURCEDIR)/$(TCPDUMPARCHIVE): $(SOURCEDIR)
	$(call box,"Downloading tcpdump source code")
	test -f $@ || $(DOWNLOADCMD) $@ $(TCPDUMPURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/libpcap: $(SOURCEDIR)/$(LIBPCAPARCHIVE)
	$(call box,"Building libpcap")
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBPCAPVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBPCAPARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBPCAPVERSION)			\
	&& $(BUILDENV)						\
		./configure						\
			--host=$(TARGET)			\
			--prefix="$(PREFIXDIR)"		\
			--disable-canusb			\
			--with-pcap=linux			\
			ac_cv_type_u_int64_t=yes	\
		&& make -j$(PROCS)				\
		&& make -j$(PROCS) install
	@rm -rf $@-$(LIBPCAPVERSION)
	@touch $@


$(BUILDDIR)/tcpdump: $(BUILDDIR)/libpcap $(SOURCEDIR)/$(TCPDUMPARCHIVE)
	$(call box,"Building tcpdump")
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(TCPDUMPVERSION)
	@tar -xzf $(SOURCEDIR)/$(TCPDUMPARCHIVE) -C $(BUILDDIR)
	@cd $@-$(TCPDUMPVERSION)				\
	&& export CROSS_COMPILE="$(TARGET)-"	\
	&& $(BUILDENV)							\
		CC="$(TOOLCHAINDIR)/$(TARGET)-gcc"	\
		LIBS='-lpcap'						\
		./configure							\
			--host=$(TARGET)				\
			--prefix="$(PREFIXDIR)"			\
			--without-crypto				\
			ac_cv_linux_vers=3				\
		&& make -j$(PROCS)					\
		&& make -j$(PROCS) install
	@rm -rf $@-$(TCPDUMPVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
