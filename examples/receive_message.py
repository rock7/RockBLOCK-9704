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

        # Begin serial communication
        connected = rb.begin(args.device)

        if connected:

            # Get RockBlock information
            print("Board temperature: \t", rb.get_board_temp())
            print("Hardware version: \t", rb.get_hardware_version())
            print("9704 Modem IMEI: \t", rb.get_imei())

            # Wait for signal
            while rb.get_signal() < 1:
                print("Waiting for signal...")
                sleep(2)

            print("Signal acquired!")

            # Check for messages
            message = rb.receive_message()

            if message is not None:
                print("Received message: ", message)
    else:
        print("Please specify a serial port")
        parser.print_help()
