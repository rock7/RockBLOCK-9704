import argparse
from rockblock9704 import *
from time import sleep

messagesSent = 0
receivedNewMessage = False
current_signal = 0

def on_provision(messageProvisioning):
    if messageProvisioning["provisioningSet"]:
        print(f"\033[1;32mProvisioned for {messageProvisioning['topicCount']} topics\033[0m")
        for topic in messageProvisioning["provisioning"]:
            print(f"\033[1;32mTopic name: {topic['topicName']}, Topic number: {topic['topicId']}\033[0m")

def on_mo(id, status):
    global messagesSent
    print(f"\033[1;32mMO complete: ID={id}, status={status}\033[0m")
    if(status == 1):
        messagesSent += 1
        print(f"\033[1;33mMessage Sent: {messagesSent}\033[0m")

def on_mt(id, status):
    global receivedNewMessage
    print(f"\033[1;32mMT complete: ID={id}, status={status}\033[0m")
    if(status == 1):
        receivedNewMessage = True

def on_signal(state):
    global current_signal
    if state["signalBars"] != current_signal:
        print(f"\033[1;34mCurrent Signal: {state['signalBars']}\033[0m")
        current_signal = state["signalBars"]

if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog='receive-message', description="Python example to send IMT message on RockBLOCK 9704.")
    parser.add_argument("--device",
                        help="Serial port of RockBLOCK 9704")

    args = parser.parse_args()

    if (args.device):

        # Create RockBlock9704 instance
        rb = RockBlock9704()
        rb.register_callbacks(
        #message_provisioning=on_provision,
        mo_message_complete=on_mo,
        mt_message_complete=on_mt
        #constellation_state=on_signal
        )
        # Begin serial communication
        connected = rb.begin(args.device)

        if connected:

            # Get RockBlock information
            print("\033[1;34mBoard temperature: \t\033[0m", rb.get_board_temp())
            print("\033[1;34mSerial number: \t\033[0m", rb.get_serial_number())
            print("\033[1;34m9704 Modem IMEI: \t\033[0m", rb.get_imei())
            print("\033[1;34m9704 Modem ICCID: \t\033[0m", rb.get_iccid())
            messages_queued = 0
            messages_received = 0
            while True:
                rb.poll()
                if(messages_queued == 0):
                    # Send message with a custom 60s timeout
                    payload = b"Hello, world!"
                    queue = rb.send_message_async(payload, 244)

                    if queue:
                        messages_queued += 1
                        print(f"\033[1;33mMessages queued: {messages_queued}\033[0m")
                    else:
                        # Timed out
                        sleep(0.1)
                        print("\033[1;31mQueueing failed\033[0m")

                # Check for messages
                if receivedNewMessage:
                    message = rb.receive_message_async()

                    if message is not None:
                        print(f"\033[1;32mReceived message: {message}\033[0m")
                        # Acknowledge the head of the queue
                        rb.acknowledge_receive_head_async()
                        messages_received += 1
                        print(f"\033[1;32mMessages received: {messages_received}\033[0m")

                if messages_received >= 5:
                    break

            if(rb.end()):
                print("Serial connection terminated successfully")

    else:
        print("Please specify a serial port")
        parser.print_help()
