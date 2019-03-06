#################################################################
## READLINE													##
#################################################################

READLINEVERSION := 7.0
READLINEARCHIVE := readline-$(READLINEVERSION).tar.gz
READLINEURI     := https://ftp.gnu.org/gnu/readline/$(READLINEARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(READLINEARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(READLINEURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/readline: $(SOURCEDIR)/$(READLINEARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(READLINEVERSION)
	@tar -xzf $(SOURCEDIR)/$(READLINEARCHIVE) -C $(BUILDDIR)
	@cd $@-$(READLINEVERSION)			&& \
	$(BUILDENV)							\
		./configure						\
			--host=$(TARGET)			\
			--prefix=$(PREFIXDIR)		\
			--disable-static			\
			--enable-shared				&& \
		make -j$(PROCS)					&& \
		make -j$(PROCS) install
	@rm -rf $@-$(READLINEVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
