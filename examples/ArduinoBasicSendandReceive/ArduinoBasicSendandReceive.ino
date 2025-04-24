#include "rockblock_9704.h"

bool ended = false;
char * mtBuffer = NULL;
int oldSignal;

void setup() {
  Serial.begin(9600);
  Serial1.begin(230400);
  delay(1000);
  if(rbBegin("Serial1"))
  {
      Serial.println("Sucessfully started serial session with RB9704\r\n");
      //Queue and send MO
      const char *message = "Reflected Hello World!";
      if(sendMessageAny(80, message, strlen(message)))
      {
          Serial.print("Sent MO: ");
          Serial.println(message);
          //Start listening for MT
          int oldSignal = getSignal();
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
      const size_t mtLength = receiveMessage(&mtBuffer);
      if ((mtLength > 0) && (mtBuffer != NULL))
      {
          Serial.print("Received MT: ");
          Serial.println(mtBuffer);
          //End serial connection
          if(rbEnd())
          {
              Serial.println("Ended connection sucessfully\r\n");
              Serial1.end();
              ended = true;
          }
          else
          {
              Serial.println("Failed to end connection\r\n");           
          }
      }
      int newSignal = getSignal();
      if(oldSignal != newSignal)
      {
          Serial.print("Current Signal: ");
          Serial.println(newSignal);
          oldSignal = newSignal;
      }
    }
}
