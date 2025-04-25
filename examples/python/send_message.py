import argparse
from rockblock9704 import *
from time import sleep

#This script sends a "Hello, world!" message via the RB9704.
#
#A serial connection will first be attempted on the selected port, if successful 
#the device information will be printed and the signal will be polled and once obtained 
#a request to que the message will be issued (note: this will fail if the RB9704 is not 
#provisioned for the specified topic and will block until the message is fully transferred, 
#this might take a while if the signal is poor).
#
#Requirements:
#RB9704 needs to be provisioned for topic 315 (RED).
#Have an open view of the sky where a good signal can be obtained.

if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog='receive-message', description="Python example to send IMT message on RockBLOCK 9704.")
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

            # Send message with a custom 60s timeout
            sent = rb.send_message(b"Hello, world!", timeout=60)

            if sent:
                print("Message sent!")
            else:
                # Timed out
                print("Sending failed")

            # Send message to specific topic
            sent = rb.send_message(b"Hello, red topic!", topic=RockBlock9704.RED_TOPIC)

            if sent:
                print("Message sent!")
            else:
                # Timed out
                print("Sending failed")
    else:
        print("Please specify a serial port")
        parser.print_help()
