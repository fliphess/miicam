#################################################################
## FFMPEG													  ##
#################################################################

FFMPEGVERSION := $(shell cat $(SOURCES) | jq -r '.ffmpeg.version' )
FFMPEGARCHIVE := $(shell cat $(SOURCES) | jq -r '.ffmpeg.archive' )
FFMPEGURI     := $(shell cat $(SOURCES) | jq -r '.ffmpeg.uri' )

#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(FFMPEGARCHIVE): $(SOURCEDIR)
	$(call box,"Downloading ffmpeg source code")
	test -f $@ || $(DOWNLOADCMD) $@ $(FFMPEGURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/ffmpeg: $(SOURCEDIR)/$(FFMPEGARCHIVE) $(BUILDDIR)/x264 $(BUILDDIR)/zlib
	$(call box,"Building ffmpeg")
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(FFMPEGVERSION)
	@tar -xjf $(SOURCEDIR)/$(FFMPEGARCHIVE) -C $(BUILDDIR)
	@cd $@-$(FFMPEGVERSION)						\
		&& $(BUILDENV)							\
		./configure								\
			--extra-cxxflags="-L$(PREFIXDIR)/lib -I$(PREFIXDIR)/include"   \
			--extra-ldflags=" -L$(PREFIXDIR)/lib -I$(PREFIXDIR)/include -Wl,-rpath,/tmp/sd/firmware/lib" \
			--arch=arm							\
			--target-os=linux					\
			--cross-prefix=$(TARGET)-			\
			--prefix=$(PREFIXDIR)				\
			--disable-ffplay					\
			--disable-doc						\
			--disable-w32threads				\
			--disable-static					\
			--enable-shared						\
			--enable-avcodec					\
			--enable-avformat					\
			--enable-avfilter					\
			--enable-swresample					\
			--enable-swscale					\
			--enable-ffmpeg						\
			--enable-filters					\
			--enable-gpl						\
			--enable-iconv						\
			--enable-libx264					\
			--enable-nonfree					\
			--enable-pthreads					\
			--enable-runtime-cpudetect			\
			--enable-small						\
			--enable-version3					\
			--enable-zlib						\
		&& make -j$(PROCS)						\
		&& make -j$(PROCS) install
	@rm -rf $(BUILDDIR)/$@-$(FFMPEGVERSION)
	@touch $@

#################################################################
##                                                             ##
#################################################################
