#################################################################
## JQ                                                          ##
#################################################################

JQVERSION := 1.6
JQARCHIVE := jq-$(JQVERSION).tar.gz
JQURI     := https://github.com/stedolan/jq/releases/download/jq-$(JQVERSION)/$(JQARCHIVE)


#################################################################
##     														   ##
#################################################################

$(SOURCEDIR)/$(JQARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(JQURI) || rm -f $@


#################################################################
##     														   ##
#################################################################

$(BUILDDIR)/jq: $(SOURCEDIR)/$(JQARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(JQVERSION)
	tar -xzf $(SOURCEDIR)/$(JQARCHIVE) -C $(BUILDDIR)
	@cd $@-$(JQVERSION)						&& \
	autoreconf -fi 							&& \
	$(BUILDENV)								\
		./configure							\
			--with-oniguruma=builtin		\
			--disable-maintainer-mode		\
			--disable-valgrind				\
			--prefix=$(PREFIXDIR)			\
			--host=$(TARGET)				&& \
		make -j$(PROCS)						&& \
		make -j$(PROCS) install
	rm -rf $@-$(JQVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
