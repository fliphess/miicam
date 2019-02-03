TOOLCHAINDIR := /usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin
PATH         := $(TOOLCHAINDIR):$(PATH)
TARGET       := arm-unknown-linux-uclibcgnueabi
PROCS        := 7

DOWNLOADCMD := wget -t 5 -T 10 -c -O

BUILDENV :=                 \
	AR=$(TARGET)-ar         \
	AS=$(TARGET)-as         \
	CC=$(TARGET)-gcc        \
	CXX=$(TARGET)-g++       \
	LD=${TARGET}-ld         \
	NM=$(TARGET)-nm         \
	RANLIB=$(TARGET)-ranlib \
	STRIP=$(TARGET)-strip   \
	CFLAGS="-fPIC"          \
	CPPFLAGS="-I$(PREFIXDIR)/include -L$(PREFIXDIR)/lib" \
	LDFLAGS=" -I$(PREFIXDIR)/include -L$(PREFIXDIR)/lib -Wl,-rpath -Wl,/tmp/sd/firmware/lib -Wl,--enable-new-dtags"

TOPDIR       := $(CURDIR)
SOURCEDIR    := $(TOPDIR)/src
PREFIXDIR    := $(TOPDIR)/prefix
BUILDDIR     := $(TOPDIR)/build
TOOLSDIR     := $(TOPDIR)/tools

PATCHESDIR   := $(TOOLSDIR)/patches
GMLIBDIR     := $(TOOLSDIR)/gm_lib
RTSPDDIR     := $(TOOLSDIR)/rtsp_server
UTILSDIR     := $(TOOLSDIR)/utils

BINARIESDIR  := $(TOPDIR)/sdcard/firmware/bin
LIBRARIESDIR := $(TOPDIR)/sdcard/firmware/lib

WEBROOT      := $(TOPDIR)/sdcard/firmware/www

include SOURCES.mk
include OUTPUT.mk


LIBS :=                              \
	$(BUILDDIR)/chuangmi_ircut       \
	$(BUILDDIR)/chuangmi_isp328      \
	$(BUILDDIR)/chuangmi_pwm         \
	$(BUILDDIR)/chuangmi_utils


UTILS :=                             \
	$(BUILDDIR)/chuangmi_ctrl        \
	$(BUILDDIR)/take_snapshot        \
	$(BUILDDIR)/take_video           \
	$(BUILDDIR)/ir_cut               \
	$(BUILDDIR)/ir_led               \
	$(BUILDDIR)/mirrormode           \
	$(BUILDDIR)/nightmode            \
	$(BUILDDIR)/flipmode             \
	$(BUILDDIR)/camera_adjust        \
	$(BUILDDIR)/auto_night_mode


utils: $(UTILS)

libs: $(LIBS)

website: $(WEBROOT)

all:                                 \
	$(PREFIXDIR)/bin                 \
	$(BUILDDIR)/zlib                 \
	$(BUILDDIR)/libxml2              \
	$(BUILDDIR)/libjpeg-turbo        \
	$(BUILDDIR)/libpng               \
	$(BUILDDIR)/libgd                \
	$(BUILDDIR)/pcre                 \
	$(BUILDDIR)/popt                 \
	$(BUILDDIR)/x264                 \
	$(BUILDDIR)/ncurses              \
	$(BUILDDIR)/readline             \
	$(BUILDDIR)/libpcap              \
	$(BUILDDIR)/tcpdump              \
	$(BUILDDIR)/openssl              \
	$(BUILDDIR)/socat                \
	$(BUILDDIR)/logrotate            \
	$(BUILDDIR)/sftp                 \
	$(BUILDDIR)/dropbear             \
	$(BUILDDIR)/lighttpd             \
	$(BUILDDIR)/vim                  \
	$(BUILDDIR)/nano                 \
	$(BUILDDIR)/php                  \
	$(BUILDDIR)/rsync                \
	$(BUILDDIR)/lsof                 \
	$(BUILDDIR)/strace               \
	$(BUILDDIR)/ffmpeg               \
	$(BUILDDIR)/rtspd                \
	website                          \
	libs                             \
	utils                            \
	sdcard/manufacture.bin


#################################################################
## BINDIR                                                      ##
#################################################################

$(PREFIXDIR)/bin:
	@mkdir -p $(PREFIXDIR)/bin


#################################################################
## WEB INTERFACE                                               ##
#################################################################

$(SOURCEDIR)/$(WEBSITEARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(WEBSITEURI) || rm -f $@


$(WEBROOT): $(SOURCEDIR)/$(WEBSITEARCHIVE)
	@mkdir -p $(WEBROOT)
	tar -xzf $(SOURCEDIR)/$(WEBSITEARCHIVE) -C $(WEBROOT)


#################################################################
## ZLIB                                                        ##
#################################################################

$(SOURCEDIR)/$(ZLIBARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(ZLIBURI) || rm -f $@


$(BUILDDIR)/zlib: $(SOURCEDIR)/$(ZLIBARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(ZLIBVERSION)
	tar -xzf $(SOURCEDIR)/$(ZLIBARCHIVE) -C $(BUILDDIR)
	@cd $@-$(ZLIBVERSION)             && \
	$(BUILDENV)                          \
		./configure                      \
			--prefix=$(PREFIXDIR)        \
			--enable-shared           && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	@rm -rf $@-$(ZLIBVERSION)
	@touch $@


#################################################################
## LIBXML                                                      ##
#################################################################

$(SOURCEDIR)/$(LIBXML2ARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LIBXML2URI) || rm -f $@


$(BUILDDIR)/libxml2: $(SOURCEDIR)/$(LIBXML2ARCHIVE) $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBXML2VERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBXML2ARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBXML2VERSION)          && \
	$(BUILDENV)                          \
		ARCH=arm                         \
		Z_CFLAGS="-DHAVE_ZLIB_H=1 -DHAVE_LIBZ=1 -I$(PREFIXDIR)/include" \
		./configure                      \
			--prefix=$(PREFIXDIR)        \
			--host=$(TARGET)             \
			--disable-static             \
			--enable-shared              \
			--with-zlib=$(PREFIXDIR)     \
			--without-python             \
			--without-iconv              \
			--without-lzma            && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBXML2VERSION)
	@touch $@


#################################################################
## LIBJPEG-TURBO                                               ##
#################################################################

$(SOURCEDIR)/$(LIBJPEGARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LIBJPEGURI) || rm -f $@


$(BUILDDIR)/libjpeg-turbo: $(SOURCEDIR)/$(LIBJPEGARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBJPEGVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBJPEGARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBJPEGVERSION)          && \
	$(BUILDENV)                          \
		./configure                      \
			--prefix=$(PREFIXDIR)        \
			--disable-static             \
			--enable-shared              \
			--host=$(TARGET)          && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBJPEGVERSION)
	@touch $@


#################################################################
## LIBPNG                                                      ##
#################################################################

$(SOURCEDIR)/$(LIBPNGARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LIBPNGURI) || rm -f $@


$(BUILDDIR)/libpng: $(SOURCEDIR)/$(LIBPNGARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBPNGVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBPNGARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBPNGVERSION)            && \
	$(BUILDENV)                           \
		./configure                       \
			--prefix=$(PREFIXDIR)         \
			--disable-static              \
			--enable-shared               \
			--host=$(TARGET)           && \
		make -j$(PROCS)                && \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBPNGVERSION)
	@touch $@


#################################################################
## LIBGD                                                       ##
#################################################################

$(SOURCEDIR)/$(LIBGDARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LIBGDURI) || rm -f $@


$(BUILDDIR)/libgd: $(SOURCEDIR)/$(LIBGDARCHIVE) $(BUILDDIR)/zlib $(BUILDDIR)/libjpeg-turbo $(BUILDDIR)/libpng
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBGDVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBGDARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBGDVERSION)            && \
	$(BUILDENV)                          \
	ARCH=arm                             \
		./configure                      \
			--prefix=$(PREFIXDIR)        \
			--host=$(TARGET)             \
			--with-jpeg=$(PREFIXDIR)     \
			--with-png=$(PREFIXDIR)      \
			--with-zlib=$(PREFIXDIR)     \
			--disable-static             \
			--enable-shared              \
			--without-tiff               \
			--without-freetype           \
			--without-fontconfig      && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBGDVERSION)
	@touch $@


#################################################################
## LIBPCRE                                                     ##
#################################################################

$(SOURCEDIR)/$(PCREARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(PCREURI) || rm -f $@


$(BUILDDIR)/pcre: $(SOURCEDIR)/$(PCREARCHIVE) $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(PCREVERSION)
	@unzip -q $(SOURCEDIR)/$(PCREARCHIVE) -d $(BUILDDIR)
	@cd $@-$(PCREVERSION)             && \
	$(BUILDENV)                          \
		./configure                      \
			--host=$(TARGET)             \
			--prefix=$(PREFIXDIR)        \
			--enable-shared              \
			--disable-static          && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	@rm -rf $@-$(PCREVERSION)
	@touch $@


#################################################################
## Libpopt                                                     ##
#################################################################

$(SOURCEDIR)/$(LIBPOPTARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LIBPOPTURI) || rm -f $@


$(BUILDDIR)/popt: $(SOURCEDIR)/$(LIBPOPTARCHIVE) $(BUILDDIR)/zlib
	mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBPOPTVERSION)
	tar -xzf $(SOURCEDIR)/$(LIBPOPTARCHIVE) -C $(BUILDDIR)
	cd $@-$(LIBPOPTVERSION)           && \
	$(BUILDENV)                          \
		./configure                      \
			--host=$(TARGET)             \
			--prefix=$(PREFIXDIR)        \
			--enable-shared              \
			--disable-static          && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	rm -rf $@-$(LIBPOPTVERSION)
	touch $@


#################################################################
## X264                                                        ##
#################################################################

$(SOURCEDIR)/$(X264ARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(X264URI) || rm -f $@


$(BUILDDIR)/x264: $(SOURCEDIR)/$(X264ARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(X264VERSION)
	@tar -xjf $(SOURCEDIR)/$(X264ARCHIVE) -C $(BUILDDIR)
	@cd $@-$(X264VERSION)             && \
	$(BUILDENV)                          \
		./configure                      \
			--host=$(TARGET)             \
			--cross-prefix=$(TARGET)-    \
			--prefix=$(PREFIXDIR)        \
			--enable-pic                 \
			--disable-asm                \
			--enable-shared           && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install-cli install-lib-shared
	@rm -rf $@-$(X264VERSION)
	@touch $@


#################################################################
## NCURSES                                                     ##
#################################################################

$(SOURCEDIR)/$(NCURSESARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(NCURSESURI) || rm -f $@


$(BUILDDIR)/ncurses: $(SOURCEDIR)/$(NCURSESARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(NCURSESVERSION)
	@tar -xzf $(SOURCEDIR)/$(NCURSESARCHIVE) -C $(BUILDDIR)
	@cd $@-$(NCURSESVERSION)                && \
	$(BUILDENV)                                \
		./configure                            \
			--host=$(TARGET)                   \
			--prefix=$(PREFIXDIR)              \
			--with-normal                      \
			--without-debug                    \
			--without-ada                      \
			--with-default-terminfo=/usr/share/terminfo \
			--with-terminfo-dirs="/etc/terminfo:/lib/terminfo:/usr/share/terminfo:/usr/lib/terminfo" && \
		make -j$(PROCS)                     && \
		make -j$(PROCS) install.libs install.includes
	@rm -rf $@-$(NCURSESVERSION)
	@touch $@


#################################################################
## READLINE                                                    ##
#################################################################

$(SOURCEDIR)/$(READLINEARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(READLINEURI) || rm -f $@


$(BUILDDIR)/readline: $(SOURCEDIR)/$(READLINEARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(READLINEVERSION)
	@tar -xzf $(SOURCEDIR)/$(READLINEARCHIVE) -C $(BUILDDIR)
	@cd $@-$(READLINEVERSION)               && \
	$(BUILDENV)                                \
		./configure                            \
			--host=$(TARGET)                   \
			--prefix=$(PREFIXDIR)              \
			--disable-static                   \
			--enable-shared                 && \
		make -j$(PROCS)                     && \
		make -j$(PROCS) install
	@rm -rf $@-$(READLINEVERSION)
	@touch $@


#################################################################
## LIBPCAP/TCPDUMP                                             ##
#################################################################

$(SOURCEDIR)/$(LIBPCAPARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LIBPCAPURI) || rm -f $@


$(BUILDDIR)/libpcap: $(SOURCEDIR)/$(LIBPCAPARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBPCAPVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBPCAPARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBPCAPVERSION)            && \
	$(BUILDENV)                            \
		./configure                        \
			--host=$(TARGET)               \
			--prefix="$(PREFIXDIR)"        \
			--disable-canusb               \
			--with-pcap=linux              \
			ac_cv_type_u_int64_t=yes    && \
		make -j$(PROCS)                 && \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBPCAPVERSION)
	@touch $@


$(SOURCEDIR)/$(TCPDUMPARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(TCPDUMPURI) || rm -f $@


$(BUILDDIR)/tcpdump: $(BUILDDIR)/libpcap $(SOURCEDIR)/$(TCPDUMPARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(TCPDUMPVERSION)
	@tar -xzf $(SOURCEDIR)/$(TCPDUMPARCHIVE) -C $(BUILDDIR)
	@cd $@-$(TCPDUMPVERSION)                  && \
	export CROSS_COMPILE="$(TARGET)-"         && \
	$(BUILDENV)                                  \
		CC="$(TOOLCHAINDIR)/$(TARGET)-gcc"       \
		LIBS='-lpcap'                            \
		./configure                              \
			--host=$(TARGET)                     \
			--prefix="$(PREFIXDIR)"              \
			--without-crypto                     \
			ac_cv_linux_vers=3                && \
		make -j$(PROCS)                       && \
		make -j$(PROCS) install
	@rm -rf $@-$(TCPDUMPVERSION)
	@touch $@


#################################################################
## OPENSSL                                                     ##
#################################################################

$(SOURCEDIR)/$(OPENSSLARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(OPENSSLURI) || rm -f $@


$(BUILDDIR)/openssl: $(SOURCEDIR)/$(OPENSSLARCHIVE) $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(OPENSSLVERSION)
	@tar -xzf $(SOURCEDIR)/$(OPENSSLARCHIVE) -C $(BUILDDIR)
	cd $@-$(OPENSSLVERSION)                       && \
	$(BUILDENV)                                      \
		./Configure no-async linux-armv4 -DL_ENDIAN  \
			--prefix=$(PREFIXDIR)                    \
			--openssldir=/tmp/sd/firmware/etc/ssl && \
		make -j$(PROCS) depend                    && \
		make -j$(PROCS)                           && \
		make -j$(PROCS) install
	@rm -rf $@-$(OPENSSLVERSION)
	@touch $@


#################################################################
## SOCAT                                                       ##
#################################################################

$(SOURCEDIR)/$(SOCATARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(SOCATURI) || rm -f $@


$(BUILDDIR)/socat: $(SOURCEDIR)/$(SOCATARCHIVE) $(BUILDDIR)/ncurses $(BUILDDIR)/readline $(BUILDDIR)/openssl
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(SOCATVERSION)
	@tar -xzf $(SOURCEDIR)/$(SOCATARCHIVE) -C $(BUILDDIR)
	cd $@-$(SOCATVERSION)                   && \
	$(BUILDENV)                                \
	CPPFLAGS="-I$(PREFIXDIR)/include -DNETDB_INTERNAL=-1" \
		./configure                            \
			--host=$(TARGET)                   \
			--prefix=$(PREFIXDIR)           && \
		make -j$(PROCS)                     && \
		make -j$(PROCS) install
	@rm -rf $@-$(SOCATVERSION)
	@touch $@


#################################################################
## LOGROTATE                                                   ##
#################################################################

$(SOURCEDIR)/$(LOGROTATEARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LOGROTATEURI) || rm -f $@


$(BUILDDIR)/logrotate: $(SOURCEDIR)/$(LOGROTATEARCHIVE) $(BUILDDIR)/popt
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LOGROTATEVERSION)
	@tar -xzf $(SOURCEDIR)/$(LOGROTATEARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LOGROTATEVERSION)          && \
	$(BUILDENV)                            \
		./configure                        \
			--host=$(TARGET)               \
			--prefix="$(PREFIXDIR)"     && \
		make -j$(PROCS)                 && \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBPCAPVERSION)
	@touch $@



#################################################################
## DROPBEAR                                                    ##
#################################################################

$(SOURCEDIR)/$(SFTPARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(SFTPURI) || rm -f $@


$(SOURCEDIR)/$(DROPBEARARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(DROPBEARURI) || rm -f $@


$(BUILDDIR)/sftp: $(SOURCEDIR)/$(SFTPARCHIVE) $(BUILDDIR)/zlib $(BUILDDIR)/openssl
	mkdir -p $(BUILDDIR) && rm -rf $(BUILDDIR)/openssh-$(SFTPVERSION)
	tar -xzf $(SOURCEDIR)/$(SFTPARCHIVE) -C $(BUILDDIR)
	cd $(BUILDDIR)/openssh-$(SFTPVERSION) && \
	$(BUILDENV)                              \
		./configure                          \
			--host=$(TARGET)                 \
			--prefix=$(PREFIXDIR)            \
			--with-zlib=$(PREFIXDIR)         \
			--sysconfdir=/etc                \
			--enable-shared                  \
			--disable-static              && \
		make -j$(PROCS) sftp-server       && \
		cp sftp-server $(PREFIXDIR)/bin
	rm -rf $(BUILDDIR)/openssh-$(SFTPVERSION)
	touch $@


$(BUILDDIR)/dropbear: $(SOURCEDIR)/$(DROPBEARARCHIVE) $(BUILDDIR)/zlib $(BUILDDIR)/sftp
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(DROPBEARVERSION)
	@tar -xjf $(SOURCEDIR)/$(DROPBEARARCHIVE) -C $(BUILDDIR)
	sed -i 's|\(#define DROPBEAR_PATH_SSH_PROGRAM\).*|\1 "/tmp/sd/ft/dbclient"|' $@-$(DROPBEARVERSION)/default_options.h
	sed -i 's|\(#define SFTPSERVER_PATH\).*|\1 "/tmp/sd/firmware/bin/sftp-server"|' $@-$(DROPBEARVERSION)/default_options.h
	sed -i 's|\(#define DEFAULT_PATH\).*|\1 "/tmp/sd/firmware/bin:/tmp/sd/firmware/scripts:/bin:/sbin:/usr/bin:/usr/sbin:/tmp/sd/ft:/mnt/data/ft"|' $@-$(DROPBEARVERSION)/default_options.h
	@cd $@-$(DROPBEARVERSION)         && \
	$(BUILDENV)                          \
		./configure                      \
			--prefix=$(PREFIXDIR)        \
			--host=$(TARGET)             \
			--with-zlib=$(PREFIXDIR)     \
			--disable-wtmp               \
			--disable-lastlog         && \
		make PROGRAMS="dropbear scp dbclient dropbearkey" MULTI=0 -j$(PROCS)         && \
		make PROGRAMS="dropbear scp dbclient dropbearkey" MULTI=0 -j$(PROCS) install && \
	rm -rf $@-$(DROPBEARVERSION)
	@touch $@


#################################################################
## LIGHTTPD                                                    ##
#################################################################

$(SOURCEDIR)/$(LIGHTTPDARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LIGHTTPDURI) || rm -f $@


$(BUILDDIR)/lighttpd: $(SOURCEDIR)/$(LIGHTTPDARCHIVE) $(BUILDDIR)/zlib $(BUILDDIR)/pcre
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIGHTTPDVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIGHTTPDARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIGHTTPDVERSION)        && \
	$(BUILDENV)                         \
		./configure                     \
			--prefix=$(PREFIXDIR)       \
			--host=$(TARGET)            \
			--with-zlib=$(PREFIXDIR)    \
			--with-pcre=$(PREFIXDIR)    \
			--with-openssl=$(PREFIXDIR) \
			--with-openssl-libs=$(PREFIXDIR)/lib \
			--disable-static            \
			--enable-shared             \
			--without-mysql             \
			--without-bzip2          && \
		make -j$(PROCS)              && \
		make -j$(PROCS) install
	@rm -rf $@-$(LIGHTTPDVERSION)
	@touch $@


#################################################################
## PHP                                                         ##
#################################################################

$(SOURCEDIR)/$(PHPARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(PHPURI) || rm -f $@


$(BUILDDIR)/php: $(SOURCEDIR)/$(PHPARCHIVE) $(BUILDDIR)/zlib $(BUILDDIR)/libxml2 $(BUILDDIR)/libjpeg-turbo $(BUILDDIR)/libpng $(BUILDDIR)/pcre $(BUILDDIR)/libgd
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(PHPVERSION)
	@tar -xjf $(SOURCEDIR)/$(PHPARCHIVE) -C $(BUILDDIR)
	@sed -i -e '/.*hp_ini_register_extensions.*/d' $@-$(PHPVERSION)/main/main.c
	@cd $@-$(PHPVERSION)             && \
	$(BUILDENV)                         \
	LIBS='-ldl'                         \
		./configure                     \
			--prefix=$(PREFIXDIR)       \
			--host=$(TARGET)            \
			--target=$(TARGET)          \
			--program-prefix="arm-"        \
			--with-config-file-path=/etc   \
			--with-libxml-dir=$(PREFIXDIR) \
			--with-jpeg-dir=$(PREFIXDIR)   \
			--with-png-dir=$(PREFIXDIR)    \
			--with-zlib-dir=$(PREFIXDIR)   \
			--with-mhash                \
			--with-pdo-mysql            \
			--with-sqlite3              \
			--with-pdo-sqlite           \
			--with-xmlrpc               \
			--with-zlib                 \
			--with-pcre-regex           \
			--with-pcre-jit             \
			--with-gd                   \
			--with-xpm-dir=no           \
			--without-pear              \
			--without-xsl               \
			--enable-pdo                \
			--enable-simplexml          \
			--enable-json               \
			--enable-sockets            \
			--enable-fpm                \
			--enable-libxml             \
			--enable-ftp                \
			--enable-mbstring           \
			--enable-mbregex            \
			--enable-mbregex-backtrack  \
			--enable-hash               \
			--enable-xml                \
			--enable-session            \
			--enable-soap               \
			--enable-tokenizer          \
			--enable-xmlreader          \
			--enable-xmlwriter          \
			--enable-dom                \
			--enable-zip                \
			--disable-mbregex           \
			--disable-opcache           \
			--disable-all            && \
		make -j$(PROCS)              && \
		make -j$(PROCS) install-binaries
	@rm -rf $@-$(PHPVERSION)
	@touch $@


#################################################################
## VIM                                                         ##
#################################################################

$(SOURCEDIR)/$(VIMARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(VIMURI) || rm -f $@


$(BUILDDIR)/vim: $(SOURCEDIR)/$(VIMARCHIVE) $(BUILDDIR)/ncurses $(BUILDDIR)/readline $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(VIMVERSION)
	@tar -xzf $(SOURCEDIR)/$(VIMARCHIVE) -C $(BUILDDIR)
	@cd $@-$(VIMVERSION)                                  && \
	export vim_cv_tgetent=zero                            && \
	export vim_cv_toupper_broken="set"                    && \
	export vim_cv_terminfo="yes"                          && \
	export vim_cv_getcwd_broken="yes"                     && \
	export vim_cv_tty_group=0                             && \
	export vim_cv_tty_mode="0750"                         && \
	export vim_cv_stat_ignores_slash="yes"                && \
	export vim_cv_memmove_handles_overlap="yes"           && \
	$(BUILDENV)                                              \
	CC="$(TARGET)-gcc"                                       \
		./configure                                          \
			--host=$(TARGET)                                 \
			--prefix=$(PREFIXDIR)                            \
			--enable-multibyte                               \
			--enable-gui=no                                  \
			--disable-gpm                                    \
			--disable-gtktest                                \
			--disable-xim                                    \
			--disable-pythoninterp                           \
			--disable-python3interp                          \
			--disable-rubyinterp                             \
			--disable-luainterp                              \
			--disable-netbeans                               \
			--with-features=normal                           \
			--with-tlib=ncurses                              \
			--without-x                                   && \
		make VIMRCLOC=/etc VIMRUNTIMEDIR=/tmp/sd/firmware/share/vim CC=$(TARGET)-gcc -j$(PROCS) && \
		make -j$(PROCS) install
	@rm -rf $@-$(VIMVERSION)
	@touch $@


#################################################################
## NANO                                                        ##
#################################################################

$(SOURCEDIR)/$(NANOARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(NANOURI) || rm -f $@


$(BUILDDIR)/nano: $(SOURCEDIR)/$(NANOARCHIVE) $(BUILDDIR)/ncurses
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(NANOVERSION)
	@tar -xzf $(SOURCEDIR)/$(NANOARCHIVE) -C $(BUILDDIR)
	@cd $@-$(NANOVERSION)                                 && \
	$(BUILDENV)                                              \
	CFLAGS="-O2 -Wall"                                       \
	CPPFLAGS="-P -I$(PREFIXDIR)/include -I$(PREFIXDIR)/include/ncurses -L$(PREFIXDIR)/lib/" \
		./configure                                          \
			--host=$(TARGET)                                 \
			--prefix=$(PREFIXDIR)                            \
			--disable-mouse                                  \
			--disable-browser                                \
			--disable-nls                                    \
			--disable-dependency-tracking                 && \
		make -j$(PROCS)                                   && \
		make -j$(PROCS) install
	@rm -rf $@-$(NANOVERSION)
	@touch $@


#################################################################
## RSYNC                                                       ##
#################################################################

$(SOURCEDIR)/$(RSYNCARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(RSYNCURI) || rm -f $@


$(BUILDDIR)/rsync: $(PREFIXDIR)/bin $(SOURCEDIR)/$(RSYNCARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(RSYNCVERSION)
	@tar -xzf $(SOURCEDIR)/$(RSYNCARCHIVE) -C $(BUILDDIR)
	@cd $@-$(RSYNCVERSION)                                && \
	$(BUILDENV)                                              \
		./configure                                          \
			--host=$(TARGET)                                 \
			--target=$(TARGET)                            && \
		make -j$(PROCS)                                   && \
	cp rsync $(PREFIXDIR)/bin/rsync
	@rm -rf $(BUILDDIR)/$@-$(RSYNCVERSION)
	@touch $@


#################################################################
## STRACE                                                      ##
#################################################################

$(SOURCEDIR)/$(STRACEARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(STRACEURI) || rm -f $@


$(BUILDDIR)/strace: $(SOURCEDIR)/$(STRACEARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(STRACEVERSION)
	@tar -xf $(SOURCEDIR)/$(STRACEARCHIVE) -C $(BUILDDIR)
	@cd $@-$(STRACEVERSION)                            && \
	patch -p2 < $(PATCHESDIR)/strace.patch             && \
	$(BUILDENV)                                           \
		./configure                                       \
			--prefix=$(PREFIXDIR)                         \
			--host=$(TARGET)                           && \
		make -j$(PROCS)                                && \
		make -j$(PROCS) install
	@rm -rf $@-$(STRACEVERSION)
	@touch $@


#################################################################
## LSOF                                                        ##
#################################################################

$(SOURCEDIR)/$(LSOFARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LSOFURI) || rm -f $@


$(BUILDDIR)/lsof: $(PREFIXDIR)/bin $(SOURCEDIR)/$(LSOFARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LSOFVERSION)
	@tar -xzf $(SOURCEDIR)/$(LSOFARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LSOFVERSION)                              && \
		export LSOF_CC="$(TOOLCHAINDIR)/$(TARGET)-gcc" && \
		export LSOF_ARCH="$(TARGET)"                   && \
		$(BUILDENV)                                       \
		./Configure -n linux                           && \
		make -j$(PROCS)                                && \
		cp lsof $(PREFIXDIR)/bin/lsof
	@rm -rf $@-$(LSOFVERSION)
	@touch $@


#################################################################
## FFMPEG                                                      ##
#################################################################

$(SOURCEDIR)/$(FFMPEGARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(FFMPEGURI) || rm -f $@


$(BUILDDIR)/ffmpeg: $(SOURCEDIR)/$(FFMPEGARCHIVE) $(BUILDDIR)/x264 $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(FFMPEGVERSION)
	@tar -xjf $(SOURCEDIR)/$(FFMPEGARCHIVE) -C $(BUILDDIR)
	@cd $@-$(FFMPEGVERSION)                          && \
		$(BUILDENV)                                     \
		./configure                                     \
			--extra-cxxflags="-L$(PREFIXDIR)/lib -I$(PREFIXDIR)/include"   \
			--extra-ldflags=" -L$(PREFIXDIR)/lib -I$(PREFIXDIR)/include -Wl,-rpath,/tmp/sd/firmware/lib" \
			--arch=arm                                  \
			--target-os=linux                           \
			--cross-prefix=$(TARGET)-                   \
			--prefix=$(PREFIXDIR)                       \
			--disable-ffplay                            \
			--disable-doc                               \
			--disable-w32threads                        \
			--disable-static                            \
			--enable-shared                             \
			--enable-avcodec                            \
			--enable-avformat                           \
			--enable-avfilter                           \
			--enable-swresample                         \
			--enable-swscale                            \
			--enable-ffmpeg                             \
			--enable-filters                            \
			--enable-gpl                                \
			--enable-iconv                              \
			--enable-libx264                            \
			--enable-nonfree                            \
			--enable-pthreads                           \
			--enable-runtime-cpudetect                  \
			--enable-small                              \
			--enable-version3                           \
			--enable-zlib                            && \
		make -j$(PROCS)                              && \
		make -j$(PROCS) install
	@rm -rf $(BUILDDIR)/$@-$(FFMPEGVERSION)
	@touch $@


#################################################################
## RTSPD                                                       ##
#################################################################

$(BUILDDIR)/rtspd: $(PREFIXDIR)/bin
	@mkdir -p $(BUILDDIR) $(TOOLSDIR)/bin
	cd $(RTSPDDIR) && \
	$(TARGET)-gcc \
		-DLOG_USE_COLOR \
		-Wall \
		-I$(GMLIBDIR)/inc \
		$(RTSPDDIR)/$(@F).c \
		$(RTSPDDIR)/log/log.c \
		$(RTSPDDIR)/librtsp.a \
		-L$(GMLIBDIR)/lib \
		-lpthread -lm -lrt -lgm -o $(TOOLSDIR)/bin/rtspd && \
		$(TARGET)-strip $(TOOLSDIR)/bin/rtspd
	@touch $@


#################################################################
## UTILS                                                       ##
#################################################################

$(LIBS):
	@mkdir -p $(BUILDDIR)
	$(TARGET)-gcc -shared -o $(TOOLSDIR)/lib/lib$(@F).so -fPIC $(TOOLSDIR)/lib/$(@F).c && \
	touch $(BUILDDIR)/$(@F)


$(UTILS): $(PREFIXDIR)/bin $(LIBS)
	@mkdir -p $(BUILDDIR)
	CPPFLAGS="-I$(TOOLSDIR)/lib -L$(TOOLSDIR)/lib" \
	LDFLAGS=" -I$(TOOLSDIR)/lib -L$(TOOLSDIR)/lib -Wl,-rpath -Wl,/tmp/sd/firmware/lib -Wl,--enable-new-dtags" \
	$(TARGET)-gcc \
		-Wall \
		-o $(TOOLSDIR)/bin/$(@F) $(UTILSDIR)/$(@F).c \
		-I$(TOOLSDIR)/lib -L$(TOOLSDIR)/lib \
		-lchuangmi_ircut \
		-lchuangmi_utils \
		-lchuangmi_isp328 \
		-lchuangmi_pwm && \
	touch $(BUILDDIR)/$(@F)


sdcard/manufacture.bin:
	tar -cf $(TOPDIR)/sdcard/manufacture.bin manufacture/test_drv


#################################################################
##                                                             ##
#################################################################

.PHONY: install images uninstall clean

install: all
	@mkdir -p $(BINARIESDIR) \
	\
	&& echo "*** Copying third party binaries and extras to $(BINARIESDIR)" \
	&& cd $(PREFIXDIR)/sbin && $(TARGET)-strip $(THIRD_PARTY_SBINS) && cp $(THIRD_PARTY_SBINS) $(BINARIESDIR) \
	&& cd $(PREFIXDIR)/bin  && $(TARGET)-strip $(THIRD_PARTY_BINS)  && cp $(THIRD_PARTY_BINS)  $(BINARIESDIR) \
	&& cd $(PREFIXDIR)/bin  && cp $(THIRD_PARTY_BIN_EXTRAS) $(BINARIESDIR) \
	\
	&& echo "*** Copying third party libraries and extras to $(LIBRARIESDIR)" \
	&& cd $(PREFIXDIR)/lib  && $(TARGET)-strip $(THIRD_PARTY_LIBS) && cp $(THIRD_PARTY_LIBS) $(LIBRARIESDIR) \
	&& cd $(PREFIXDIR)/lib  && cp $(THIRD_PARTY_LIB_EXTRAS) $(LIBRARIESDIR) \
	\
	&& echo "*** Copying our own binaries to $(BINARIESDIR)" \
	&& cd $(TOOLSDIR)/bin   && find . -maxdepth 1 -type f -exec $(TARGET)-strip {} \; -exec cp {} $(BINARIESDIR) \; \
	\
	&& echo "*** Copying our own libraries to $(BINARIESDIR)" \
	&& cd $(TOOLSDIR)/lib   && find . -maxdepth 1 -type f -name '*.so' -exec $(TARGET)-strip {} \; -exec cp {} $(LIBRARIESDIR) \;


images: all website install
	@cd $(TOPDIR) \
	\
	&& echo "*** Removing older version of MiiCam.tgz and MiiCam.zip" \
	&& find $(TOPDIR) -maxdepth 1 -type f -name 'MiiCam.zip' -or -name 'MiiCam.tgz' -delete \
	\
	&& echo "*** Creating file listing before packing" \
	&& find $(TOPDIR)/sdcard -type f -exec md5sum {} \; > sdcard/firmware/etc/files.db \
	\
	&& echo "*** Creating zip archive of sdcard/ to MiiCam.zip" \
	&& zip -r --quiet MiiCam.zip README.md sdcard \
	\
	&& echo "*** Creating tar archive of sdcard/ to MiiCam.tgz" \
	&& tar czf MiiCam.tgz -C $(TOPDIR) README.md sdcard \
	\
	&& echo "*** MiiCam.zip and MiiCam.tgz created"


clean:
	@cd $(TOPDIR) \
	\
	&& echo "*** Removing directories with build artifacts" \
	&& rm -rf $(BINARIESDIR) $(LIBRARIESDIR) $(SOURCEDIR) $(PREFIXDIR) $(BUILDDIR) \
	\
	&& echo "Removing all own-brewed binaries" \
	&& find $(TOOLSDIR)/bin -maxdepth 1 -type f -delete \
	\
	&& echo "Removing all self-brewed libraries" \
	&& find $(TOOLSDIR)/lib -maxdepth 1 -type f -name '*.so' -delete

