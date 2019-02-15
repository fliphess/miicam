#################################################################
## LIBPCAP/TCPDUMP											 ##
#################################################################

LIBPCAPVERSION := 1.9.0
LIBPCAPARCHIVE := libpcap-$(LIBPCAPVERSION).tar.gz
LIBPCAPURI     := https://www.tcpdump.org/release/$(LIBPCAPARCHIVE)

TCPDUMPVERSION := 4.9.2
TCPDUMPARCHIVE := tcpdump-$(TCPDUMPVERSION).tar.gz
TCPDUMPURI     := http://www.tcpdump.org/release/$(TCPDUMPARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LIBPCAPARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(LIBPCAPURI) || rm -f $@


$(SOURCEDIR)/$(TCPDUMPARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(TCPDUMPURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/libpcap: $(SOURCEDIR)/$(LIBPCAPARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBPCAPVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBPCAPARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBPCAPVERSION)			&& \
	$(BUILDENV)							\
		./configure						\
			--host=$(TARGET)			   \
			--prefix="$(PREFIXDIR)"		\
			--disable-canusb			   \
			--with-pcap=linux			  \
			ac_cv_type_u_int64_t=yes	&& \
		make -j$(PROCS)				 && \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBPCAPVERSION)
	@touch $@


$(BUILDDIR)/tcpdump: $(BUILDDIR)/libpcap $(SOURCEDIR)/$(TCPDUMPARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(TCPDUMPVERSION)
	@tar -xzf $(SOURCEDIR)/$(TCPDUMPARCHIVE) -C $(BUILDDIR)
	@cd $@-$(TCPDUMPVERSION)				&& \
	export CROSS_COMPILE="$(TARGET)-"		&& \
	$(BUILDENV)								\
		CC="$(TOOLCHAINDIR)/$(TARGET)-gcc"	\
		LIBS='-lpcap'						\
		./configure							\
			--host=$(TARGET)				\
			--prefix="$(PREFIXDIR)"			\
			--without-crypto				\
			ac_cv_linux_vers=3				&& \
		make -j$(PROCS)						&& \
		make -j$(PROCS) install
	@rm -rf $@-$(TCPDUMPVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
