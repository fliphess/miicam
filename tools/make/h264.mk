#################################################################
## X264														##
#################################################################

X264VERSION := snapshot-20181028-2245-stable
X264ARCHIVE := x264-$(X264VERSION).tar.bz2
X264URI     := https://download.videolan.org/pub/x264/snapshots/$(X264ARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(X264ARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(X264URI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/x264: $(SOURCEDIR)/$(X264ARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(X264VERSION)
	@tar -xjf $(SOURCEDIR)/$(X264ARCHIVE) -C $(BUILDDIR)
	@cd $@-$(X264VERSION)				&& \
	$(BUILDENV)							\
		./configure						\
			--host=$(TARGET)			\
			--cross-prefix=$(TARGET)-	\
			--prefix=$(PREFIXDIR)		\
			--enable-pic				\
			--disable-asm				\
			--enable-shared				&& \
		make -j$(PROCS)					&& \
		make -j$(PROCS) install-cli install-lib-shared
	@rm -rf $@-$(X264VERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
