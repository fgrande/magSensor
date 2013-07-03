magSensor
=========

Nagios plugin to check temperature through Magiant's USB sensors.


To compile, simply use gcc:

                                      gcc magSensor.c -o magsensor

Then put the magsensor executable file in a convenient directory (I use /usr/local/bin)



Configure Nagios
================

In the config directory You'll find a couple of files, defining commands and services for Nagios. Modify them to suite Your needs,
then copy them in Your Nagios config directory. Fnally, restart Nagios to activate them.

Since the Magiant Sensor is a USB device, the OS can deny access (even read-only) to it.
If this is the case, we need to modify "udev rules", creating a new file such as : 

/etc/udev/rules.d/90-magiant.rules

With a text editor, write in it the following line:

SUBSYSTEM=="usb", ATTRS{idVendor}=="04d8", MODE="0666"

This means that every device from Vendor "04d8" is installed with permission "666" (read allowed for User, Group and Owner).
