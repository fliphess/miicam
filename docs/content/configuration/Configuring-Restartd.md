# Configuring Restart Daemon

Restartd is a daemon for checking running and not running processes.
It reads the /proc directory every n seconds and does a POSIX regexp on the process names.
You can execute a script or a program if the process is or is not running.

By default, if enabled the ntpd and the dropbear SSH daemon are both guarded using restartd.
You can enable the restartd service by settings `ENABLE_RESTARTD` to `1`.

You can add your own services to restartd to automatically restart the services when they are killed due to out-of-memory issues or when something flaky is going on.

## Configuration options

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `ENABLE_RESTARTD`         | `1` to enable, `0` to disable. | Start or Stop `restartd` at boot time|


## Restartd Config

To add your services, add a configuration line to the `/etc/restartd.conf`.

Use the following format:
```bash
# <process_name> <regexp> <action_if_not_running> <action_if_running>

# process_name: the name of the process which is just for logging
# as it does not affect for the regexp
#
# regexp: the POSIX regular expression for the command line of the
# process
#
# action_if_not_running: a script or program name to execute if the
# regexp does not match the full process command line
#
# action_if_running: a script or program name to execute if the regexp
# matches the full process command line
```

For example:
```bash
dropbear "/tmp/sd/firmware/bin/dropbear" "/tmp/sd/firmware/etc/init/S99dropbear restart" "/bin/echo '*** Dropbear SSH was restarted from restartd... '"
ntpd "/usr/sbin/ntpd" "/tmp/sd/firmware/etc/init/S51ntpd restart" "/bin/echo '*** NTPd was restarted from restartd... '"
```
