import argparse
from rockblock9704 import *
from time import sleep

#This example code showcases the asynchronous capabilities of this library by setting
#the necessary callbacks, queuing and sending a message, listening for any incoming message
#without blocking and finally quitting after receiving a successful message.
#
#At the start of the script we setup our 4 user defined callbacks to get message provisioning,
#check if our queued message has sent, check if we received any messages and finally check the
#signal strength. A serial connection will then be attempted on the selected port. The script will call
#poll() every time it loops, it is important that this is done quite frequently (10ms is reliable)
#as that function is responsible for listening to all the replies from the modem. A message will be
#queued at the start and should start sending if a good signal has been obtained. The callback for message
#provisioning should then return all the provisioned topics, followed by the onMoComplete callback when 
#the message has sent. The onConstellationState callback will be called anytime either the signal level or
#signal bars have changed although in this example we will only print the change in signal bars. Finally
#the script will constantly wait for an incoming message, by waiting for the onMtComplete callback to be called,
#then store and acknowledge it to clear space.
#
#Requirements:
#RB9704 needs to be provisioned for messaging topic 244 (RAW).
#Have an open view of the sky where a good signal can be obtained.

messages_sent = 0
messages_received = 0
received_new_message = False
current_signal = 0

def on_provision(messageProvisioning):
    if messageProvisioning["provisioningSet"]:
        print(f"\033[1;32mProvisioned for {messageProvisioning['topicCount']} topics\033[0m")
        for topic in messageProvisioning["provisioning"]:
            print(f"\033[1;32mTopic name: {topic['topicName']}, Topic number: {topic['topicId']}\033[0m")

def on_mo(id, status):
    global messages_sent
    print(f"\033[1;32mMO complete: ID={id}, status={status}\033[0m")
    if(status == 1):
        messages_sent += 1
        print(f"\033[1;33mMessage Sent: {messages_sent}\033[0m")

def on_mt(id, status):
    global received_new_message, messages_received
    print(f"\033[1;32mMT complete: ID={id}, status={status}\033[0m")
    if(status == 1):
        messages_received += 1
        print(f"\033[1;32mMessages received: {messages_received}\033[0m")
        received_new_message = True

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
        rb.set_message_provisioning_callback(message_provisioning=on_provision)
        rb.set_mo_message_complete_callback(mo_message_complete=on_mo)
        rb.set_mt_message_complete_callback(mt_message_complete=on_mt)
        rb.set_constellation_state_callback(constellation_state=on_signal)
        # Begin serial communication
        connected = rb.begin(args.device)

        if connected:

            # Get RockBlock information
            print("\033[1;34mBoard temperature: \t\033[0m", rb.get_board_temp())
            print("\033[1;34mSerial number: \t\033[0m", rb.get_serial_number())
            print("\033[1;34m9704 Modem IMEI: \t\033[0m", rb.get_imei())
            print("\033[1;34m9704 Modem ICCID: \t\033[0m", rb.get_iccid())
            messages_queued = 0
            messages_acknowledged = 0
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
                if received_new_message:
                    message = rb.receive_message_async()

                    if message is not None:
                        print(f"\033[1;32mReceived message: {message}\033[0m")
                        # Acknowledge the head of the queue
                        rb.acknowledge_receive_head_async()
                        messages_acknowledged += 1
                        print(f"\033[1;32mMessages acknowledged: {messages_acknowledged}\033[0m")

                if messages_acknowledged >= 1:
                    break

            if(rb.end()):
                print("Serial connection terminated successfully")

    else:
        print("Please specify a serial port")
        parser.print_help()
