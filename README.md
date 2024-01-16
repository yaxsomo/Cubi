### Description
Simple open source bluetooth controlled wheeled robot. Mechanical design, electronics and software designs available on this repo.
I used @Gregwar's Holo as a starting point.

The initial design is a cube like robot :

https://www.youtube.com/watch?v=2ZDV0_4vD0g

https://www.youtube.com/watch?v=TOusHRbKWQg

A second design was also made to play around cart-poling:

https://www.youtube.com/watch?v=Ubej95gH9Gs

TODO:

- Add Julien's O. designs and events

- Add Julien's A. and Jérémy's designs and event (cap science)


### Simulation
Some simulation stuff (simple kinematic control, inverted pendulum tests) is available in Python here :
https://bitbucket.org/RemiFabre/cubi/src/master/simulation/

0) Installation
You'll need Python (any version should work) and pygame
```
pip install pygame
```

1) Simple control of a wheeled robot (click around)
```
python main.py
```
https://www.youtube.com/watch?v=O8ZoUAvGYmE

2) Inverted pendulum (cart-pole) simulation
```
python cart_pole.py
```
https://www.youtube.com/watch?v=xhEcLnnj7fg




### How to make it work
You'll have to install the arm cross-compilation tools. On debian-like distributions,
you can get it with:

```
    sudo aptitude install build-essential git-core dfu-util openocd python \
        python-serial binutils-arm-none-eabi gcc-arm-none-eabi libstdc++-arm-none-eabi-newlib
```
If you're running an Ubuntu 14.04 or older, you might need to install the libstdc++-arm-none-eabi-newlib packet by hand. Download link :
http://ftp.de.debian.org/debian/pool/main/libs/libstdc++-arm-none-eabi/libstdc++-arm-none-eabi-newlib_4.8.3-9+4_all.deb

Command (if a conflict rises, you can add --force-overwrite to the following line):
```
	sudo dpkg -i libstdc++-arm-none-eabi-newlib_4.8.3-9+4_all.deb
```

Then go to Cubi/firmware/src and run :

```
bash ./prepare.sh
```


Then, go to the `firmware/` directory and edit the `Makefile` to set the appropriate
`BOARD` variable, targetting your servo.

You can then run:

```
make
```

Use an USB cable to connect your pc with the robot's board

```
make install
```
If you didn't follow this section to setup udev rules, you might need to ```sudo make install``` instead

http://static.leaflabs.com/pub/leaflabs/maple-docs/0.0.12/unix-toolchain.html#toolchain-udev


### Bluetooth
You can control the robot without the need of a cable. The most straight-forward way is to download the "Metabot" app from Google Play. Once on the app you can scan for devices, find your robot and pair it with your phone.

If you want to pair your PC with your robot, one method is as follows (Ubuntu).

Uninstall modemmanager (not needed but causes some issues)

```
    sudo apt-get purge modemmanager
```

Scan for bluetooth devices and save the adress (e.g. 98:D3:37:90:ED:D6) of your robot. For example, we use blueman-manager :

```
    sudo apt-get install blueman
    sudo blueman-manager
```

Assign a serial port to the connection. You can do that on the graphical interface of blueman-manager but doing it directly with rfcomm seems more robust :

```
    sudo rfcomm bind /dev/rfcomm0 98:D3:37:90:ED:D6
```

Now you can access the remote terminal on the robot with :

```
    cu -l /dev/rfcomm0
```

You might have permission access issues. Make sure you're in the dialout group or go bonkers :

```
    sudo chmod 777 /dev/rfcomm0 && cu -l /dev/rfcomm0
```

