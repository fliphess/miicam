# Adding software to the build


To add a piece of software it needs to be cross-compiled using the compiler in the toolchain.

Have a look in `tools/make` for all third party software that is already compiled.

In the early days all tools in the MiiCam hack were statically compiled,
but as I needed more memory for the rtspd to extend the motion detection,
snapshot functionality but mostly: as I wanted to add openssl libraries to the `lighttpd` server,
which was already around 40 mb on disk as nearly all the modules were included in a single binary, this had to change.

All third party software is now dynamically linked.

You can add the created libraries and output binary files to the `tools/make/OUTPUT.mk` lists
to make sure the files are stripped and then copied to the `sdcard/firmware/bin/` and `sdcard/firmware/lib/` directory.

Using the `LD_LIBRARY_PATH` variable the shell is configured to use the libraries in `/tmp/firmware/lib`
as well the `/lib/` directory.

To test your changes, first start cross-compiling the software in the container
without the general `Makefile` in the repo to keep things simple.
If things workout, you can easily copy your shell history into the make files.

After creating a makefile in `tools/make`, include this file in the general `Makefile` and make sure the software is build by adding it to the `all` list.

## Helpers

I've created several helper utils that are sourced when using the container.
For being able to upload created bins to the camera, set the hostname to an existing camera:

```
cp tools/dev/host.cfg.example tools/dev/host.cfg
echo "camera.local" > tools/dev/host.cfg
```
