# Using Auto Night Mode

**Auto night mode is completely rewritten and new and shiny.**

## Auto night mode

Auto night mode is a little daemon that turns on the IR led and/or the night mode
when a certain minimum IR or light value has been reached.

By running it in the foreground with `-v` (verbose),
you can determine the light values that you want the camera to switch on
the IR led and the black and white view (night mode).

## Command line arguments

The auto night mode switcher supports the following arguments:

```
Usage:
   auto_night_mode [-d|-e|-i|-l|-n|-v]

Available options:
  -d  (int)  delay in seconds (default: 10)
  -e  (int)  lowest EV value
  -i  (int)  lowest IR value
  -l  (bool) switch IR led
  -n  (bool) switch night mode
  -c  (bool) switch ir cut
  -v  be verbose
```

Example:

```
## Only control night mode
auto_night_mode -e 200 -i 5 -n

## Only switch ir led
auto_night_mode -e 200 -i 5 -l

## Set a delay of 2 minutes
auto_night_mode -e 200 -i 5 -l -n -d 120

## Switch IR cut when IR value is low
auto_night_mode -e 200 -i 5 -c
```


## Tuning the Auto Night Mode Switcher

To use the `auto_night_mode` utility, you need to tune it so it switches at the correct light values.

To do this, start the switcher using a very low delay and with the verbose flag.
You'll notice it prints the `EV` and `IR` values to the terminal every few seconds:

```bash
# auto_night_mode -d 1 -e 200 -i 5 -l -n -v

*** Auto nightmode started
*** Nightmode values changed: ev=173 ir=49
*** Enable night mode triggered: ev=(173,200)
*** Turning on nightmode
*** Setting nightmode to 1
*** Nightmode values changed: ev=173 ir=48
*** Nightmode values changed: ev=186 ir=50
*** Nightmode values changed: ev=173 ir=48
*** Nightmode values changed: ev=175 ir=48
*** Nightmode values changed: ev=173 ir=48
*** Nightmode values changed: ev=175 ir=47
```

By setting the values that are printed for `ev` and `ir`, you set the value at which the switcher should be active.

Using the following settings, the switcher will behave as follows:
```bash
# auto_night_mode -d 10 -e 200 -i 5 -l -n
```

**Sequence**: `measure` -> `act` -> `sleep`

- The sensor data is retrieved
- When the `EV` value is below 200 the nightmode is switched ON
- When the `EV` value is over 200 the nightmode is switched OFF
- When the `IR` value is below 5 the IR led is switched ON
- When the `IR` value is over 5 the IR led is switch OFF
- The loop sleeps for 10 seconds

## Setting the delay

When you set the `-d` (delay) flag, the sequence will wait N seconds between measurements (and switching).


## IR cut switching

By setting the `-c` flag, you can use ir cut switching as well.

When the IR Led goes on, it does not make much sense to filter the IR led as well, hence the option to switch IR cut off.

## Configuration Options

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `AUTO_NIGHT_MODE`        | `1` to enable, `0` to disable. | Enable or disable the auto nightmode service (no auth) |
| `AUTO_NIGHT_MODE_ARGS`   | CLI arguments                  | Set the cli arguments for the auto nightmode daemon |

Default arguments: `AUTO_NIGHT_MODE_ARGS="-d 120 -e 200 -i 5 -l -n -v"`

## Use the old, original nightmode switcher

You can still use the previous, original nightmode switcher by changing `auto_night_mode` to `/usr/sbin/ir_sample` in the init script.
