# Security Considerations

TLDR: **DO NOT EVER PUBLICLY EXPOSE THIS WEBCAM TO THE INTERNETZ!**

This is not a very secure webcam. Although it is semi-safe to use for home automation projects, 
exposing it to the internet would backfire quickly and is considered 
very [badong](https://www.urbandictionary.com/define.php?term=badong).

The camera is running an old u-boot version, a kernel from the middle ages 
and was never designed to be exposed to the internet. 
The kernel has no iptables or any other traffic filtering mechanism build in, 
so securing this ip camera from the big angry internet is not easily done without expensive hardware.

Use the camera with common sense: Use secure strong passwords, 
use a private and secure wifi network, put it behind a firewall, 
don't show your junk in front of the camera and make sure you don't place it in private spaces 
that you don't want others on your wifi network to see.

Generate an SSL certificate and implement it on the web server. 
You can find out how to do this in the [ssl instructions](/configuration/Creating-an-SSL-certificate-for-use-on-this-camera).

As the RTSP daemon is semi-secured with authentication, non-computer-skilled people in your network 
might not be able to view the camera, but the guy in the hoodie leeching on your wifi and who knows 
how to use a packet sniffer certainly can.

I use this camera's on a private wifi dedicated to IOT devices. 
It has no connection to the internet but NTP and ping and is not accessible from 
my users wifi other than through zone minder.

This way i feel i've at least tried to prevent others from being able to access my cameras for evil.

Although the camera was originally designed as a baby monitor, 
I would not recommend to use it that way without some additional security layers. 
Not with the hack and not without it either.
Don't look at me when your private sauna pictures (or worse) show up on the internet: I warned you ;)

Use the firmware only indoors and at your own risk. 
Use a safe and certified power adapter to power the camera and make sure it doesnt get wet or moist. 
Use common sense and don't be evil!


