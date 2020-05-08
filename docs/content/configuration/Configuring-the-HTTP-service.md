# Configuring the HTTP Service

There aren't many options to configure on the webserver as it is enabled by default.

**It is recommended to enable SSL on the webserver.**

## Restricting access

You can add a configuration snippet allow or deny access to the webserver.

To do this, open `/tmp/sd/firmware/etc/lighttpd.conf` and add the following section:

```lighttpd
## Limit access to the web to these ip's
$HTTP["remoteip"] !~ "^192\.168\.|^172\.1\.|10\." {
      url.access-deny = ( "" )
}
```

## Configuration options

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `ENABLE_HTTPD`           | `1` to enable, `0` to disable. | Enable or disable the lighttpd webserver |
| `HTTP_API_USER`          | The username string            | The username of the http only user |
| `HTTP_API_PASS`          | The password string            | The password of the http only user |

## configure SSL

Using encrypted connections on the web server is possible but as for now not default enabled.

To generate an SSL certificate and configure SSL on the camera,
[follow this instruction](/configuration/Creating-an-SSL-certificate-for-use-on-this-camera).

