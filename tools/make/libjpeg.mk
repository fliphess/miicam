#################################################################
## LIBJPEG-TURBO											   ##
#################################################################

LIBJPEGVERSION := $(shell cat $(SOURCES) | jq -r '.libjpeg.version' )
LIBJPEGARCHIVE := $(shell cat $(SOURCES) | jq -r '.libjpeg.archive' )
LIBJPEGURI     := $(shell cat $(SOURCES) | jq -r '.libjpeg.uri' )

#################################################################
##                                                             ##
#################################################################

$(SOURCEDIR)/$(LIBJPEGARCHIVE): $(SOURCEDIR)
	$(call box,"Downloading libjpeg source code")
	test -f $@ || $(DOWNLOADCMD) $@ $(LIBJPEGURI) || rm -f $@


#################################################################
##                                                             ##
#################################################################

$(BUILDDIR)/libjpeg-turbo: $(SOURCEDIR)/$(LIBJPEGARCHIVE)
	$(call box,"Building libjpeg-turbo")
	@mkdir -p $(BUILDDIR) && rm -rf $@-$(LIBJPEGVERSION)
	@tar -xzf $(SOURCEDIR)/$(LIBJPEGARCHIVE) -C $(BUILDDIR)
	@cd $@-$(LIBJPEGVERSION)		\
	&& echo "set(CMAKE_SYSTEM_NAME Linux)"                          >> toolchain.cmake \
	&& echo "set(CMAKE_SYSTEM_PROCESSOR arm)"                       >> toolchain.cmake \
	&& echo "set(CMAKE_CROSSCOMPILING true)"                        >> toolchain.cmake \
	&& echo "set(CMAKE_CXX_COMPILER $(TOOLCHAINDIR)/$(TARGET)-gcc)" >> toolchain.cmake \
	&& echo "set(CMAKE_C_COMPILER $(TOOLCHAINDIR)/$(TARGET)-gcc)"   >> toolchain.cmake \
	&& echo "set(CMAKE_FIND_ROOT_PATH $(PREFIXDIR))"                   >> toolchain.cmake \
	&& echo "set(CMAKE_INSTALL_PREFIX $(PREFIXDIR))"                   >> toolchain.cmake \
	&& export CFLAGS="-fPIC -march=armv5te -m32 -Os"                                   \
	&& export LDFLAGS="-m32 -pie -I$(PREFIXDIR)/include -L$(PREFIXDIR)/lib -Wl,-rpath -Wl,/tmp/sd/firmware/lib -Wl,--enable-new-dtags" \
	&& $(BUILDENV)                                 \
		cmake -G"Unix Makefiles"                   \
			-DCMAKE_TOOLCHAIN_FILE=toolchain.cmake \
			-DCMAKE_POSITION_INDEPENDENT_CODE=1    \
			-DCMAKE_INSTALL_PREFIX=$(PREFIXDIR)    \
			-DWITH_JPEG8=1 .                       \
	&& make clean                                  \
	&& make -j$(PROCS)                             \
	&& make -j$(PROCS) install
	@rm -rf $@-$(LIBJPEGVERSION)
	@touch $@

#################################################################
##                                                             ##
#################################################################
