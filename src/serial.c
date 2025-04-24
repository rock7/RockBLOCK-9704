#include "serial.h"

//Serial Variables
int serialConnection;
enum serialState serialState = CLOSED;
char* serialPort;
int serialBaud;
serialContext context =
{
    NULL, // serialInit
    NULL, // serialDeInit
    NULL, // serialRead
    NULL, // serialWrite
    NULL, // serialPeek
};