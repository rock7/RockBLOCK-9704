from rockblock9704 import *

if __name__ == '__main__':

    # Create RockBlock9704 instance
    rb = RockBlock9704()

    # Begin serial communication
    connected = rb.beginHat(60)

    if connected:

        # Get RockBlock information
        print("9704 Modem IMEI: \t", rb.get_imei())

        # End Connection
        end = rb.endHat()