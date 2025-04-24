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
