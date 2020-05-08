# Flashing the U boot firmware to an older version when the hack is not working

If you updated the camera to the latest firmware version using the mii home app, 
the firmware does not allow booting from an SD card anymore, resulting in the hack not working.

## How to flash the firmware

To revert this behaviour, you can flash the firmware of the camera
to an older u-boot version that still allows booting from SD.

To activate this firmware, use the `tf_recovery.img` file:

- Place the `tf_recovery.img` in the root of the SD card and remove any other files.
- Slide the SD card in the slot and power up the camera.
- Wait for 10 minutes till the blue led goes on.
- Unplug the camera's power.
- If the flash is successful, the `tf_recovery.img` file is renamed to `tf_recovery.img.bak`.

**If you are using a mac, add the SD card to your spotlight private excludes 
in your settings to prevent the `.Spotlight` and `.fseventsd` directory to be created, which could block the reset.**

