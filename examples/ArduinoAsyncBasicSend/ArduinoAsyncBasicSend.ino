#include "rockblock_9704.h"

/**
 * This sketch sends a "Hello World!" message via the RB9704.
 * 
 * A serial connection (on Serial1) will first be attempted, if successful a request to
 * que the message will be issued (note: this will fail if the RB9704 is not provisioned
 * for the specified topic). We then setup our 4 user defined callbacks to get message provisioning,
 * check if our queued messages have sent, check if we received any messages and finally check the
 * signal strength. The script will call rbPoll() every time it loops, it is important that this is done 
 * quite frequently (10ms in this example) as that function is responsible for listening to all the replies 
 * from the modem. Finally the script will end the connection gracefully once the message has been sent.
 * 
 * Requirements:
 * RB9704 needs to be provisioned for messaging topic 244.
 * RB9704 needs to be wired to the RX and TX pins of Serial1 (In this example we set those to be pins 21 & 22 respectively).
 * Have an open view of the sky where a good signal can be obtained.
 * 
*/

int messagesSent = 0;

void onMessageProvisioning(const jsprMessageProvisioning_t *messageProvisioning)
{
    if(messageProvisioning->provisioningSet == true)
    {
        Serial.print("Device is provisioned for ");
        Serial.print(messageProvisioning->topicCount);
        Serial.println(" topics");
        Serial.println("Provisioned topics:");
        for(int i = 0; i < messageProvisioning->topicCount; i++)
        {
            Serial.print("Topic name: ");
            Serial.print(messageProvisioning->provisioning[i].topicName);
            Serial.print(", Topic number: ");
            Serial.println(messageProvisioning->provisioning[i].topicId);
        }
    }
}

void onMoComplete(const uint16_t id, const rbMsgStatus_t status)
{
    Serial.print("MO Complete: ID = ");
    Serial.print(id);
    Serial.print(", Status = ");
    Serial.println(status);
    if(status == RB_MSG_STATUS_OK)
    {
        messagesSent += 1;
        Serial.print("Message Sent: ");
        Serial.println(messagesSent);
    }
}

void onMtComplete(const uint16_t id, const rbMsgStatus_t status)
{
    Serial.print("MT Complete: ID = ");
    Serial.print(id);
    Serial.print(", Status = ");
    Serial.println(status);
}

void onConstellationState(const jsprConstellationState_t *state)
{
        Serial.print("Current Signal: ");
        Serial.println(state->signalBars);
}

static rbCallbacks_t myCallbacks =
{
.messageProvisioning = onMessageProvisioning,
.moMessageComplete = onMoComplete,
.mtMessageComplete = onMtComplete,
.constellationState = onConstellationState
};

void setup() {
  Serial.begin(9600);
  Serial1.begin(230400, SERIAL_8N1, 21, 22);
  delay(1000);
  Serial.println("Starting\r\n");
  if(rbBegin("Serial1"))
  {
      delay(1000);
      //Register Callbacks
      rbRegisterCallbacks(&myCallbacks);
      Serial.println("Successfully started serial session with RB9704\r\n");
      //Queue and send MO
      const char *message = "Hello World!";
      if(rbSendMessageAsync(244, message, strlen(message)))
      {
          Serial.print("Sent MO: ");
          Serial.println(message);
      }
      else
      {
          Serial.println("Failed to queue message\r\n");
      }
  }
  else
  {
      Serial.println("Failed to begin the serial connection\r\n");
  }
}

void loop() {
    rbPoll();
    delay(10);

    if(messagesSent > 0)
    {
        //End serial connection
        if(rbEnd())
        {
            Serial.println("Ended connection successfully\r\n");
            Serial1.end();
            messagesSent = 0;
        }
        else
        {
            Serial.println("Failed to end connection\r\n");
        }
    }
}
