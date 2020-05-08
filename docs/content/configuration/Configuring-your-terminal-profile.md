# Configuring your terminal profile

If you experience issues when using command line tools that use ncurses, 
for example `nano` complaining about the terminal profile not being set, or a more generic error like:

```
nano error: Error opening terminal: xterm-256color
```

Your terminal is not configured accurately.

## Terminal profiles

This error is caused because the camera has only a limited set of terminal profiles available.

These profiles configure settings like terminal size, the presence of color and the usage of special characters, 
and can be found on the readonly filesystem of the camera in `/usr/share/terminfo`

You can configure a terminal profile manually by exporting the `TERM` variable, 
or by adding them to your [shell configuration file](/configuration/Configuring-the-terminal):


## Available terminal profiles

The current available profiles are:

```bash
ansi
linux
screen
vt100
vt102
vt200
vt220
xterm
xterm-color
xterm-xfree86
```

## Manually configure a terminal profile

Manually configuring only works for the current SSH session, when you logout the settings are gone.

To do this, issue the following command:
```bash
export TERM="xterm-color"
```

## Permanent configuration

To make the terminal profile settings permanent, add the settings to your shell configuration in `.profile`:

```bash
echo 'export TERM="xterm-color"' >> /tmp/sd/firmware/root/.profile
echo 'export TERM="xterm-color"' >> /root/.profile
```

Now close your ssh connection and reconnect.
You can check whether the settings is correctly set, by issuing:

```bash
env | grep TERM
```
