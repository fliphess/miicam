# Configuration file options

There are many options in the configuration file. As not all have their own documentation page, 
this page contains a list of some options, a description and sometimes links to other documentation pages.

## Manage Xiaomi functionality

| Configuration   | Options                        | Description |
| ---             | ---                            | ---     |
| `DISABLE_HACK`  | `1` to enable, `0` to disable. | Disable the custom firmware and boot the chinese firmware. |
| `DISABLE_CLOUD` | `1` to enable, `0` to disable. | Disable all xiaomi functionality including the cloud streaming, audio, record on motion detection and firmware updates. |
| `DISABLE_OTA`   | `1` to enable, `0` to disable. | Allow cloud functionality like record on motion detection and usage of the Mii Home App, but disable firmware updates of the official firmware. |

## System Settings

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `HOSTNAME`               | The hostname string            | The hostname of the camera
| `ROOT_PASSWORD`          | The root password string       | The root pass (used for SSH, telnet, samba and http)

## Wifi Settings

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `WIFI_PASS`              | The wifi password string       | The WIFI Password. Will be saved in nvram, so can be cleared for privacy purposes. |
| `WIFI_SSID`              | The wifi SSID string           | The WIFI SSID. Will be saved in nvram at boot, so can be cleared after first boot. |


## Services

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `AUTO_NIGHT_MODE`        | `1` to enable, `0` to disable. | Enable or disable the automatic night mode daemon |


### Camera Options

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `CEILING_MODE`           | `1` to enable, `0` to disable. | Enable ceiling rotation |


See more info the [camera options page](/Camera-Options)
