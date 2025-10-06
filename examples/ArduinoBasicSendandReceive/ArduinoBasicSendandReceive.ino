#include "rockblock_9704.h"

bool ended = false;
char * mtBuffer = NULL;
int oldSignal;

/**
 * This sketch sends a "Hello World!" message via the RB9704, then attempts
 * to read a message if it gets sent back to the device.
 * 
 * A serial connection (on Serial1) will first be attempted, if successful a request to
 * que the message will be issued (note: this will fail if the RB9704 is not provisioned
 * for the specified topic and will block until the message is fully transferred, this might
 * take a while if the signal is poor). Once the message is sent the script will display
 * any change in signal and listen for a message to be sent back, once the message
 * has been received an attempt to shutdown the serial connection will be made.
 * 
 * Requirements:
 * RB9704 needs to be provisioned for cloudloop topic 244.
 * RB9704 needs to be wired to the TX and RX pins of Serial1.
 * Have an open view of the sky where a good signal can be obtained.
 * 
*/

void setup() {
  Serial.begin(9600);
  Serial1.begin(230400);
  delay(1000);
  if(rbBegin("Serial1"))
  {
      Serial.println("Successfully started serial session with RB9704\r\n");
      delay(100); //Wait at least 100ms before queueing a message the first time you run rbBegin after boot.
      //Queue and send MO
      const char *message = "Hello World!";
      if(rbSendMessageAny(244, message, strlen(message), 600))
      {
          Serial.print("Sent MO: ");
          Serial.println(message);
          //Start listening for MT
          int oldSignal = rbGetSignal();
          Serial.print("Current Signal: ");
          Serial.println(oldSignal);
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
  delay(1000);
}

void loop() {
  //Begin serial connection and initialise the modem
    delay(1000);
    if(!ended)
    {
      const size_t mtLength = rbReceiveMessage(&mtBuffer);
      if ((mtLength > 0) && (mtBuffer != NULL))
      {
          Serial.print("Received MT: ");
          Serial.println(mtBuffer);
          //End serial connection
          if(rbEnd())
          {
              Serial.println("Ended connection successfully\r\n");
              Serial1.end();
              ended = true;
          }
          else
          {
              Serial.println("Failed to end connection\r\n");           
          }
      }
      int newSignal = rbGetSignal();
      if(oldSignal != newSignal)
      {
          Serial.print("Current Signal: ");
          Serial.println(newSignal);
          oldSignal = newSignal;
      }
    }
}
