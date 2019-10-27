#################################################################
## X264														##
#################################################################

X264VERSION := $(shell cat $(SOURCES) | jq -r '.x264.version' )
X264ARCHIVE := $(shell cat $(SOURCES) | jq -r '.x264.archive' )
X264URI     := $(shell cat $(SOURCES) | jq -r '.x264.uri' )


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
