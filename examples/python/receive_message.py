import argparse
from rockblock9704 import *
from time import sleep

#This script receives a message via the RB9704.
#
#A serial connection will first be attempted on the selected port, if successful 
#the device information will be printed and script will begin listening for MT messages.
#
#Requirements:
#Have an open view of the sky where a good signal can be obtained.
#Have the RockBLOCK 9704 on an active plan.
#Send MT messages after running this script.

if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog='receive-message', description="Python example to receive IMT message on RockBLOCK 9704.")
    parser.add_argument("--device",
                        help="Serial port of RockBLOCK 9704")

    args = parser.parse_args()

    if (args.device):
        # Create RockBLOCK9704 instance
        rb = RockBlock9704()

        # Begin serial communication
        connected = rb.begin(args.device)

        if connected:
            sleep(0.1); #Wait at least 100ms before queueing a message the first time you run rbBegin after boot.
            # Get RockBLOCK information
            print("Board temperature: \t", rb.get_board_temp())
            print("Hardware version: \t", rb.get_hardware_version())
            print("9704 Modem IMEI: \t", rb.get_imei())
            print("9704 Serial Number: \t", rb.get_serial_number())

            # Begin listening for MT messages
            print("Listening for MT messages... Press Ctrl+C to stop.")
            try:
                while True:
                    message = rb.receive_message()
                    if message is not None:
                        print("Received message: ", message)
                    sleep(1)
            except KeyboardInterrupt:
                print("\nStopped by user.")
    else:
        print("Please specify a serial port")
        parser.print_help()
