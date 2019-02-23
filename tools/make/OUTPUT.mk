
## The third party binaries in prefix/sbin that should be stripped and copied to firmware/bin
THIRD_PARTY_SBINS :=			\
	fatlabel					\
	fsck.fat					\
	mkfs.fat					\
	dropbear					\
	logrotate					\
	lighttpd					\
	lighttpd-angel				\
	tcpdump


## The third party binaries in prefix/bin that should be stripped and copied to firmware/bin
THIRD_PARTY_BINS :=				\
	scp	dbclient dropbearkey	\
	sftp-server 				\
	arm-php	arm-php-cgi			\
	bash						\
	ffmpeg ffprobe				\
	openssl						\
	procan socat filan			\
	pcregrep pcretest			\
	jq							\
	lsof						\
	fromdos todos				\
	nano						\
	rsync						\
	strace						\
	vim	xxd						\
	wget

## Extra tools and symlinks that should not be stripped but copied too
THIRD_PARTY_BIN_EXTRAS :=		\
	rview						\
	vimdiff						\
	ex							\
	rvim						\
	view


## Third party library files that should be stripped and copied to firmware/lib
THIRD_PARTY_LIBS :=				\
	libavcodec.so.58.18.100		\
	libavdevice.so.58.3.100		\
	libavfilter.so.7.16.100		\
	libavformat.so.58.12.100	\
	libavutil.so.56.14.100		\
	libcrypto.a					\
	libform.a					\
	libgd.so.3.0.5				\
	libhistory.so.7.0			\
	libjpeg.so.62.2.0			\
	libmenu.a					\
	libncurses++.a				\
	libncurses.a				\
	libpanel.a					\
	libpcap.a					\
	libpcap.so.1.9.0			\
	libpcre.so.1.2.10			\
	libpcrecpp.so.0.0.1			\
	libpcreposix.so.0.0.6		\
	libpng16.so.16.36.0			\
	libpopt.so.0.0.0			\
	libpostproc.so.55.1.100		\
	libreadline.so.7.0			\
	libssl.a					\
	libswresample.so.3.1.100	\
	libswscale.so.5.1.100		\
	libturbojpeg.so.0.1.0		\
	libx264.so.155				\
	libxml2.so.2.9.8			\
	libz.a						\
	libz.so.1.2.11				\
	libjq.a						\
	libjq.so.1.0.4				\
	libonig.a					\
	libonig.so.4.0.0			\
	mod_access.so				\
	mod_accesslog.so			\
	mod_alias.so				\
	mod_auth.so					\
	mod_authn_file.so			\
	mod_cgi.so					\
	mod_compress.so				\
	mod_deflate.so				\
	mod_dirlisting.so			\
	mod_evasive.so				\
	mod_evhost.so				\
	mod_expire.so				\
	mod_extforward.so			\
	mod_fastcgi.so				\
	mod_flv_streaming.so		\
	mod_indexfile.so			\
	mod_openssl.so				\
	mod_proxy.so				\
	mod_redirect.so				\
	mod_rewrite.so				\
	mod_rrdtool.so				\
	mod_scgi.so					\
	mod_secdownload.so			\
	mod_setenv.so				\
	mod_simple_vhost.so			\
	mod_sockproxy.so			\
	mod_ssi.so					\
	mod_staticfile.so			\
	mod_status.so				\
	mod_uploadprogress.so		\
	mod_userdir.so				\
	mod_usertrack.so			\
	mod_vhostdb.so				\
	mod_webdav.so				\
	mod_wstunnel.so


## The symlinks to library files that are linked and should be copied to firmware/lib but not stripped
THIRD_PARTY_LIB_EXTRAS :=		\
	bash                        \
	libjq.so					\
	libjq.so.1					\
	libonig.so					\
	libonig.so.4				\
	libavcodec.so				\
	libavcodec.so.58			\
	libavdevice.so				\
	libavdevice.so.58			\
	libavfilter.so				\
	libavfilter.so.7			\
	libavformat.so				\
	libavformat.so.58			\
	libavutil.so				\
	libavutil.so.56				\
	libgd.so					\
	libgd.so.3					\
	libhistory.so				\
	libhistory.so.7				\
	libjpeg.so					\
	libjpeg.so.62				\
	libpcap.so					\
	libpcap.so.1				\
	libpcre.so					\
	libpcre.so.1				\
	libpcrecpp.so				\
	libpcrecpp.so.0				\
	libpcreposix.so				\
	libpcreposix.so.0			\
	libpng.so					\
	libpng16.so					\
	libpng16.so.16				\
	libpopt.so					\
	libpopt.so.0				\
	libpostproc.so				\
	libpostproc.so.55			\
	libreadline.so				\
	libreadline.so.7			\
	libswresample.so			\
	libswresample.so.3			\
	libswscale.so				\
	libswscale.so.5				\
	libturbojpeg.so				\
	libturbojpeg.so.0			\
	libx264.so					\
	libxml2.so					\
	libxml2.so.2				\
	libz.so						\
	libz.so.1

