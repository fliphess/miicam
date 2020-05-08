# Creating an SSL Certificate for use on this camera

Using SSL actually is possible and can be enabled on the webserver.

As it is not recommended to use this camera over the internet, 
using Letsencrypt on this camera is not (easily) possible. 
You can order an expensive ssl certificate from an SSL provider, but this is useless for internal use.

The easiest method to acquire an ssl certificate for the ip camera is to generate a 
self-signed certificate to securely access the cameras web interface using `openssl`.

As it took me 4 hours to generate the required `dhparams` and the certificate files using the camera itself, 
doing so is not a very nice option for the impatient (like me).
It is better and a lot faster to do this on a computer or laptop ;)

To implement SSL, some manual configuration is (still) required and you need 
the openssl utility and libraries installed on your computer.

## Automagically generate the required certificates

If you have git and docker installed, you can use the `gencert` functionality in the `manage.sh` script in this repository. 
It executes all the required openssl magic in the correct order and copies the files in the right directory.

To do this, clone the repo and run:

```bash
./manage.sh --build-docker
./manage.sh --gencert
```

This starts a docker container and runs all the required commands.

*The certificate files will be stored in `sdcard/firmware/etc/ssl`*


## Manually generate an SSL certificate.

To create a ssl certificate chain that can be used on the camera, use the following steps.

### Prerequisites

Lets export some settings and create some directories that are required first:
```bash
cd <GIT REPOSITORY>
export SSLDIR="sdcard/firmware/etc/ssl"
mkdir -p $SSLDIR
```

Next, create a configuration file we need for setting up ssl:

```bash
cat > "$SSLDIR/v3.ext" <<EOF
authorityKeyIdentifier=keyid,issuer
basicConstraints=CA:FALSE
keyUsage = digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment
subjectAltName = @alt_names
[alt_names]
DNS.1 = camera.name
DNS.2 = camera.local
DNS.3 = camera.home
## Add your own aliases here
EOF
```

Use the hostname and or other varieties you use to reach the camera over the web for `DNS.*`

### Generate root (CA) key and cert

To create a root key and cert, use the following commands:

```bash
openssl genrsa -out "$SSLDIR/rootCA.key" 2048

openssl req                         \
        -x509                       \
        -new                        \
        -nodes                      \
        -key "$SSLDIR/rootCA.key"   \
        -sha256                     \
        -days 1024                  \
        -out "$SSLDIR/rootCA.pem"
```

### Create a private key, CSR and cert for the webserver

Next, Create a CSR and a key at once:

```bash
openssl req                         \
        -new                        \
        -nodes                      \
        -out "$SSLDIR/server.csr"   \
        -newkey rsa:2048            \
        -keyout "$SSLDIR/server.key"
```

And after that the certificate:

```bash
openssl x509                        \
        -req                        \
        -in "$SSLDIR/server.csr"    \
        -CA "$SSLDIR/rootCA.pem"    \
        -CAkey "$SSLDIR/rootCA.key" \
        -CAcreateserial             \
        -out "$SSLDIR/server.crt"   \
        -days 500                   \
        -sha256                     \
        -extfile "$SSLDIR/v3.ext"
```


### Generate a dhparams file

If all went well, it's time to generate a `dhparams` file. 
This can take some time, and as it took 4 hours on the camera itself to generate one, 
it is recommended to generate it on a regular computer or laptop.

Create one using the command:

```bash
openssl dhparam                     \
        -out "$SSLDIR/dh2048.pem"   \
        -outform PEM -2 2048
```


### Combine the key and cert into a single pem file

If the dhparam file is finally generated, 
all that's left to do is combine the generated ssl cert and key file in a single pem file.

This can be done by concatting both files to a new file:

```bash
cat "$SSLDIR/server.key" "$SSLDIR/server.crt" > "$SSLDIR/server.pem"
```


## Configure the web server to use SSL

When the certificates are generated it's time to configure `lighttpd` to use SSL.
For now this is a manual process, so buckle up, it's going to be a bumpy ride :)

First, login on the camera by opening a SSH connection to the camera.
When you are on the camera (or the SD card is still connected to your computer), 
open the `lighttpd` configuration file:

```bash
vim /tmp/sd/firmware/etc/lighttpd.conf
```

In the list if `server.modules`, add the `mod_openssl` module:
```lighttpd
server.modules = (
    "mod_openssl",
    ...
)
```

Now in the first line of the configuration file, change the port from `80` to `443`:

```lighttpd
#server.port = 80
server.port = 443
```

Now, right under the `server.port` setting, add the following content block:

```lighttpd
ssl.engine   = "enable"
ssl.pemfile  = "/tmp/sd/firmware/etc/ssl/server.pem"
ssl.ca-file  = "/tmp/sd/firmware/etc/ssl/rootCA.pem"

ssl.dh-file  = "/tmp/sd/firmware/etc/ssl/dh2048.pem"
ssl.ec-curve = "secp384r1"
```

This should suffice to configure an encrypted connection, 
but if you want to make it secure, look up the latest safe ssl configuration 
requirements on [cipherli.st](https://cipherli.st/)

At the time I wrote this, this was:

```lighttpd
ssl.honor-cipher-order = "enable"
ssl.cipher-list = "EECDH+AESGCM:EDH+AESGCM:AES256+EECDH:AES256+EDH"
ssl.use-compression = "disable"
setenv.add-response-header = (
    "Strict-Transport-Security" => "max-age=63072000; includeSubDomains; preload",
    "X-Frame-Options" => "DENY",
    "X-Content-Type-Options" => "nosniff"
)
ssl.use-sslv2 = "disable"
ssl.use-sslv3 = "disable"
```

Now copy the configuration file to etc and restart the webserver:
```bash
cp /tmp/sd/firmware/etc/lighttpd.conf /etc/lighttpd.conf
/etc/init/S99lighttpd restart
```


## Trusting the self-signed SSL certificate.

If you used the instructions in this documentation page or used the `manage.sh` utility to create the certs,
you have a root certificate and a certificate for the lighttpd web server (`server.pem`).

By importing the root certificate in your SSL keychain you can prevent annoying browser warnings.
