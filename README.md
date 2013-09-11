magSensor
=========

Nagios plugin to check temperature/humidity through Magiant's USB sensors.


To compile, simply use gcc:

                                      gcc magsensor.c -o magsensor

Then put the magsensor executable file in a convenient directory (I use /usr/local/bin) - Will improve this process through autotools



How to use it
=============

You can run magsensor with the following syntax:

    magsensor [-h] [-V] [-v] [-t <C|F>] [-m <T|H>] [-d <U|D>] [-z <Sensor name>] -p <devicePath> -c <value> -w <value>

Where :

-h (--help)      : Show quick help screen.

-v (--verbose)   : Enable verbose output.

-V (--version)   : Show script version.

-t <C|F>         : Temperature Units:  C - Celsius degrees
                                       F - Fahreneit degrees
                   If not specified, the default is "C" (Celsius).

-m <T|H>         : Reading Mode: T - Temperature
                                 H - Humidity
                   If not specified, the default is "T" (Temperature).

-l <U|D>         : Threshold mode: U - Alarms are triggered if read value is higher than threshold.
                                   D - Alarms are triggered if read value is lower than threshold.
                   If not specified, the default value is "U".
                 
-z <Sensor Name> : Sensor logical name, to easily identify returning messages.

-p <Device Path> : USB Device Path.

-c <value>       : Critical Threshold.

-w <value>       : Warning Threshold.




Configure Nagios
================

In the "nagioscfg" directory You'll find a couple of files, defining commands and services for Nagios. Modify them to suite Your needs,
then copy them in Your Nagios config directory. Finally, restart Nagios to activate them.

Since the Magiant Sensor is a USB device, the OS can deny access (even read-only) to it.
If this is the case, we need to modify "udev rules", creating a new file such as : 

/etc/udev/rules.d/90-magiant.rules

With a text editor, write in it the following line:

SUBSYSTEM=="usb", ATTRS{idVendor}=="04d8", MODE="0666"

This means that every device from Vendor "04d8" is installed with permission "666" (read allowed for User, Group and Owner).



pnp4nagios
==========

In the "pnp4nagios" directory You'll find the template that will be used to draw perfdata on pnp4nagios graphs.

To be used, the name of the template must follow the one of the Nagios command it's related to. Otherwise default.php will be used.

Take magCheckHumidity.php and magCheckTemperature.php files and put them in the pnp4nagios "template" directory: 
you'll stop using the "default" template to show perfdata from Magiant sensor. 
