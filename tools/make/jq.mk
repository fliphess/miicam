#################################################################
## JQ                                                          ##
#################################################################

JQVERSION := $(shell cat $(SOURCES) | jq -r '.jq.version' )
JQARCHIVE := $(shell cat $(SOURCES) | jq -r '.jq.archive' )
JQURI     := $(shell cat $(SOURCES) | jq -r '.jq.uri' )


#################################################################
##     														   ##
#################################################################

$(SOURCEDIR)/$(JQARCHIVE): $(SOURCEDIR)
	$(call box,"Downloading jq source code")
	test -f $@ || $(DOWNLOADCMD) $@ $(JQURI) || rm -f $@


#################################################################
##     														   ##
#################################################################

$(BUILDDIR)/jq: $(SOURCEDIR)/$(JQARCHIVE)
	$(call box,"Building jq")
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(JQVERSION)
	@tar -xzf $(SOURCEDIR)/$(JQARCHIVE) -C $(BUILDDIR)
	@cd $@-$(JQVERSION)					 	\
	&& autoreconf -fi 						\
	&& $(BUILDENV)							\
		./configure							\
			--with-oniguruma=builtin		\
			--disable-maintainer-mode		\
			--disable-valgrind				\
			--prefix=$(PREFIXDIR)			\
			--host=$(TARGET)				\
		&& make -j$(PROCS)					\
		&& make -j$(PROCS) install
	@rm -rf $@-$(JQVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
