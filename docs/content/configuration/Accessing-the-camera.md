# Accessing the camera

The camera can be accessed using SSH or Telnet. (When enabled)

I recommend using the SSH service, as this is more secure and easier to use when you have key authentication in use.

## Configuration options for SSH

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `ENABLE_SSHD`            | `1` to enable, `0` to disable. | Enable or disable the dropbear SSH daemon |
| `DROPBEAR_EXTRA_ARGS`    | A command line string          | Set extra command line arguments to the SSH daemon |


## Using the SSH server

If enabled the SSH server is on port 22.


## Enabling the dropbear SSH service

The dropbear SSH service can be enabled at boot time by setting `ENABLE_SSHD` to `1` in `config.cfg`.

After changing the setting, you can start the dropbear SSH service using the services page in the web interface or by issuing a start using the init script:

```bash
/etc/init/S99dropbear start
```

To stop the dropbear SSH service use:
```
/etc/init/S99dropbear stop
```

## Using SSH key authentication

As the dropbear SSH service supports authorized key authentication, 
you are encouraged to use SSH keys rather than password authentication.

You can add a `.ssh` directory containing an `authorized_keys` file to enable key authentication.

As `/root` is a `tmpfs` (a filesystem that only exists in memory), 
it is deleted every time the camera is rebooted or shut-off and re-created every time at boot time.

To ensure your key is permanent after a reboot, 
don't just add your key to `/root/.ssh/authorized_keys` but add it in `/tmp/sd/firmware/root/.ssh/authorized_keys`. 
This way the key will be copied to /root at boot time.


## Creating an SSH keypair

For the best performance, use ECDSA keys rather dan RSA or DSA keys.

To create a key pair:
```
ssh-keygen -t ecdsa -f identity
```

After generating the key pair, you can add the content of `identity.pub` to your `authorized_keys` file.


## Preserving the SSH host key

If you have to accept a new SSH host key fingerprint on each login after a reboot, 
make sure the `/etc/dropbear/dropbear_ecdsa_host_key` is correctly copied 
to `/tmp/sd/firmware/etc/dropbear` at SSH startup.


At start time, a new ssh host key is generated if nonexistent. 
This creates a new fingerprint for the server, resulting in the same dialog over and over:

```
The authenticity of host 'camera (192.168.1.1)' can't be established.
ECDSA key fingerprint is SHA256:FINMZzDR+oDsXp5i5peVRoMB7vV1KOUJm8ExS3aV7u8.
Are you sure you want to continue connecting (yes/no)?
```

If this dialog keeps appearing after a reboot, 
the generated host key is not correctly copied to your sd card and restored after a reboot. 
Check your `tf_boot.log`log file to find the culprit.


## Disabling password authentication

Disabling password authentication for root 
or for all users can be accomplished by changing the command line arguments for the dropbear SSH service.

This can be done using the `DROPBEAR_EXTRA_ARGS` variable, 
which is appended to the command line arguments in the init script of the SSH daemon.

To disable password authentication, the `-s` flag is available. 
Have a look [in the manpage](https://www.systutorials.com/docs/linux/man/8-dropbear/) or 
the [website of the dropbear daemon](https://matt.ucc.asn.au/dropbear/dropbear.html) 
to see which options are available for your convenience.


## Using the Telnet server

**The telnet protocol is unencrypted and therefore insecure. 
Only use it when needed, but disable the service after usage and stick to SSH for general access of the camera**

If the telnet service is enabled it is listening on port 23.


## Configuration options for Telnet

| Configuration            | Options                        | Description |
| ---                      | ---                            | ---         |
| `ENABLE_TELNETD`         | `1` to enable, `0` to disable. | Enable or disable the telnet daemon |


## Authentication issues

If boot issues appear due to syntax errors in scripts, the root password is often not set, resulting in an unauthenticated telnet service. Check your logs to find the culprit.

## Enabling the telnet service

The telnet service can be enabled at boot time by setting `ENABLE_TELNETD` to `1` in `config.cfg`.

After changing the setting, you can start the telnet service using the services page in the web interface or by issuing a start using the init script:

```
/etc/init/S99telnet start
```

To stop the telnet service use:
```
/etc/init/S99telnet stop
```

