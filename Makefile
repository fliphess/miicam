
TOOLCHAINDIR = /usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin
PATH  := $(TOOLCHAINDIR):$(PATH)
TARGET = arm-unknown-linux-uclibcgnueabi

PROCS = 2

BUILDENV :=                 \
	AR=$(TARGET)-ar         \
	AS=$(TARGET)-as         \
	CC=$(TARGET)-gcc        \
	CXX=$(TARGET)-g++       \
	LD=${TARGET}-ld         \
	NM=$(TARGET)-nm         \
	RANLIB=$(TARGET)-ranlib \
	STRIP=$(TARGET)-strip


TOPDIR      := $(CURDIR)
SOURCEDIR   := $(TOPDIR)/src
PREFIXDIR   := $(TOPDIR)/prefix
BUILDDIR    := $(TOPDIR)/build

INSTALLDIR  := $(TOPDIR)/sdcard/firmware/bin
WEBROOT     := $(TOPDIR)/sdcard/firmware/www
COMPOSER    := /usr/local/bin/composer

BINS =                                                  \
		smbpasswd smbstatus smbtree smbclient           \
		scp dbclient dropbearkey                        \
		arm-php arm-php-cgi                             \
		chuangmi_ctrl                                   \
		ffmpeg ffprobe                                  \
		runas                                           \
		rsync                                           \
		rtspd                                           \
		gd2togif gd2topng gdparttopng gdtopng giftogd2 pngtogd pngtogd2

SBINS =          \
		dropbear \
		lighttpd \
		nmbd     \
		smbd

GMLIBDIR  := $(TOPDIR)/tools/gm_lib
RTSPDDIR  := $(TOPDIR)/tools/rtspd
GMUTILDIR := $(TOPDIR)/tools/utils

UTILS :=                                                \
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


include SOURCES.mk

.PHONY: all

all:                                     \
		libs                             \
		$(BUILDDIR)/dropbear             \
		$(BUILDDIR)/lighttpd             \
		$(BUILDDIR)/php                  \
		$(BUILDDIR)/samba                \
		$(BUILDDIR)/chuangmi_ctrl        \
		$(BUILDDIR)/runas                \
		$(BUILDDIR)/rsync                \
		$(BUILDDIR)/ffmpeg               \
		$(BUILDDIR)/rtspd                \
		sdcard/manufacture.bin           \
		utils

libs:                                    \
		$(BUILDDIR)/zlib                 \
		$(BUILDDIR)/libxml2              \
		$(BUILDDIR)/libjpeg-turbo        \
		$(BUILDDIR)/libpng               \
		$(BUILDDIR)/libgd                \
		$(BUILDDIR)/pcre                 \
		$(BUILDDIR)/x264

utils: $(UTILS)


$(BUILDDIR)/zlib: $(SOURCEDIR)/$(ZLIBARCHIVE)
	mkdir -p $(BUILDDIR) && rm -rf $@-$(ZLIBVERSION)
	tar -xzf $(SOURCEDIR)/$(ZLIBARCHIVE) -C $(BUILDDIR)
	cd $@-$(ZLIBVERSION)              && \
		$(BUILDENV)                      \
		./configure                      \
			--prefix=$(PREFIXDIR)        \
			--static                  && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	rm -rf $@-$(ZLIBVERSION)
	touch $@


$(BUILDDIR)/libxml2: $(SOURCEDIR)/$(LIBXML2ARCHIVE) $(BUILDDIR)/zlib
	mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBXML2VERSION)
	tar -xzf $(SOURCEDIR)/$(LIBXML2ARCHIVE) -C $(BUILDDIR)
	cd $@-$(LIBXML2VERSION)           && \
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
	rm -rf $@-$(LIBXML2VERSION)
	touch $@


$(BUILDDIR)/libjpeg-turbo: $(SOURCEDIR)/$(LIBJPEGARCHIVE)
	mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBJPEGVERSION)
	tar -xzf $(SOURCEDIR)/$(LIBJPEGARCHIVE) -C $(BUILDDIR)
	cd $@-$(LIBJPEGVERSION)           && \
		$(BUILDENV)                      \
		./configure                      \
			--prefix=$(PREFIXDIR)        \
			--host=$(TARGET)             \
			--disable-shared             \
			--enable-static           && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	rm -rf $@-$(LIBJPEGVERSION)
	touch $@


$(BUILDDIR)/libpng: $(SOURCEDIR)/$(LIBPNGARCHIVE)
	mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBPNGVERSION)
	tar -xzf $(SOURCEDIR)/$(LIBPNGARCHIVE) -C $(BUILDDIR)
	cd $@-$(LIBPNGVERSION)            && \
		$(BUILDENV)                      \
		LDFLAGS="-L$(PREFIXDIR)/lib"     \
		CPPFLAGS="-I$(PREFIXDIR)/include" \
		./configure                      \
			--prefix=$(PREFIXDIR)        \
			--host=$(TARGET)             \
			--disable-shared             \
			--enable-static           && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	rm -rf $@-$(LIBPNGVERSION)
	touch $@


$(BUILDDIR)/libgd: $(SOURCEDIR)/$(LIBGDARCHIVE) $(BUILDDIR)/zlib $(BUILDDIR)/libjpeg-turbo $(BUILDDIR)/libpng
	mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBGDVERSION)
	tar -xzf $(SOURCEDIR)/$(LIBGDARCHIVE) -C $(BUILDDIR)
	cd $@-$(LIBGDVERSION)             && \
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
	rm -rf $@-$(LIBGDVERSION)
	touch $@


$(BUILDDIR)/pcre: $(SOURCEDIR)/$(PCREARCHIVE) $(BUILDDIR)/zlib
	mkdir -p $(BUILDDIR) && rm -rf $@-$(PCREVERSION)
	unzip -q $(SOURCEDIR)/$(PCREARCHIVE) -d $(BUILDDIR)
	cd $@-$(PCREVERSION)              && \
		$(BUILDENV)                      \
		./configure                      \
			--host=$(TARGET)             \
			--prefix=$(PREFIXDIR)        \
			--disable-shared             \
			--enable-static           && \
		make -j$(PROCS)               && \
		make -j$(PROCS) install
	rm -rf $@-$(PCREVERSION)
	touch $@


$(BUILDDIR)/x264: $(SOURCEDIR)/$(X264ARCHIVE)
	mkdir -p $(BUILDDIR) && rm -rf $@-$(X264VERSION)
	tar -xjf $(SOURCEDIR)/$(X264ARCHIVE) -C $(BUILDDIR)
	cd $@-$(X264VERSION)                && \
	$(BUILDENV)                            \
		./configure                        \
			--host=$(TARGET)               \
			--cross-prefix=$(TARGET)-      \
			--prefix="$(PREFIXDIR)"        \
			--enable-static                \
			--enable-pic                   \
			--disable-asm               && \
		make -j$(PROCS)                 && \
		make -j$(PROCS) install
	rm -rf $@-$(X264VERSION)
	touch $@


$(BUILDDIR)/dropbear: $(SOURCEDIR)/$(DROPBEARARCHIVE) $(BUILDDIR)/zlib
	mkdir -p $(BUILDDIR) && rm -rf $@-$(DROPBEARVERSION)
	tar -xjf $(SOURCEDIR)/$(DROPBEARARCHIVE) -C $(BUILDDIR)
	sed -i 's|\(#define DROPBEAR_PATH_SSH_PROGRAM\).*|\1 "/tmp/sd/ft/dbclient"|' $@-$(DROPBEARVERSION)/options.h
	sed -i 's|\(#define DEFAULT_PATH\).*|\1 "/bin:/sbin:/usr/bin:/usr/sbin:/tmp/sd/ft:/mnt/data/ft"|' $@-$(DROPBEARVERSION)/options.h
	cd $@-$(DROPBEARVERSION)          && \
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
	touch $@


$(BUILDDIR)/lighttpd: $(SOURCEDIR)/$(LIGHTTPDARCHIVE) $(BUILDDIR)/zlib $(BUILDDIR)/pcre
	mkdir -p $(BUILDDIR) && rm -rf $@-$(LIGHTTPDVERSION)
	tar -xzf $(SOURCEDIR)/$(LIGHTTPDARCHIVE) -C $(BUILDDIR)
	for i in access                     \
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
		flv_streaming                   \
		indexfile                       \
		proxy                           \
		redirect                        \
		rewrite                         \
		rrdtool                         \
		scgi                            \
		secdownload                     \
		setenv                          \
		simple_vhost                    \
		ssi                             \
		staticfile                      \
		status                          \
		uploadprogress                  \
		userdir                         \
		usertrack                       \
		vhostdb                         \
		webdav;                         \
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
			--without-mysql             \
			--without-bzip2          && \
		make -j$(PROCS)              && \
		make -j$(PROCS) install
	rm -rf $@-$(LIGHTTPDVERSION)
	touch $@


$(BUILDDIR)/php: $(SOURCEDIR)/$(PHPARCHIVE) $(BUILDDIR)/zlib $(BUILDDIR)/libxml2 $(BUILDDIR)/libjpeg-turbo $(BUILDDIR)/libpng $(BUILDDIR)/pcre $(BUILDDIR)/libgd
	mkdir -p $(BUILDDIR) && rm -rf $@-$(PHPVERSION)
	tar -xjf $(SOURCEDIR)/$(PHPARCHIVE) -C $(BUILDDIR)
	sed -i -e '/.*hp_ini_register_extensions.*/d' $@-$(PHPVERSION)/main/main.c
	cd $@-$(PHPVERSION)              && \
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
			--disable-all            && \
		make -j$(PROCS)              && \
		make -j$(PROCS) install-binaries
	rm -rf $@-$(PHPVERSION)
	touch $@


$(BUILDDIR)/samba: $(SOURCEDIR)/$(SAMBAARCHIVE)
	mkdir -p $(BUILDDIR) && rm -rf $@-$(SAMBAVERSION)
	tar -xzf $(SOURCEDIR)/$(SAMBAARCHIVE) -C $(BUILDDIR)
	cd $@-$(SAMBAVERSION)/source3    && \
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
	rm -rf $@-$(SAMBAVERSION)
	touch $@


$(BUILDDIR)/chuangmi_ctrl: $(SOURCEDIR)/$(MIJIACTRLARCHIVE)
	mkdir -p $(BUILDDIR) && rm -rf $(BUILDDIR)/mijia-720p-ctrl-$(MIJIACTRLVERSION)              && \
	unzip $(SOURCEDIR)/$(MIJIACTRLARCHIVE) -d $(BUILDDIR)                                       && \
	cd $(BUILDDIR)/mijia-720p-ctrl-$(MIJIACTRLVERSION)                                          && \
	make                                                                                        && \
	cp mijia_ctrl $(PREFIXDIR)/bin/chuangmi_ctrl                                                && \
	rm -rf $(BUILDDIR)/mijia-720p-ctrl-$(MIJIACTRLVERSION)                                      && \
	cd $(TOPDIR)                                                                                && \
	touch $@


$(BUILDDIR)/runas: $(SOURCEDIR)/$(RUNASARCHIVE)
	mkdir -p $(BUILDDIR) && rm -rf $(BUILDDIR)/static-sudo-$(RUNASVERSION)                      && \
	unzip $(SOURCEDIR)/$(RUNASARCHIVE) -d $(BUILDDIR)                                           && \
	cd $(BUILDDIR)/static-sudo-$(RUNASVERSION)                                                  && \
	$(TARGET)-gcc -static -W -Wall -Wextra -Werror -pedantic rpzsudo.c -o runas                 && \
	cp runas $(PREFIXDIR)/bin/runas                                                             && \
	rm -rf $(BUILDDIR)/static-sudo-$(RUNASVERSION)                                              && \
	cd $(TOPDIR)                                                                                && \
	touch $@


$(BUILDDIR)/rsync: $(SOURCEDIR)/$(RSYNCARCHIVE)
	mkdir -p $(BUILDDIR) && rm -rf $@-$(RSYNCVERSION)                                           && \
	tar -xzf $(SOURCEDIR)/$(RSYNCARCHIVE) -C $(BUILDDIR)                                        && \
	cd $@-$(RSYNCVERSION)                                                                       && \
		$(BUILDENV)                                                                                \
		./configure CFLAGS="-static" EXEEXT="-static"                                              \
		--host=$(TARGET)                                                                           \
		--target=$(TARGET)                                                                      && \
		make -j$(PROCS)                                                                         && \
	cp rsync $(PREFIXDIR)/bin/rsync                                                             && \
	rm -rf $(BUILDDIR)/$@-$(RSYNCVERSION)                                                       && \
	cd $(TOPDIR)                                                                                && \
	touch $@


$(BUILDDIR)/ffmpeg: $(SOURCEDIR)/$(FFMPEGARCHIVE) $(BUILDDIR)/x264 $(BUILDDIR)/zlib
	mkdir -p $(BUILDDIR) && rm -rf $@-$(FFMPEGVERSION)     && \
	tar -xjf $(SOURCEDIR)/$(FFMPEGARCHIVE) -C $(BUILDDIR)  && \
	cd $@-$(FFMPEGVERSION)                                 && \
		$(BUILDENV)                                           \
		./configure                                           \
			--pkg-config-flags="--static"                     \
			--extra-cflags=-I$(PREFIXDIR)/include             \
			--extra-cxxflags="-I$(PREFIXDIR)"                 \
			--extra-ldflags=-L$(PREFIXDIR)/lib                \
		    --arch=arm                                        \
		   	--target-os=linux                                 \
		   	--cross-prefix=$(TARGET)-                         \
			--prefix=$(PREFIXDIR)                             \
			--disable-shared                                  \
			--disable-ffplay                                  \
			--disable-doc                                     \
			--disable-w32threads                              \
			--enable-avcodec                                  \
			--enable-avformat                                 \
			--enable-avfilter                                 \
			--enable-swresample                               \
			--enable-swscale                                  \
			--enable-ffmpeg                                   \
			--enable-filters                                  \
			--enable-gpl                                      \
			--enable-iconv                                    \
			--enable-libx264                                  \
			--enable-nonfree                                  \
			--enable-pthreads                                 \
			--enable-runtime-cpudetect                        \
			--enable-small                                    \
			--enable-static                                   \
			--enable-version3                                 \
			--enable-zlib                                  && \
		make -j$(PROCS)                                    && \
        make -j$(PROCS) install                            && \
	rm -rf $(BUILDDIR)/$@-$(FFMPEGVERSION)                 && \
	cd $(TOPDIR)                                           && \
	touch $@


sdcard/manufacture.bin:
	tar -cf $(TOPDIR)/sdcard/manufacture.bin manufacture/test_drv


$(BUILDDIR)/rtspd:
	mkdir -p $(BUILDDIR) $(PREFIXDIR)/bin            && \
	cd $(RTSPDDIR)                                   && \
	$(TARGET)-gcc                                       \
		-Wall                                           \
		-I$(GMLIBDIR)/inc                               \
		$(RTSPDDIR)/$(@F).c                             \
		$(RTSPDDIR)/librtsp.a                           \
		-L$(GMLIBDIR)/lib                               \
		-lpthread -lm -lrt -lgm -o $(RTSPDDIR)/rtspd && \
	cp $(RTSPDDIR)/rtspd $(PREFIXDIR)/bin/rtspd      && \
    touch $@


$(UTILS):
	$(TARGET)-gcc -Wall -I$(GMLIBDIR)/inc -L$(GMLIBDIR)/lib -lpthread -lgm $(GMUTILDIR)/$(@F).c -o $@


.PHONY: website install images uninstall clean

website: all
	cd $(WEBROOT)                                                                               && \
	echo '*** Running composer install in $(WEBROOT)'                                           && \
	$(COMPOSER) install --no-dev                                                                && \
	echo '*** Removing symlinks from $(WEBROOT)/vendor to prevent fat32 symlink issues'         && \
	find $(WEBROOT)/vendor -type l -delete

install: all
	mkdir -p $(INSTALLDIR)                                                                      && \
	echo "*** Moving binaries to $(INSTALLDIR)"                                                 && \
	cd $(PREFIXDIR)/bin  && cp $(BINS) $(INSTALLDIR)                                            && \
	cd $(PREFIXDIR)/sbin && cp $(SBINS) $(INSTALLDIR)                                           && \
	cd $(TOPDIR)/tools  && find . -maxdepth 1 -type f -exec cp {} $(INSTALLDIR) \;              && \
	echo "*** Stripping binaries"                                                               && \
	$(TARGET)-strip $(INSTALLDIR)/*

images: install website
	echo "*** Creating archive of sdcard/ to chuangmi-720p-hack.zip and chuangmi-720p-hack.tgz" && \
	zip -r --quiet chuangmi-720p-hack.zip README.md sdcard                                      && \
	tar czf chuangmi-720p-hack.tgz -C $(TOPDIR) README.md sdcard                                && \
	echo "*** chuangmi-720p-hack.zip and chuangmi-720p-hack.tgz created"

uninstall:
	cd $(TOPDIR)/                                                                               && \
	rm -f chuangmi-720p-hack.tgz chuangmi-720p-hack.zip                                         && \
	rm -rf $(INSTALLDIR) $(SOURCEDIR) $(PREFIXDIR) $(BUILDDIR)                                  && \
	cd $(TOPDIR)/ && find tools -maxdepth 1 -type f -not -name 'README' | xargs rm -f

clean:
	rm -rf $(INSTALLDIR) $(SOURCEDIR) $(PREFIXDIR) $(BUILDDIR)                                  && \
	cd $(TOPDIR)/ && find tools -maxdepth 1 -type f -not -name 'README' | xargs rm -f

