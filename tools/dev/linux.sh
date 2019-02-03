#!/bin/bash

echo "*** Downloading linux kernel from toolchain"
wget -O /tmp/linux.tgz https://fliphess.com/toolchain/Software/Embedded_Linux/source/arm-linux-3.3_2015-01-09.tgz

echo "*** Unpacking linux kernel"
tar xvzf /tmp/linux.tgz -C /usr/src

echo "*** Unpacking rootfs"
cd /usr/src/arm-linux-3.3 && tar xvzf target_rootfs.tgz -C .
