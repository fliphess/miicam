# Using cronjobs

As the camera has a busybox installed that supports running a cron daemon, 
it is possible to schedule periodic tasks on the camera.

[Have a look online](https://www.howtogeek.com/101288/how-to-schedule-tasks-on-linux-an-introduction-to-crontab-files/)
[for a good howto](https://opensource.com/article/17/11/how-use-cron-linux) 
[on setting up cronjobs](http://www.adminschoice.com/crontab-quick-reference) ;)

If you need some help with the time format, take a look at [crontab.guru](https://crontab.guru/)

## Configuration options

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `ENABLE_CRON`            | `1` to enable, `0` to disable. | Enable or disable the cron daemon  |


## Configuring the cron daemon

To run periodic tasks, make sure the `crond` service is started at boot time by setting `ENABLE_CRON` to `1`.

You can then start the service through the services page in the web interface or by executing its init script:

```bash
## Start the cron daemon
/etc/init/S99crond start

## Restart the cron daemon
/etc/init/S99crond restart

## Stop the cron daemon
/etc/init/S99crond stop

## Get the status of the service
/etc/init/S99crond status
```

## Editing the crontab

The crontab file is stored as `/var/run/crontab`, 
but this file is stored in memory and will be deleted on each restart of the camera.

It is recommended to make the crontab file permanent by copying the crontab file 
from `/var/run/crontab` to `/tmp/sd/firmware/etc/crontab` so it will be restored on startup.

Using `crontab -e` is the best method of editing the (live) crontab, 
as the file is syntax checked before the cron daemon is reloaded. 
As this only changes the file in `/var/run`, 
after changing the crontab file you should copy the file to the SD card:

```bash
cp /var/run/crontab /tmp/sd/firmware/etc/crontab
```

(Or you can create a cronjob to do it ;))

The crontab utility allows several arguments to manipulate the live crontab file:

```bash
## List the cron jobs in crontab file
crontab -c /var/run/cron -l

## Edit the crontab file
crontab -c /var/run/cron -e

## Delete the crontab file (will be restored after a reboot when cron is enabled in `config.cfg`)
crontab -c /var/run/cron -r
```

## Running cron jobs as a restricted user

It is not possible to run cronjobs for other users when using the crontab file 
as the cron service only facilitates cronjobs for root.

If you want to schedule tasks as a restricted user, 
create the user by adding it to the `/tmp/sd/firmware/etc/passwd` and `/tmp/sd/firmware/etc/shadow` files 
and use `su` to run the job under another uid.

## Using `@` notation cronjobs

The busybox crond does **not** support  `@yearly`, `@hourly` or any other at sign notations.

Using [a little hack](https://github.com/MiiCam/MiiCam/blob/master/sdcard/firmware/etc/init/S99crond) in the init script of the crond, `@restart` is sort of possible, and jobs using this notation are executed every time the daemon is started through the init script:

```cron
@restart cat /tmp/sd/some/other/location/crontab >> /var/run/crontab
```


## Example: Setting night mode at a specific time.

If you want to switch the mode from day to night at a certain time of day and back in the morning when the sun is up, a cronjob can be useful to accomplish this behaviour.

To do this, edit `/tmp/sd/firmware/etc/crontab` and add the following 2 cronjobs:

```cron
## Disable nightmode at six in the morning
0 6 * * * /tmp/sd/firmware/bin/nightmode -d

## Enable nightmode at 18:30 in the evening
30 18 * * * /tmp/sd/firmware/bin/nightmode -e
```
