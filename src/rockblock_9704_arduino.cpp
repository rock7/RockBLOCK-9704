#ifdef ARDUINO
#include "rockblock_9704.h"
#include "imt_queue.h"

extern int messageReference;
extern serialContext context;
extern enum serialState serialState;

bool rbBegin(Stream &port)
{
    bool began = false;
    if(SERIAL_CONTEXT_SETUP_FUNC(port, RB9704_BAUD))
    {
        if(context.serialInit != NULL)
        {
            if(context.serialInit())
            {
                serialState = OPEN;
                if(setApi())
                {
                    if(setSim())
                    {
                        if(setState())
                        {
                            imtQueueInit(); //initialise (clean) the queue
                            began = true;
                        }
                    }
                }
            }
        }
    }
    return began;
}

#endif