#################################################################
## VIM														 ##
#################################################################

VIMVERSION := 8.1.0629
VIMARCHIVE := vim-$(VIMVERSION)
VIMURI     := https://github.com/vim/vim/archive/v$(VIMVERSION).tar.gz


#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(VIMARCHIVE): $(SOURCEDIR)
	$(DOWNLOADCMD) $@ $(VIMURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/vim: $(SOURCEDIR)/$(VIMARCHIVE) $(BUILDDIR)/ncurses $(BUILDDIR)/readline $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(VIMVERSION)
	@tar -xzf $(SOURCEDIR)/$(VIMARCHIVE) -C $(BUILDDIR)
	@cd $@-$(VIMVERSION)						&& \
	export vim_cv_tgetent=zero					&& \
	export vim_cv_toupper_broken="set"			&& \
	export vim_cv_terminfo="yes"				&& \
	export vim_cv_getcwd_broken="yes"			&& \
	export vim_cv_tty_group=0					&& \
	export vim_cv_tty_mode="0750"				&& \
	export vim_cv_stat_ignores_slash="yes"		&& \
	export vim_cv_memmove_handles_overlap="yes"	&& \
	$(BUILDENV)									\
	CC="$(TARGET)-gcc"							\
		./configure								\
			--host=$(TARGET)					\
			--prefix=$(PREFIXDIR)				\
			--enable-multibyte					\
			--enable-gui=no						\
			--disable-gpm						\
			--disable-gtktest					\
			--disable-xim						\
			--disable-pythoninterp				\
			--disable-python3interp				\
			--disable-rubyinterp				\
			--disable-luainterp					\
			--disable-netbeans					\
			--with-features=normal				\
			--with-tlib=ncurses					\
			--without-x							&& \
		make VIMRCLOC=/etc VIMRUNTIMEDIR=/tmp/sd/firmware/share/vim CC=$(TARGET)-gcc -j$(PROCS) && \
		make -j$(PROCS) install
	@rm -rf $@-$(VIMVERSION)
	@touch $@

#################################################################
##                                                             ##
#################################################################
