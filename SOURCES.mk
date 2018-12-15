
DOWNLOADCMD := wget -t 5 -T 10 -c -O

ZLIBVERSION = 1.2.11
ZLIBARCHIVE = zlib-$(ZLIBVERSION).tar.gz
ZLIBURI = https://www.zlib.net/$(ZLIBARCHIVE)

LIBXML2VERSION = 2.9.8
LIBXML2ARCHIVE = libxml2-$(LIBXML2VERSION).tar.gz
LIBXML2URI = http://xmlsoft.org/sources/$(LIBXML2ARCHIVE)

LIBJPEGVERSION = 1.5.3
LIBJPEGARCHIVE = libjpeg-turbo-$(LIBJPEGVERSION).tar.gz
LIBJPEGURI = http://prdownloads.sourceforge.net/libjpeg-turbo/$(LIBJPEGARCHIVE)

LIBPNGVERSION = 1.6.36
LIBPNGARCHIVE = libpng-$(LIBPNGVERSION).tar.gz
LIBPNGURI = https://prdownloads.sourceforge.net/libpng/$(LIBPNGARCHIVE)

LIBGDVERSION = 2.2.5
LIBGDARCHIVE = libgd-$(LIBGDVERSION).tar.gz
LIBGDURI = https://github.com/libgd/libgd/releases/download/gd-$(LIBGDVERSION)/$(LIBGDARCHIVE)

PCREVERSION = 8.42
PCREARCHIVE = pcre-$(PCREVERSION).zip
PCREURI = https://ftp.pcre.org/pub/pcre/$(PCREARCHIVE)

NCURSESVERSION = 6.1
NCURSESARCHIVE = ncurses-$(NCURSESVERSION).tar.gz
NCURSESURI = https://ftp.gnu.org/pub/gnu/ncurses/$(NCURSESARCHIVE)

LIBPCAPVERSION = 1.9.0
LIBPCAPARCHIVE = libpcap-$(LIBPCAPVERSION).tar.gz
LIBPCAPURI = http://www.tcpdump.org/release/$(LIBPCAPARCHIVE)

TCPDUMPVERSION = 4.9.2
TCPDUMPARCHIVE = tcpdump-$(TCPDUMPVERSION).tar.gz
TCPDUMPURI = http://www.tcpdump.org/release/$(TCPDUMPARCHIVE)

DROPBEARVERSION = 2018.76
DROPBEARARCHIVE = dropbear-$(DROPBEARVERSION).tar.bz2
DROPBEARURI = https://matt.ucc.asn.au/dropbear/releases/$(DROPBEARARCHIVE)

LIGHTTPDVERSION = 1.4.52
LIGHTTPDARCHIVE = lighttpd-$(LIGHTTPDVERSION).tar.gz
LIGHTTPDURI = https://download.lighttpd.net/lighttpd/releases-1.4.x/$(LIGHTTPDARCHIVE)

PHPVERSION = 7.2.12
PHPARCHIVE = php-$(PHPVERSION).tar.bz2
PHPURI = http://php.net/get/$(PHPARCHIVE)/from/this/mirror

SAMBAVERSION = 3.6.25
SAMBAARCHIVE = samba-$(SAMBAVERSION).tar.gz
SAMBAURI = https://download.samba.org/pub/samba/$(SAMBAARCHIVE)

MIJIACTRLVERSION = master
MIJIACTRLARCHIVE = mijia-720p-ctrl-$(MIJIACTRLVERSION).zip
MIJIACTRLURI = https://github.com/cck56/mijia-720p-ctrl/archive/$(MIJIACTRLVERSION).zip

NANOVERSION = 2.9.8
NANOARCHIVE = nano-$(NANOVERSION).tar.gz
NANOURI = https://www.nano-editor.org/dist/v2.9/$(NANOARCHIVE)

RUNASVERSION = master
RUNASARCHIVE = runas.zip
RUNASURI = https://github.com/remram44/static-sudo/archive/$(RUNASVERSION).zip

RSYNCVERSION = 3.1.3
RSYNCARCHIVE = rsync-$(RSYNCVERSION).tar.gz
RSYNCURI = https://download.samba.org/pub/rsync/src/$(RSYNCARCHIVE)

X264VERSION = snapshot-20181028-2245-stable
X264ARCHIVE = x264-$(X264VERSION).tar.bz2
X264URI = https://download.videolan.org/pub/x264/snapshots/$(X264ARCHIVE)

FFMPEGVERSION = 4.0.2
FFMPEGARCHIVE = ffmpeg-$(FFMPEGVERSION).tar.bz2
FFMPEGURI = https://ffmpeg.org/releases/$(FFMPEGARCHIVE)

STRACEVERSION = 4.10
STRACEARCHIVE = strace-$(STRACEVERSION).tar.xz
STRACEURI = https://downloads.sourceforge.net/project/strace/strace/$(STRACEVERSION)/$(STRACEARCHIVE)

LSOFVERSION = master
LSOFARCHIVE = $(LSOFVERSION).tar.gz
LSOFURI = https://github.com/Distrotech/lsof/archive/$(LSOFVERSION).tar.gz


$(SOURCEDIR)/$(ZLIBARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(ZLIBURI)      || rm -f $@

$(SOURCEDIR)/$(LIBXML2ARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LIBXML2URI)   || rm -f $@

$(SOURCEDIR)/$(LIBJPEGARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LIBJPEGURI)   || rm -f $@

$(SOURCEDIR)/$(LIBPNGARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LIBPNGURI)    || rm -f $@

$(SOURCEDIR)/$(LIBGDARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LIBGDURI)     || rm -f $@

$(SOURCEDIR)/$(PCREARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(PCREURI)      || rm -f $@

$(SOURCEDIR)/$(X264ARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(X264URI)      || rm -f $@

$(SOURCEDIR)/$(OPENJPEGARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(OPENJPEGURI)  || rm -f $@

$(SOURCEDIR)/$(NCURSESARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(NCURSESURI)  || rm -f $@

$(SOURCEDIR)/$(LIBPCAPARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LIBPCAPURI)  || rm -f $@

$(SOURCEDIR)/$(TCPDUMPARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(TCPDUMPURI)  || rm -f $@

$(SOURCEDIR)/$(DROPBEARARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(DROPBEARURI)  || rm -f $@

$(SOURCEDIR)/$(LIGHTTPDARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LIGHTTPDURI)  || rm -f $@

$(SOURCEDIR)/$(PHPARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(PHPURI)       || rm -f $@

$(SOURCEDIR)/$(SAMBAARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(SAMBAURI)     || rm -f $@

$(SOURCEDIR)/$(MIJIACTRLARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(MIJIACTRLURI) || rm -f $@

$(SOURCEDIR)/$(RSYNCARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(RSYNCURI)     || rm -f $@

$(SOURCEDIR)/$(RUNASARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(RUNASURI)     || rm -f $@

$(SOURCEDIR)/$(NANOARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(NANOURI)     || rm -f $@

$(SOURCEDIR)/$(FFMPEGARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(FFMPEGURI)    || rm -f $@

$(SOURCEDIR)/$(STRACEARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(STRACEURI)    || rm -f $@

$(SOURCEDIR)/$(LSOFARCHIVE):
	mkdir -p $(SOURCEDIR) && $(DOWNLOADCMD) $@ $(LSOFURI)      || rm -f $@

