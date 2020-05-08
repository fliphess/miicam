# How does this hack work

This firmware extension uses the kernel and the userland that is build by the creators of the original firmware.


## How does the hack work

By using a developer boot mode in that allows running scripts from the SD card at boot-time, 
we manipulate the boot sequence of the camera.

This developer mode normally verifies the external software on the SD card to be signed with a GPG key from the developers. 
By overwriting the verification script with our own, we ensure our software can be started as well.

As newer firmwares do not provide the "mode 4" developer mode anymore, 
[an older firmware is provided](/Flashing-the-U-boot-firmware-to-an-older-version-when-the-hack-is-not-working)
to downgrade to a firmware that does support this boot mode.

Normally the software created by Xiaomi is started when the camera is powered on, but instead of the regular boot script, 
we use our own scripts to ensure the Xiaomi cloud suite is not able to start.

After this is done, we start our boot sequence starting the web server, the rtspd and the dropbear SSH server.
After our own boot scripts, the rest of the boot sequence is continued as usual to ensure all required services 
and configuration is done at boot time.
