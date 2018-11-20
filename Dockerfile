FROM ubuntu:18.04

####################################################
## Build environment (for manual devving)         ##
####################################################

ENV TOOLCHAINDIR="/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin"
ENV PATH="${TOOLCHAINDIR}:${PATH}"

ENV TARGET="arm-unknown-linux-uclibcgnueabi"

ENV AR="${TARGET}-ar"
ENV AS="${TARGET}-as"
ENV CC="${TARGET}-gcc"
ENV CXX="${TARGET}-g++"
ENV LD="${TARGET}-ld"
ENV NM="${TARGET}-nm"
ENV RANLIB="${TARGET}-ranlib"
ENV STRIP="${TARGET}-strip"

ENV TOPDIR="/build"
ENV SOURCEDIR="${TOPDIR}/src"
ENV PREFIXDIR="${TOPDIR}/prefix"
ENV BUILDDIR="${TOPDIR}/build"

ENV INSTALLDIR="${TOPDIR}/sdcard/firmware/bin"
ENV WEBROOT="${TOPDIR}/sdcard/firmware/www"


####################################################
## Install dependencies and requirements          ##
####################################################

ENV DEBIAN_FRONTEND=noninteractive

RUN echo "*** Install required packages" \
 && apt-get -qq update                   \
 && apt-get -qq install -y               \
      autoconf                           \
      ca-certificates                    \
      bison                              \
      build-essential                    \
      curl                               \
      flex                               \
      gawk                               \
      git                                \
      lib32z1-dev                        \
      locales                            \
      make                               \
      ncurses-dev                        \
      openssl                            \
      php-cli                            \
      php-mbstring                       \
      unrar                              \
      unzip                              \
      vim                                \
      wget                               \
      zip                                \
 && apt-get clean

####################################################
## Configure locales                              ##
####################################################

RUN locale-gen en_US.UTF-8


####################################################
## Install composer                               ##
####################################################

RUN echo "*** Installing composer"       \
 && mkdir -p /usr/local/bin              \
 && curl -sS https://getcomposer.org/installer | php -- --install-dir=/usr/local/bin --filename=composer


####################################################
## Download and unpack toolchain                  ##
####################################################

RUN echo "*** Downloading toolchain"     \
 && mkdir -p /usr/src/arm-linux-3.3      \
 && curl -qs --output /tmp/toolchain.tgz https://fliphess.com/toolchain/Software/Embedded_Linux/source/toolchain_gnueabi-4.4.0_ARMv5TE.tgz

RUN echo "*** Unpacking Toolchain"       \
 && cd /usr/src/arm-linux-3.3            \
 && tar xzf /tmp/toolchain.tgz


####################################################
## Get repo                                       ##
####################################################

COPY . /build


####################################################
## Setting workdir to /build                      ##
####################################################

WORKDIR /build

