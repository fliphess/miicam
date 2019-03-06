#################################################################
## NCURSES													 ##
#################################################################

NCURSESVERSION := 6.1
NCURSESARCHIVE := ncurses-$(NCURSESVERSION).tar.gz
NCURSESURI     := https://ftp.gnu.org/pub/gnu/ncurses/$(NCURSESARCHIVE)


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(NCURSESARCHIVE): $(SOURCEDIR)
	test -f $@ || $(DOWNLOADCMD) $@ $(NCURSESURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/ncurses: $(SOURCEDIR)/$(NCURSESARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(NCURSESVERSION)
	@tar -xzf $(SOURCEDIR)/$(NCURSESARCHIVE) -C $(BUILDDIR)
	@cd $@-$(NCURSESVERSION)			&& \
	$(BUILDENV)							\
		./configure						\
			--host=$(TARGET)			\
			--prefix=$(PREFIXDIR)		\
			--with-normal				\
			--without-debug				\
			--without-ada				\
			--with-default-terminfo=/usr/share/terminfo \
			--with-terminfo-dirs="/etc/terminfo:/lib/terminfo:/usr/share/terminfo:/usr/lib/terminfo" \
			--disable-stripping         && \
		make -j$(PROCS)					&& \
		make -j$(PROCS) install
	@rm -rf $@-$(NCURSESVERSION)
	@touch $@


#################################################################
##                                                             ##
#################################################################
