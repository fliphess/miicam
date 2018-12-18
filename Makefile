
TOOLCHAINDIR = /usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin
PATH  := $(TOOLCHAINDIR):$(PATH)
TARGET = arm-unknown-linux-uclibcgnueabi
PROCS = 7

BUILDENV :=                 \
	AR=$(TARGET)-ar         \
	AS=$(TARGET)-as         \
	CC=$(TARGET)-gcc        \
	CXX=$(TARGET)-g++       \
	LD=${TARGET}-ld         \
	NM=$(TARGET)-nm         \
	RANLIB=$(TARGET)-ranlib \
	STRIP=$(TARGET)-strip

TOPDIR       := $(CURDIR)
SOURCEDIR    := $(TOPDIR)/src
PREFIXDIR    := $(TOPDIR)/prefix
BUILDDIR     := $(TOPDIR)/build

GMLIBDIR     := $(TOPDIR)/tools/gm_lib
GMSAMPLES    := $(TOPDIR)/tools/samples

RTSPDDIR     := $(TOPDIR)/tools/rtspd
UTILSDIR     := $(TOPDIR)/tools/utils
PATCHESDIR   := $(TOPDIR)/tools/patches

BINARIESDIR  := $(TOPDIR)/sdcard/firmware/bin
LIBRARIESDIR := $(TOPDIR)/sdcard/firmware/lib
WEBROOT      := $(TOPDIR)/sdcard/firmware/www
COMPOSER     := /usr/local/bin/composer

BINS =                                     \
	smbpasswd smbstatus smbtree smbclient  \
	scp dbclient dropbearkey               \
	arm-php arm-php-cgi                    \
	ffmpeg ffprobe                         \
	openssl                                \
	procan  socat filan                    \
	lsof                                   \
	nano                                   \
	runas                                  \
	rsync                                  \
	rtspd                                  \
	strace

SBINS =             \
	dropbear        \
	lighttpd        \
	tcpdump         \
	nmbd            \
	smbd


include SOURCES.mk

SAMPLES :=                                          \
	tools/audio_livesound                           \
	tools/audio_playback                            \
	tools/audio_record                              \
	tools/display_with_encode                       \
	tools/encode_and_liveview_with_notification     \
	tools/encode_capture_substream                  \
	tools/encode_force_keyframe                     \
	tools/encode_scaler_substream                   \
	tools/encode_update_notification                \
	tools/encode_with_advance_feature               \
	tools/encode_with_av_sync                       \
	tools/encode_with_capture_motion_detection      \
	tools/encode_with_capture_motion_detection2     \
	tools/encode_with_capture_tamper_detection      \
	tools/encode_with_capture_tamper_detection2     \
	tools/encode_with_deinterlace                   \
	tools/encode_with_eptz                          \
	tools/encode_with_getraw                        \
	tools/encode_with_getraw2                       \
	tools/encode_with_osd                           \
	tools/encode_with_roi                           \
	tools/encode_with_snapshot                      \
	tools/encode_with_watermark_and_vui             \
	tools/liveview_with_clearwin                    \
	tools/liveview_with_pip


UTILS :=                                            \
	tools/chuangmi_ctrl                             \
	tools/take_snapshot


samples: $(SAMPLES)

utils: $(UTILS)


all:                                 \
	$(PREFIXDIR)/bin                 \
	$(BUILDDIR)/zlib                 \
	$(BUILDDIR)/libxml2              \
	$(BUILDDIR)/libjpeg-turbo        \
	$(BUILDDIR)/libpng               \
	$(BUILDDIR)/libgd                \
	$(BUILDDIR)/pcre                 \
	$(BUILDDIR)/x264                 \
	$(BUILDDIR)/ncurses              \
	$(BUILDDIR)/readline             \
	$(BUILDDIR)/libpcap              \
	$(BUILDDIR)/tcpdump              \
	$(BUILDDIR)/openssl              \
	$(BUILDDIR)/socat                \
	$(BUILDDIR)/dropbear             \
	$(BUILDDIR)/lighttpd             \
	$(BUILDDIR)/nano                 \
	$(BUILDDIR)/php                  \
	$(BUILDDIR)/samba                \
	$(BUILDDIR)/runas                \
	$(BUILDDIR)/rsync                \
	$(BUILDDIR)/lsof                 \
	$(BUILDDIR)/strace               \
	$(BUILDDIR)/ffmpeg               \
	$(BUILDDIR)/rtspd                \
	utils                            \
	samples                          \
	sdcard/manufacture.bin


$(PREFIXDIR)/bin:
	@mkdir -p $(PREFIXDIR)/bin


$(BUILDDIR)/zlib: $(SOURCEDIR)/$(ZLIBARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(ZLIBVERSION)
	@tar -xzf $(SOURCEDIR)/$(ZLIBARCHIVE) -C $(BUILDDIR)
	@cd $@-$(ZLIBVERSION)             && \
		$(BUILDENV)                      \
		./configure                      \
			--prefix=$(PREFIXDIR)        \
			--static                  && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	@rm -rf $@-$(ZLIBVERSION)
	@touch $@


$(BUILDDIR)/libxml2: $(SOURCEDIR)/$(LIBXML2ARCHIVE) $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBXML2VERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBXML2ARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBXML2VERSION)          && \
		$(BUILDENV)                      \
		ARCH=arm                         \
		Z_CFLAGS="-DHAVE_ZLIB_H=1 -DHAVE_LIBZ=1 -I$(PREFIXDIR)/include" \
		./configure                      \
			--prefix=$(PREFIXDIR)        \
			--host=$(TARGET)             \
			--disable-shared             \
			--enable-static              \
			--with-zlib=$(PREFIXDIR)     \
			--without-python             \
			--without-iconv              \
			--without-lzma            && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBXML2VERSION)
	@touch $@


$(BUILDDIR)/libjpeg-turbo: $(SOURCEDIR)/$(LIBJPEGARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBJPEGVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBJPEGARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBJPEGVERSION)          && \
		$(BUILDENV)                      \
		./configure                      \
			--prefix=$(PREFIXDIR)        \
			--host=$(TARGET)             \
			--disable-shared             \
			--enable-static           && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBJPEGVERSION)
	@touch $@


$(BUILDDIR)/libpng: $(SOURCEDIR)/$(LIBPNGARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBPNGVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBPNGARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBPNGVERSION)            && \
		$(BUILDENV)                       \
		LDFLAGS="-L$(PREFIXDIR)/lib"      \
		CPPFLAGS="-I$(PREFIXDIR)/include" \
		./configure                       \
			--prefix=$(PREFIXDIR)         \
			--host=$(TARGET)              \
			--disable-shared              \
			--enable-static            && \
		make -j$(PROCS)                && \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBPNGVERSION)
	@touch $@


$(BUILDDIR)/libgd: $(SOURCEDIR)/$(LIBGDARCHIVE) $(BUILDDIR)/zlib $(BUILDDIR)/libjpeg-turbo $(BUILDDIR)/libpng
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBGDVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBGDARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBGDVERSION)            && \
		$(BUILDENV)                      \
		ARCH=arm                         \
		./configure                      \
			--prefix=$(PREFIXDIR)        \
			--host=$(TARGET)             \
			--disable-shared             \
			--enable-static              \
			--with-jpeg=$(PREFIXDIR)     \
			--with-png=$(PREFIXDIR)      \
			--with-zlib=$(PREFIXDIR)     \
			--without-tiff               \
			--without-freetype           \
			--without-fontconfig      && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBGDVERSION)
	@touch $@


$(BUILDDIR)/pcre: $(SOURCEDIR)/$(PCREARCHIVE) $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(PCREVERSION)
	@unzip -q $(SOURCEDIR)/$(PCREARCHIVE) -d $(BUILDDIR)
	@cd $@-$(PCREVERSION)             && \
		$(BUILDENV)                      \
		./configure                      \
			--host=$(TARGET)             \
			--prefix=$(PREFIXDIR)        \
			--disable-shared             \
			--enable-static           && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	@rm -rf $@-$(PCREVERSION)
	@touch $@


$(BUILDDIR)/x264: $(SOURCEDIR)/$(X264ARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(X264VERSION)
	@tar -xjf $(SOURCEDIR)/$(X264ARCHIVE) -C $(BUILDDIR)
	@cd $@-$(X264VERSION)             && \
	$(BUILDENV)                          \
		./configure                      \
			--host=$(TARGET)             \
			--cross-prefix=$(TARGET)-    \
			--prefix="$(PREFIXDIR)"      \
			--enable-static              \
			--enable-pic                 \
			--disable-asm             && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	@rm -rf $@-$(X264VERSION)
	@touch $@


$(BUILDDIR)/ncurses: $(SOURCEDIR)/$(NCURSESARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(NCURSESVERSION)
	@tar -xzf $(SOURCEDIR)/$(NCURSESARCHIVE) -C $(BUILDDIR)
	@cd $@-$(NCURSESVERSION)                && \
	$(BUILDENV)                                \
	CC='$(TOOLCHAINDIR)/$(TARGET)-gcc -static' \
	CFLAGS='-fPIC'                             \
		./configure                            \
			--host=$(TARGET)                   \
			--prefix=$(PREFIXDIR)              \
			--disable-shared                   \
			--enable-static                    \
			--with-normal                      \
			--without-debug                    \
			--without-ada                      \
			--with-default-terminfo=/usr/share/terminfo \
			--with-terminfo-dirs="/etc/terminfo:/lib/terminfo:/usr/share/terminfo:/usr/lib/terminfo" && \
		make -j$(PROCS)                     && \
		make -j$(PROCS) install.libs install.includes
	@rm -rf $@-$(NCURSESVERSION)
	@touch $@


$(BUILDDIR)/readline: $(SOURCEDIR)/$(READLINEARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(READLINEVERSION)
	@tar -xzf $(SOURCEDIR)/$(READLINEARCHIVE) -C $(BUILDDIR)
	@cd $@-$(READLINEVERSION)               && \
	$(BUILDENV)                                \
	CC='$(TOOLCHAINDIR)/$(TARGET)-gcc -static' \
	CFLAGS='-fPIC'                             \
		./configure                            \
			--host=$(TARGET)                   \
			--prefix=$(PREFIXDIR)              \
			--disable-shared                   \
			--enable-static                 && \
		make -j$(PROCS)                     && \
		make -j$(PROCS) install-static install-headers
	@rm -rf $@-$(READLINEVERSION)
	@touch $@

$(BUILDDIR)/libpcap: $(SOURCEDIR)/$(LIBPCAPARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBPCAPVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBPCAPARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBPCAPVERSION)            && \
	$(BUILDENV)                            \
		CFLAGS='-I$(PREFIXDIR)'            \
		./configure                        \
			--prefix="$(PREFIXDIR)"        \
			--disable-canusb               \
			--host=$(TARGET)               \
			--with-pcap=linux              \
			ac_cv_type_u_int64_t=yes    && \
		make -j$(PROCS)                 && \
		make -j$(PROCS) install
	@rm -rf $@-$(LIBPCAPVERSION)
	@touch $@


$(BUILDDIR)/tcpdump: $(BUILDDIR)/libpcap $(SOURCEDIR)/$(TCPDUMPARCHIVE) $(BUILDDIR)/openssl
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(TCPDUMPVERSION)
	@tar -xzf $(SOURCEDIR)/$(TCPDUMPARCHIVE) -C $(BUILDDIR)
	@cd $@-$(TCPDUMPVERSION)                  && \
	export CROSS_COMPILE="$(TARGET)-"         && \
	export CFLAGS="-static -L$(PREFIXDIR)/lib -I$(PREFIXDIR)/include -I$(PREFIXDIR)/include/pcap"  && \
	export CPPFLAGS="${CFLAGS}"               && \
	export LDFLAGS="${CFLAGS}"                && \
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


$(BUILDDIR)/openssl: $(SOURCEDIR)/$(OPENSSLARCHIVE) $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(OPENSSLVERSION)
	@tar -xzf $(SOURCEDIR)/$(OPENSSLARCHIVE) -C $(BUILDDIR)
	cd $@-$(OPENSSLVERSION)                   && \
	$(BUILDENV)                                  \
	CC='$(TOOLCHAINDIR)/$(TARGET)-gcc -static'   \
	LDFLAGS="-L$(PREFIXDIR)/lib"                 \
	CPPFLAGS="-I$(PREFIXDIR)/include"            \
		./Configure no-shared no-async linux-armv4 -DL_ENDIAN \
			--prefix=$(PREFIXDIR)                \
			--openssldir=$(PREFIXDIR)         && \
		make -j$(PROCS) depend                && \
		make -j$(PROCS)                       && \
		make -j$(PROCS) install
	@rm -rf $@-$(OPENSSLVERSION)
	@touch $@


$(BUILDDIR)/socat: $(SOURCEDIR)/$(SOCATARCHIVE) $(BUILDDIR)/ncurses $(BUILDDIR)/readline $(BUILDDIR)/openssl
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(SOCATVERSION)
	@tar -xzf $(SOURCEDIR)/$(SOCATARCHIVE) -C $(BUILDDIR)
	cd $@-$(SOCATVERSION)                   && \
	$(BUILDENV)                                \
	CC="$(TOOLCHAINDIR)/$(TARGET)-gcc -static" \
	CFLAGS="-fPIC"                             \
	CPPFLAGS="-I$(PREFIXDIR)/include -DNETDB_INTERNAL=-1" \
	LDFLAGS="-L$(PREFIXDIR)/lib"               \
		./configure                            \
			--host=$(TARGET)                   \
			--prefix=$(PREFIXDIR)           && \
		make -j$(PROCS)                     && \
		make -j$(PROCS) install
	@rm -rf $@-$(SOCATVERSION)
	@touch $@


$(BUILDDIR)/dropbear: $(SOURCEDIR)/$(DROPBEARARCHIVE) $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(DROPBEARVERSION)
	@tar -xjf $(SOURCEDIR)/$(DROPBEARARCHIVE) -C $(BUILDDIR)
	@sed -i 's|\(#define DROPBEAR_PATH_SSH_PROGRAM\).*|\1 "/tmp/sd/ft/dbclient"|' $@-$(DROPBEARVERSION)/options.h
	@sed -i 's|\(#define DEFAULT_PATH\).*|\1 "/bin:/sbin:/usr/bin:/usr/sbin:/tmp/sd/firmware/bin:/tmp/sd/ft:/mnt/data/ft"|' $@-$(DROPBEARVERSION)/options.h
	@cd $@-$(DROPBEARVERSION)         && \
		$(BUILDENV)                      \
		./configure                      \
			--prefix=$(PREFIXDIR)        \
			--host=$(TARGET)             \
			--with-zlib=$(PREFIXDIR)     \
			--disable-wtmp               \
			--disable-lastlog         && \
		make PROGRAMS="dropbear scp dbclient dropbearkey" MULTI=0 STATIC=1 -j$(PROCS)         && \
		make PROGRAMS="dropbear scp dbclient dropbearkey" MULTI=0 STATIC=1 -j$(PROCS) install && \
	rm -rf $@-$(DROPBEARVERSION)
	@touch $@


$(BUILDDIR)/lighttpd: $(SOURCEDIR)/$(LIGHTTPDARCHIVE) $(BUILDDIR)/zlib $(BUILDDIR)/pcre
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIGHTTPDVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIGHTTPDARCHIVE) -C $(BUILDDIR)
	@for i in access                    \
		accesslog                       \
		alias                           \
		auth                            \
		authn_file                      \
		cgi                             \
		compress                        \
		deflate                         \
		dirlisting                      \
		evasive                         \
		expire                          \
		extforward                      \
		fastcgi                         \
		indexfile                       \
		proxy                           \
		redirect                        \
		rewrite                         \
		scgi                            \
		secdownload                     \
		setenv                          \
		simple_vhost                    \
		ssi                             \
		staticfile                      \
		status                          \
		uploadprogress                  \
		usertrack;                      \
	do                                  \
		echo "PLUGIN_INIT(mod_$$i)" >> $@-$(LIGHTTPDVERSION)/src/plugin-static.h; \
	done                             && \
	cd $@-$(LIGHTTPDVERSION)         && \
		$(BUILDENV)                     \
		LIGHTTPD_STATIC=yes             \
		CPPFLAGS=-DLIGHTTPD_STATIC      \
		./configure                     \
			--prefix=$(PREFIXDIR)       \
			--host=$(TARGET)            \
			--disable-shared            \
			--enable-static             \
			--with-zlib=$(PREFIXDIR)    \
			--with-pcre=$(PREFIXDIR)    \
			--with-openssl=$(PREFIXDIR) \
			--with-openssl-libs=$(PREFIXDIR)/lib \
			--without-mysql             \
			--without-bzip2          && \
		make -j$(PROCS)              && \
		make -j$(PROCS) install
	@rm -rf $@-$(LIGHTTPDVERSION)
	@touch $@


$(BUILDDIR)/php: $(SOURCEDIR)/$(PHPARCHIVE) $(BUILDDIR)/zlib $(BUILDDIR)/libxml2 $(BUILDDIR)/libjpeg-turbo $(BUILDDIR)/libpng $(BUILDDIR)/pcre $(BUILDDIR)/libgd
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(PHPVERSION)
	@tar -xjf $(SOURCEDIR)/$(PHPARCHIVE) -C $(BUILDDIR)
	@sed -i -e '/.*hp_ini_register_extensions.*/d' $@-$(PHPVERSION)/main/main.c
	@cd $@-$(PHPVERSION)             && \
		$(BUILDENV)                     \
		LIBS='-ldl'                     \
		./configure                     \
			--prefix=$(PREFIXDIR)       \
			--host=$(TARGET)            \
			--target=$(TARGET)          \
			--program-prefix="arm-"     \
			--with-config-file-path=$(PREFIXDIR)/etc \
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


$(BUILDDIR)/samba: $(SOURCEDIR)/$(SAMBAARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(SAMBAVERSION)
	@tar -xzf $(SOURCEDIR)/$(SAMBAARCHIVE) -C $(BUILDDIR)
	@cd $@-$(SAMBAVERSION)/source3   && \
		$(BUILDENV)                     \
		./autogen.sh                 && \
		./configure                     \
			--prefix=$(PREFIXDIR)       \
			--host=$(TARGET)            \
			--target=$(TARGET)          \
			--disable-shared            \
			--enable-static             \
			--enable-swat=no            \
			--enable-shared-libs=no     \
			--disable-cups              \
			--with-configdir=/tmp/sd/firmware/etc           \
			--with-nmbdsocketdir=/tmp/sd/firmware/tmp/samba \
			--with-winbind=no                               \
			--with-sys-quotas=no                            \
			--without-krb5                                  \
			--without-ldap                                  \
			--without-ads                                   \
			libreplace_cv_HAVE_GETADDRINFO=no               \
			ac_cv_file__proc_sys_kernel_core_pattern=yes    \
			samba_cv_USE_SETRESUID=yes                      \
			samba_cv_CC_NEGATIVE_ENUM_VALUES=yes         && \
		for i in smbd nmbd smbpasswd;                       \
		do                                                  \
			make -j$(PROCS) bin/$$i;                        \
		done                                             && \
		make -j$(PROCS) installservers                   && \
		make -j$(PROCS) installbin
	@rm -rf $@-$(SAMBAVERSION)
	@touch $@


$(BUILDDIR)/runas: $(PREFIXDIR)/bin $(SOURCEDIR)/$(RUNASARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $(BUILDDIR)/static-sudo-$(RUNASVERSION)
	@unzip $(SOURCEDIR)/$(RUNASARCHIVE) -d $(BUILDDIR)
	@cd $(BUILDDIR)/static-sudo-$(RUNASVERSION)                                 && \
	$(TARGET)-gcc -static -W -Wall -Wextra -Werror -pedantic rpzsudo.c -o runas && \
	cp runas $(PREFIXDIR)/bin/runas
	@rm -rf $(BUILDDIR)/static-sudo-$(RUNASVERSION)
	@touch $@


$(BUILDDIR)/nano: $(SOURCEDIR)/$(NANOARCHIVE) $(BUILDDIR)/ncurses
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(NANOVERSION)
	@tar -xzf $(SOURCEDIR)/$(NANOARCHIVE) -C $(BUILDDIR)
	@cd $@-$(NANOVERSION)                                 && \
	$(BUILDENV)                                              \
	CFLAGS="-O2 -Wall -static"                               \
	CPPFLAGS="-P -I$(PREFIXDIR)/include -I $(PREFIXDIR)/include/ncurses -L$(PREFIXDIR)/lib/" \
	LDFLAGS="-L$(PREFIXDIR)/lib/"                            \
		./configure                                          \
			--host=$(TARGET)                                 \
			--prefix=$(PREFIXDIR)                            \
			--disable-mouse                                  \
			--disable-browser                                \
			--disable-nls                                    \
			--disable-dependency-tracking                    \
			--enable-static                               && \
		make -j$(PROCS)                                   && \
		make -j$(PROCS) install
	@rm -rf $@-$(NANOVERSION)
	@touch $@


$(BUILDDIR)/rsync: $(PREFIXDIR)/bin $(SOURCEDIR)/$(RSYNCARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(RSYNCVERSION)
	@tar -xzf $(SOURCEDIR)/$(RSYNCARCHIVE) -C $(BUILDDIR)
	@cd $@-$(RSYNCVERSION)                                && \
		$(BUILDENV)                                          \
		./configure CFLAGS="-static" EXEEXT="-static"        \
		--host=$(TARGET)                                     \
		--target=$(TARGET)                                && \
		make -j$(PROCS)                                   && \
	cp rsync $(PREFIXDIR)/bin/rsync
	@rm -rf $(BUILDDIR)/$@-$(RSYNCVERSION)
	@touch $@


$(BUILDDIR)/strace: $(SOURCEDIR)/$(STRACEARCHIVE)
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(STRACEVERSION)
	@tar -xf $(SOURCEDIR)/$(STRACEARCHIVE) -C $(BUILDDIR)
	@cd $@-$(STRACEVERSION)                            && \
		patch -p2 < $(PATCHESDIR)/strace.patch         && \
		$(BUILDENV)                                       \
		LDFLAGS="-L$(PREFIXDIR)/lib"                      \
		CPPFLAGS="-I$(PREFIXDIR)/include"                 \
		./configure                                       \
			--prefix=$(PREFIXDIR)                         \
			--host=$(TARGET)                           && \
		make -j$(PROCS)                                && \
		make -j$(PROCS) install
	@rm -rf $@-$(STRACEVERSION)
	@touch $@


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


$(BUILDDIR)/ffmpeg: $(SOURCEDIR)/$(FFMPEGARCHIVE) $(BUILDDIR)/x264 $(BUILDDIR)/zlib
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(FFMPEGVERSION)
	@tar -xjf $(SOURCEDIR)/$(FFMPEGARCHIVE) -C $(BUILDDIR)
	@cd $@-$(FFMPEGVERSION)                          && \
		$(BUILDENV)                                     \
		./configure                                     \
			--pkg-config-flags="--static"               \
			--extra-cflags=-I$(PREFIXDIR)/include       \
			--extra-cxxflags="-I$(PREFIXDIR)"           \
			--extra-ldflags=-L$(PREFIXDIR)/lib          \
			--arch=arm                                  \
			--target-os=linux                           \
			--cross-prefix=$(TARGET)-                   \
			--prefix=$(PREFIXDIR)                       \
			--disable-shared                            \
			--disable-ffplay                            \
			--disable-doc                               \
			--disable-w32threads                        \
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
			--enable-static                             \
			--enable-version3                           \
			--enable-zlib                            && \
		make -j$(PROCS)                              && \
		make -j$(PROCS) install
	@rm -rf $(BUILDDIR)/$@-$(FFMPEGVERSION)
	@touch $@


$(BUILDDIR)/rtspd: $(PREFIXDIR)/bin
	@mkdir -p $(BUILDDIR)
	@cd $(RTSPDDIR)                                  && \
	$(TARGET)-gcc                                       \
		-DLOG_USE_COLOR                                 \
		-Wall                                           \
		-I$(GMLIBDIR)/inc                               \
		$(RTSPDDIR)/$(@F).c                             \
		$(RTSPDDIR)/log/log.c                           \
		$(RTSPDDIR)/librtsp.a                           \
		-L$(GMLIBDIR)/lib                               \
		-lpthread -lm -lrt -lgm -o $(RTSPDDIR)/rtspd
	@cp $(RTSPDDIR)/rtspd $(PREFIXDIR)/bin/rtspd
	@touch $@


$(UTILS):
	$(TARGET)-gcc -Wall -I$(GMLIBDIR)/inc -L$(GMLIBDIR)/lib -lpthread -lgm $(UTILSDIR)/$(@F).c -o $@


$(SAMPLES):
	$(TARGET)-gcc -Wall -I$(GMLIBDIR)/inc -L$(GMLIBDIR)/lib -lpthread -lgm $(GMSAMPLES)/$(@F).c -o $@


sdcard/manufacture.bin:
	tar -cf $(TOPDIR)/sdcard/manufacture.bin manufacture/test_drv


.PHONY: install website images uninstall clean


install: all
	@mkdir -p $(BINARIESDIR)                                                                    && \
	echo "*** Moving binaries to $(BINARIESDIR)"                                                && \
	cd $(PREFIXDIR)/bin  && cp $(BINS)  $(BINARIESDIR)                                          && \
	cd $(PREFIXDIR)/sbin && cp $(SBINS) $(BINARIESDIR)                                          && \
	cd $(TOPDIR)/tools   && find . -maxdepth 1 -type f -exec cp {} $(BINARIESDIR) \;            && \
	echo "*** Moving libraries to $(LIBRARIESDIR)"                                              && \
	cd $(PREFIXDIR)/lib  && cp *.a $(LIBRARIESDIR)                                              && \
	echo "*** Stripping binaries"                                                               && \
	$(TARGET)-strip $(BINARIESDIR)/*                                                            && \
	echo "*** Stripping libraries"                                                              && \
	find $(LIBRARIESDIR) -type f -not -name 'cacert.pem' -exec $(TARGET)-strip {} \;


website: all install
	@echo '*** Running composer install in $(WEBROOT)'                                          && \
	cd $(WEBROOT)                                                                               && \
	$(COMPOSER) install --no-dev                                                                && \
	echo '*** Removing symlinks from $(WEBROOT)/vendor to prevent fat32 symlink issues'         && \
	find $(WEBROOT)/vendor -type l -delete


images: all install website
	@echo "*** Creating archive of sdcard/ to chuangmi-720p-hack.zip and chuangmi-720p-hack.tgz" && \
	find . -maxdepth 1 -type f -name 'chuangmi-720p-hack.zip' -or -name 'chuangmi-720p-hack.tgz' -delete    && \
	zip -r --quiet chuangmi-720p-hack.zip README.md sdcard                                       && \
	tar czf chuangmi-720p-hack.tgz -C $(TOPDIR) README.md sdcard                                 && \
	echo "*** chuangmi-720p-hack.zip and chuangmi-720p-hack.tgz created"


uninstall:
	cd $(TOPDIR)/                                                                               && \
	rm -f chuangmi-720p-hack.tgz chuangmi-720p-hack.zip                                         && \
	rm -rf $(BINARIESDIR) $(SOURCEDIR) $(PREFIXDIR) $(BUILDDIR)                                 && \
	cd $(TOPDIR)/ && find tools -maxdepth 1 -type f -not -name 'README' | xargs rm -f


clean:
	rm -rf $(BINARIESDIR) $(SOURCEDIR) $(PREFIXDIR) $(BUILDDIR)                                 && \
	cd $(TOPDIR)/ && find tools -maxdepth 1 -type f -not -name 'README' | xargs rm -f

