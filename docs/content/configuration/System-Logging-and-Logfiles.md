# System Logging and Log files

Using the `ENABLE_LOGGING` setting in `config.cfg` you can enable and disable the kernel logging and syslog daemons.

### Logging Configuration Settings

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `ENABLE_LOGGING`         | `1` to enable, `0` to disable. | Enable klogd and syslogd |
| `ENABLE_REMOTE_SYSLOG`   | `1` to enable, `0` to disable. | Send logging to a remote syslog server |
| `REMOTE_SYSLOG_HOST`     | `Host:Port`                    | The remote syslog server host |
| `PURGE_LOGFILES_AT_BOOT` | `1` to enable, `0` to disable. | Remove all log files in /tmp/sd/logs at boot time. |

## Purge log files

When debugging boot issues or startup errors, it is recommended to set `PURGE_LOGFILES_AT_BOOT` to `0`.

This prevents the camera from deleting the log files every time the camera starts, wiping out your history.

## Log rotation

If the cron daemon is enabled, a cronjob triggering logrotate is ran every evening.
To add logfiles or change the settings, edit `/etc/logrotate.conf`.

The `logrotate` utility ensures log files doe not grow too large to handle for the camera 
by creating a compressed archive of the logfile and truncating the current logfile 
so it can be refilled during the day.

## Remote syslog

You can use remote syslogging to a syslog server by setting the `ENABLE_REMOTE_SYSLOG` to `1` 
and defining a remote syslog host in `REMOTE_SYSLOG_HOST`. 
If your syslog server uses a non-default port, set the port as well, using: `host:port`.

## Using `/etc/syslog.conf`

You can define a syslog configuration file by creating a configuration file in `/etc/syslog.conf`. 
The busybox syslog daemon uses a similar syntax as the rsyslog daemon.

## Kernel logging

By default, if syslogging is enabled, the kernel log daemon is enabled as well, logging kernel messages to syslog.




