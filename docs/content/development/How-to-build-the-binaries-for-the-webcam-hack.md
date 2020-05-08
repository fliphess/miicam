# How to build the binaries for the webcam hack

You can build the binaries yourself on a computer with docker installed.

## Prerequisites

Clone this repository to your local disk:

```bash
git clone https://github.com/miicam/MiiCam.git
cd MiiCam
```

## Building using docker

To build the binaries, I recommend using docker, as the build and the environment setup is fully automated.

Please install docker on your computer using the instructions in their documentation prior to the build and use the `manage.sh` script to create a build:

The fully automated method:

```bash
## Build the container environment,
## the binaries, the website and pack it all together:
./manage.sh --all
```

Or use the non-shortcut-fully-automated method if you want to type a bit more:

```bash
## Build the docker container
./manage.sh --build-docker

## Build the binaries
./manage.sh --build
```

## Building without docker

### Considerations

As the docker uses an Ubuntu 20.04 container and used 18.04 before, I know for sure it should work on Focal Fossa and Bionic Beaver but I expect the cross compile toolchain to work on any linux distribution as long as you install the required utilities and tools.

Have a look at the contents of the `Dockerfile` and `manage.sh` for more information on how to setup your environment and build the binaries.


### Download and setup the toolchain

If you are looking for the full sdk file, containing all data sheets, manufacturer pdf's and extended instructions on how to use the gmlib library, download the [full rar sdk files](https://fliphess.com/toolchain/GM8136_SDK_release_v1.0.rar).

To build the binaries only the GM8136 SDK toolchain must be installed in `/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE`, you don't need the kernel modules, uboot, linux kernel, rootfs and other tools to cross compile the binaries.

You can download the toolchain and other files from [my website](https://fliphess.com/toolchain).


### Prepare your environment

To prepare your environment and move the required cross compiler toolchain in the right place, have a look at the dockerfile to see the most recent instructions to setup an build environment.

To copy the files in place:

```bash
mkdir -p /usr/src/arm-linux-3.3

URL="https://fliphess.com/toolchain/Software/Embedded_Linux/source/toolchain_gnueabi-4.4.0_ARMv5TE.tgz"
curl -qs --output /tmp/toolchain.tgz ${URL}

cd /usr/src/arm-linux-3.3
tar xzf /tmp/toolchain.tgz
```


### Building

After moving the toolchain in place, building should be as easy as running make in the main directory of the repository:

```bash
cd chuangmi-720p-hack
make images clean
```


## Using multiple threads to build the images

The current `Makefile` does not yet support building in multiple threads as there are some bugs in the requirements I haven't found yet, but feel very welcome to create a PR to implement it.

Currently I alternatively use the `PROCS` variable inside the `Makefile` to build the individual binaries using multiple threads. If you have a much bigger computer, change the PROCS to the amount of cpu cores plus one extra `(CPUs +1)`.


## Adding new binaries to the firmware

If you want to add new binaries, rebuild the docker environment to make sure it reflects the latest changes in your working dir and open a shell:

```bash
./manage.sh --build-docker
./manage.sh --shell
```

Or use the shortcut:

```bash
./manage.sh --newshell
```

Now you can try whatever you want inside the container. If you messed up, exit the container and retry with a clean environment.

I prefer to edit the `Makefile` and `SOURCES.mk` in the repository itself and copy it inside the container build directory to keep track and prevent my changes to be gone after accidentally exiting the container.

Have a look in `tools/dev/helpers.sh` for some utils for building and testing new binaries.


To update the changed files by hand, copy the files from `/result/` to `/env`:

```bash
cd /env
renew_make
rebuild_rtsp
```

After that you can test the binary on your camera by scp-ing or rsync-ing the binary to the camera and execute it.

## Helper utils

I've created some helper utils in `tools/dev/helpers.sh` that are useful when devving in the container.
It makes recompiling an uploading binaries to the camera a bit easier until I've build an update mechanism.

To use the dev helpers, open the file and change the `CAMERA_HOSTNAME` variable to the hostname or ip of your camera.
