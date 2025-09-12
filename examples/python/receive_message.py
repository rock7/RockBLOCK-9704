import argparse
from rockblock9704 import *
from time import sleep

#This script receives a message via the RB9704.
#
#A serial connection will first be attempted on the selected port, if successful 
#the device information will be printed and the signal will be polled and once obtained 
#the script will listen for a message to come in. #not what I see happening.
#
#Requirements:
#Have an open view of the sky where a good signal can be obtained.

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

            # Get RockBlock information
            print("Board temperature: \t", rb.get_board_temp())
            print("Hardware version: \t", rb.get_hardware_version())
            print("9704 Modem IMEI: \t", rb.get_imei())

            # --- Extremely bad practice to wait for signal ---
            # while rb.get_signal() < 1:
            #     print("Waiting for signal...")
            #     sleep(2)
            #
            # print("Signal acquired!")

            # --- premature exit resulting in signal acquired but no MT ---
            # Check for messages
            # message = rb.receive_message()
            #
            # if message is not None:
            #     print("Received message: ", message)

            # --- new loop waiting for MT ---
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
