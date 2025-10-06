#include "serial.h"

//Serial Variables
int serialConnection;
enum serialState serialState = CLOSED;
serialContext context =
{
    NULL, // serialInit
    NULL, // serialDeInit
    NULL, // serialRead
    NULL, // serialWrite
    NULL, // serialPeek
    "",
    230400
};