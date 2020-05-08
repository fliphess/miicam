# Configuring FTP

If enabled the FTP server is listening on port 21.

**As there is no login/password required, make sure you disable the FTP service after usage.**

## Configuration Options

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `ENABLE_FTPD`            | `1` to enable, `0` to disable. | Enable or disable the FTP service (no auth) |
| `FTP_ROOT`               | A directory path               | Set the content root of the ftp service |
| `FTP_EXTRA_ARGS`         | A command line string          | Set extra commandline arguments to the ftpd |


## Enabling the FTP service

You can start the FTP service by setting `ENABLE_FTPD=1` in `config.cfg`
followed by starting he FTP service using the service page in the web interface, 
or by starting it on the command line using the init script:

```
/etc/init/S99ftpd start
```

## Stopping the FTP service

Using the same init script, you can stop the FTP service:

```
/etc/init/S99ftpd stop
```

If you want to disable the FTP service at boot time, make sure `ENABLE_FTPD` is set to `0`.

## Setting the content root of the ftp service

By changing the `FTP_ROOT` variable in `config.cfg`, you can change the root directory of the FTP service.

This is useful to prevent access to all files on the filesystem and especially when using write access.

**Don't set `FTP_ROOT=/` as this gives full (readonly) access to all files on the camera.**


## Allow file upload over FTP

By default, the ftp service is configured READONLY. 
This is a security measure to prevent unauthorized changes of files on the filesystem 
as there is no authentication mechanism available for the ftp service.

If you want to upload files to the camera, add the `-w` flag to the `FTP_EXTRA_ARGS` variable in `config.cfg` 
and restart the FTP service.

Make sure you set the `FTP_ROOT` to the directory in which you want to change the files.

**NEVER combine `-w` and `FTP_ROOT=/`, as this gives full unauthenticated write access to all files on the camera!**



