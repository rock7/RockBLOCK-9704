RockBLOCK 9704
##############

- The official RockBLOCK 9704 Python Library made by `Ground Control <https://www.groundcontrol.com>`_.

- This library provides an easy to use Python interface for the **RockBLOCK 9704** serial tools, originally written in C.

- The source for this project can be found on https://github.com/rock7/RockBLOCK-9704.

- All Python examples can be found `here <https://github.com/rock7/RockBLOCK-9704/tree/master/examples/python>`_

Requirements
============

- Python 3.10+

- A `RockBLOCK 9704 <https://www.groundcontrol.com/product/rockblock-9704/>`_

Installation
============

The RockBLOCK 9704 library can be installed from PyPI::

    pip install rockblock9704

Connecting to your RockBLOCK
############################

Communication with the RockBLOCK is conducted via serial, either with USB serial or using the GPIO pins on your device.

Connecting via USB
==================

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

Once you have the address of your device, create an instance of RockBlock9704 and call the begin method.

.. code-block:: python

    from rockblock9704 import *

    rb = RockBlock9704()
    connected = rb.begin(<device address>)

    if connected:
        print("Successful connection!")

The ``begin()`` function must be called before any other RockBLOCK functions can be used. You should always check that ``begin()`` has returned true to
indicate a successful connection, otherwise further interaction with the RockBLOCK will fail.

There are several functions that allow monitoring of RockBLOCK and modem information.

.. code-block:: python

    # Board Information
    hardware_version = rb.get_hardware_version()
    print("Hardware version: {}".format(hardware_version))
    firmware_version = rb.get_firmware_version()
    print("Firmware version: {}".format(firmware_version))
    board_temp = rb.get_board_temp()
    print("Board temperature: {}°C".format(board_temp))

    # Identifying Information
    serial_number = rb.get_serial_number()
    print("Serial number: {}".format(serial_number))
    imei = rb.get_imei()
    print("IMEI: {}".format(imei))
    iccid = rb.get_iccid()
    print("ICCID: {}".format(iccid))

    card_present = rb.get_card_present()
    sim_connected = rb.get_sim_connected()
    if card_present and sim_connected:
        print("Sim card valid")

Connecting via GPIO
===================

To connect to your RockBLOCK via the GPIO pins on your device, you first create a ``GpioConfig`` object in which you specify the GPIO pins to use. In the
example a single GPIO card is provided for all pins but individual cards can be specified if all pins are not shared.

.. code-block:: python

    from rockblock9704 import *

    # Set power enabled to pin 1, iridium enabled to pin 2 and booted to pin 3. Default card is provided.
    gpio_config = GpioConfig(1, 2, 3, default_card="/dev/gpiochip0")

See the `GPIO connect example <https://github.com/rock7/RockBLOCK-9704/tree/master/examples/python/gpio_connect.py>`_ for more help.

Begin the Serial Connection
---------------------------

Once you have created your ``GpioConfig`` object, you can call ``begin_gpio()`` to initialise the connection. In the example ``/dev/tty0`` is specified as
the port to bind the connection to once successfully begun. The timeout provided is in seconds.

.. code-block:: python

    from rockblock9704 import *

    # Create your RockBlock object
    rb = RockBlock9704()

    # Set power enabled to pin 1, iridium enabled to pin 2 and booted to pin 3. Default card is provided.
    gpio_config = GpioConfig(1, 2, 3, default_card="/dev/gpiochip0")

    # Initialise the serial connection via GPIO
    connected = rb.begin_gpio("/dev/tty0", gpio_config, timeout=60)

    if connected:
        print("Successful connection!")

Sending and Receiving
#####################

After a serial connection has been successfully established, you can start sending and receiving messages with your RockBLOCK.

Send a Message
==============

Data can be transmitted from the RockBLOCK by using the ``send_message()`` function. The function will attempt to send your message until it is successful
and returns true, or the timout expires (default 30 seconds) in which case it will return false.

.. code-block:: python

    # Always check if begin has returned successfully
    if connected:

        # Sending message
        sent = rb.send_message(b"Hello, world!")

        if sent:
            print("Message sent!")
        else:
            # If the timeout expires
            print("Sending failed")

A custom timeout can be set if you want the ``send_message()`` function to attempt sending for greater or less than 30 seconds.

.. code-block:: python

    # Always check if begin has returned successfully
    if connected:

        # Attempt to send for 60 seconds before failing
        sent = rb.send_message(b"Hello, world!", timeout=60)

        if sent:
            print("Message sent!")
        else:
            # could not send within 60 seconds
            print("Sending failed")

By default the message will be sent to the raw Cloudloop topic (244) but any topic can be specified by calling ``send_message(<message>, topic=<topic>)``.

.. code-block:: python

    # Sending message to topic 999
    rb.send_message(b"Hello, world!", topic=999)

    # Sending messages to the Cloudloop colour topics
    rb.send_message(b"Hello, Purple!", topic=rb.PURPLE_TOPIC)
    rb.send_message(b"Hello, Pink!", topic=rb.PINK_TOPIC)
    rb.send_message(b"Hello, Red!", topic=rb.RED_TOPIC)
    rb.send_message(b"Hello, Orange!", topic=rb.ORANGE_TOPIC)
    rb.send_message(b"Hello, Yellow!", topic=rb.YELLOW_TOPIC)

See the `send message example <https://github.com/rock7/RockBLOCK-9704/tree/master/examples/python/send_message.py>`_ for more help.

Receive a Message
=================

Data sent to the RockBLOCK can be retrieved using the ``receive_message()`` function, this will return ``None`` if no messages are available.

.. code-block:: python

    # Always check if begin has returned successfully
    if connected:

        # Check for a message
        message = rb.receive_message()

        if message is not None:
            print("Message Received!")
            print(message)
        else:
            print("No messages")

A specific topic can be provided to fetch only messages sent to that topic

.. code-block:: python

    message = rb.receive_message(topic=rb.ORANGE_TOPIC)

See the `receive message example <https://github.com/rock7/RockBLOCK-9704/tree/master/examples/python/receive_message.py>`_ for more help.

Send a Message to the RockBLOCK (Cloudloop)
===========================================

Cloudloop Data users can use the Cloudloop API to send messages direct to their RockBLOCK(s) very easily. All API calls require a user token, guidance for
acquiring a token as well as API usage is found in the `Cloudloop Data Knowledgebase`__.

.. _Knowledge: https://knowledge.cloudloop.com/docs/api/authentication

__ Knowledge_

.. code-block:: python

    import requests

    USER_TOKEN = "<token>"
    THING_ID = "<thing-id>"
    MESSAGE = "SGVsbG8sIHdvcmxkIQ=="  # Base64 'Hello, world!'
    TOPIC = "IMT_TOPIC_RAW"

    payload = {"token": USER_TOKEN, "thing": THING_ID, "message": MESSAGE, "topic": TOPIC}

    requests.post("https://api.cloudloop.com/Data/DoSendImtMessage", json=payload)

Alternatively if you have more than one RockBLOCK, they can be put in a `Thing Group`__ and a message sent to all with one request.

.. _Group: https://knowledge.cloudloop.com/docs/concepts/thing-group

__ Group_

.. code-block:: python

    import requests

    USER_TOKEN = "<token>"
    GROUP_ID = "<group-id>"
    MESSAGE = "SGVsbG8sIHdvcmxkIQ=="  # Base64 'Hello, world!'
    TOPIC = "IMT_TOPIC_RAW"

    payload = {"token": USER_TOKEN, "thingGroup": GROUP_ID, "message": MESSAGE, "topic": TOPIC}

    requests.post("https://api.cloudloop.com/Data/DoSendImtMessageForThingGroup", json=payload)

Check RockBLOCK Signal
======================

You can check the signal strength of the RockBLOCK to the Iridium network using the ``get_signal()`` function. Signal strength can vary greatly depending
where the satellites are in their orbit, do not be concerned if sometimes the signal goes from 5, max signal, to 0, no signal frequently.

.. code-block:: python

    if connected:

        # Check signal (0 - 5)
        signal = rb.get_signal()

        if signal > 0:
            print("We have an active IMT connection")
        else:
            print("Unable to contact satellites")
