RockBLOCK 9704
==============

This library provides an easy to use Python interface for the **RockBLOCK 9704** serial tools, originally written in C.

The source for this project can be found on https://github.com/rock7/RockBLOCK-9704.

For more information on the RockBLOCK 9704, see https://www.groundcontrol.com/product/rockblock-9704/

Requirements
============
- Python 3.8+
- A RockBLOCK 9704

Installation
============

The RockBLOCK 9704 library can be installed from PyPI::

    pip install rockblock9704

Connecting to your RockBLOCK
============================

Once your rockBLOCK has been connected to your device via USB you will need to find its local address.

Linux / macOS
-------------

List all local devices::

    ls /dev/tty*

The name will vary depending on machine but will generally include USB in the name (e.g. ``/dev/ttyUSB0`` or ``/dev/tty.usbserial-FTH9I1S5``)

Windows
----------

List COM devices::

    mode

When connected to my machine the port was ``COM6`` but this will vary, it can be helpful to run the command before connecting to see what port becomes active.

Begin the Serial Connection
---------------------------

Once you have the address of your device, create an instance of RockBlock9704 and call the begin method::

    from rockblock9704 import RockBlock9704

    rb = RockBlock9704()
    rb.begin(<device address>)
