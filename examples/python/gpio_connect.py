import argparse
from rockblock9704 import *

#This script uses GPIO pins rather than a USB-C cable to initialise the
#RB9704.
#
#The function to do this requires a config to first be defined which
#contains the chip and pin number information of the 3 GPIO pins attached to
#power enable, iridium enable and booted. This script will attempt to drive
#power enable to low, then iridium enable to high which essentially turns it on.
#It will then wait for the booted pin to be driven high, then attempt to make
#a serial connection, if successful it will get the IMEI then start the shutdown
#process (drive iridium enable low the power enable high).
#
#Requirements:
#Make the minimum required connections.
#
# Wiring example:
#
#GND -> GND
#VIN -> VIN
#TX -> RX
#RX -> TX
#P_EN (6) -> Any free GPIO (eg 24)
#I_EN (3) -> Any free GPIO (eg 16)
#I_BTD (7) -> Any free GPIO (eg 23)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog='receive-message', description="Python example to receive IMT message on RockBLOCK 9704.")
    parser.add_argument("--device",
                        help="Serial port of RockBLOCK 9704")

    args = parser.parse_args()

    if (args.device):
        # Create RockBlock9704 instance
        rb = RockBlock9704()

        # Create a config with your custom GPIO layout
        # Drive pin 24 low
        # Drive pin 16 high
        # wait for pin 23 to be driven high
        config = GpioConfig(24, 16, 23, default_card="/dev/gpiochip0")

        # Begin serial communication
        connected = rb.begin_gpio(args.device, config, 60)

        if connected:

            # Get RockBlock information
            print("9704 Modem IMEI: \t", rb.get_imei())

            # End Connection
            end = rb.end_gpio(config)
