import argparse
from rockblock9704 import *
from time import sleep

if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog='receive-message', description="Python example to receive IMT message on RockBLOCK 9704.")
    parser.add_argument("--device",
                        help="Serial port of RockBLOCK 9704")

    args = parser.parse_args()

    if (args.device):
        # Create RockBlock9704 instance
        rb = RockBlock9704()

        # Create a dictionary with your custom GPIO layout
        gpioInfo = {
            "powerEnable": ("/dev/gpiochip0", 24), #Drive pin 24 low
            "iridiumEnable": ("/dev/gpiochip0", 16), #Drive pin 16 high
            "booted": ("/dev/gpiochip0", 23), #wait for pin 23 to be driven high
        }

        # Begin serial communication
        connected = rb.beginGpio(args.device, gpioInfo, 60)

        if connected:

            # Get RockBlock information
            print("9704 Modem IMEI: \t", rb.get_imei())

            # End Connection
            end = rb.endGpio(gpioInfo)