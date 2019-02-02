
WEBSITEARCHIVE := website.tgz
WEBSITEURI     := $(shell curl -s https://api.github.com/repos/miicam/miicamweb/releases/latest  | grep browser_download_url | awk '{print $$NF}' | tr -d '"' )

LIBGDARCHIVE = libgd-$(LIBGDVERSION).tar.gz
LIBGDURI = https://github.com/libgd/libgd/releases/download/gd-$(LIBGDVERSION)/$(LIBGDARCHIVE)
curl -s https://api.github.com/repos/miicam/miicamweb/releases/latest  | grep browser_download_url | tr -d '"' | awk '{print $NF}'

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

LIBPOPTVERSION = 1.16
LIBPOPTARCHIVE = popt-$(LIBPOPTVERSION).tar.gz
LIBPOPTURI = http://rpm5.org/files/popt/popt-$(LIBPOPTVERSION).tar.gz

NCURSESVERSION = 6.1
NCURSESARCHIVE = ncurses-$(NCURSESVERSION).tar.gz
NCURSESURI = https://ftp.gnu.org/pub/gnu/ncurses/$(NCURSESARCHIVE)

READLINEVERSION = 7.0
READLINEARCHIVE = readline-$(READLINEVERSION).tar.gz
READLINEURI = https://ftp.gnu.org/gnu/readline/$(READLINEARCHIVE)

OPENSSLVERSION = 1.0.2q
OPENSSLARCHIVE = openssl-$(OPENSSLVERSION).tar.gz
OPENSSLURI = https://www.openssl.org/source/$(OPENSSLARCHIVE)

LIBPCAPVERSION = 1.9.0
LIBPCAPARCHIVE = libpcap-$(LIBPCAPVERSION).tar.gz
LIBPCAPURI = http://www.tcpdump.org/release/$(LIBPCAPARCHIVE)

TCPDUMPVERSION = 4.9.2
TCPDUMPARCHIVE = tcpdump-$(TCPDUMPVERSION).tar.gz
TCPDUMPURI = http://www.tcpdump.org/release/$(TCPDUMPARCHIVE)

SOCATVERSION = 1.7.3.2
SOCATARCHIVE = socat-$(SOCATVERSION).tar.gz
SOCATURI = http://www.dest-unreach.org/socat/download/$(SOCATARCHIVE)

SFTPVERSION = 7.9p1
SFTPARCHIVE = openssh-$(SFTPVERSION).tar.gz
SFTPURI = https://ftp.nluug.nl/pub/OpenBSD/OpenSSH/portable/$(SFTPARCHIVE)

DROPBEARVERSION = 2018.76
DROPBEARARCHIVE = dropbear-$(DROPBEARVERSION).tar.bz2
DROPBEARURI = https://matt.ucc.asn.au/dropbear/releases/$(DROPBEARARCHIVE)

LIGHTTPDVERSION = 1.4.52
LIGHTTPDARCHIVE = lighttpd-$(LIGHTTPDVERSION).tar.gz
LIGHTTPDURI = https://download.lighttpd.net/lighttpd/releases-1.4.x/$(LIGHTTPDARCHIVE)

PHPVERSION = 7.2.12
PHPARCHIVE = php-$(PHPVERSION).tar.bz2
PHPURI = http://php.net/get/$(PHPARCHIVE)/from/this/mirror

NANOVERSION = 2.9.8
NANOARCHIVE = nano-$(NANOVERSION).tar.gz
NANOURI = https://www.nano-editor.org/dist/v2.9/$(NANOARCHIVE)

VIMVERSION = 8.1.0629
VIMARCHIVE = vim-$(VIMVERSION)
VIMURI = https://github.com/vim/vim/archive/v$(VIMVERSION).tar.gz

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

LOGROTATEVERSION = 3.15.0
LOGROTATEARCHIVE = logrotate-$(LOGROTATEVERSION).tar.gz
LOGROTATEURI = https://github.com/logrotate/logrotate/releases/download/$(LOGROTATEVERSION)/$(LOGROTATEARCHIVE)

