# Time settings

This hack requires NTP to be accessible for the webcam.

If you experience issues with the rtp stream or you notice a time 
in the late seventies or early eighties in your log files, make sure the camera is connected to the LAN 
and is able to reach the configured NTP server.


## Wait for network

The wait-for-network options in `config.cfg`, are there to wait for the network to become available 
to ensure the time can be set at boot.

There are several options in the configuration file that can be changed:

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `WAIT_FOR_NETWORK`       | `1` to enable, `0` to disable. | Wait with boot until the network is up.
| `PING_IP`                | IP or hostname string          | The ip address to determine whether the network is up
| `PING_RETRIES`           | Max ping retries before boot   | The amount of retries before continu booting without network
| `PING_WAIT`              | Wait time in seconds           | The wait time in seconds in between ping probes


```bash
############################################################
## Wait for network connectivity                          ##
############################################################

## Wait with boot until network comes up
WAIT_FOR_NETWORK=1

## The IP to ping to verify if network is up
PING_IP="8.8.8.8"

## Ping Max retries
PING_RETRIES=10

## Wait time in seconds
PING_WAIT=2
```

## Camera without internet access

If you want to block the camera from accessing the internet, 
make sure you change the `PING_IP` setting to your local network gateway rather than an IP address on the internet.

To ensure your camera can set the correct time, you need to either run a local NTPd service for your local lan, 
or use the NTP proxy settings on your router if available.

To change the NTP server and timezone, edit the `config.cfg` to reflect the correct settings:

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `TIMEZONE`               | The timezone string            | The timezone to request ntp time for |
| `NTP_SERVER`             | The NTP server address string  | The ntp server to sync the time with |


```bash
############################################################
## Time Settings                                          ##
############################################################

## Set timezone
TIMEZONE="UTC"

## Prefered NTP server
NTP_SERVER="pool.ntp.org"
```

For an overview of all available timezones, 
have a look at the [timezone database](http://svn.fonosfera.org/fon-ng/trunk/luci/modules/admin-fon/root/etc/timezones.db)
