#include <time.h>
#include <sys/stat.h>

#include "kermit_io.h"
#include "../third_party/ekermit/cdefs.h"
#include "../serial.h"

extern serialContext context;
static FILE * iFile = NULL;

int kermit_io_readpkt(struct k_data *k, unsigned char *p, int len)
{
    UCHAR receivedByte = 0;
    int packetLength = 0;
    short packetStarted = 0;
    UCHAR strippedByte;
    short ctrlCCount = 0;
    unsigned char *ptr = p;

    if (context.serialRead == NULL)
    {
        return X_RC_ERROR;
    }

    while (1)
    {
        if (context.serialRead(&receivedByte, 1) <= 0)
        {
            return X_RC_OK; // Timeout case
        }

        // Strip parity if needed
        strippedByte = (k->parity) ? receivedByte & 0x7F : receivedByte & 0xFF;

        // Handle three consecutive ^C's for remote quit
        if (k->remote && strippedByte == (UCHAR)3)
        {
            ctrlCCount++;
            if (ctrlCCount > 2)
            {
                return X_RC_ERROR;
            }
            continue;
        }
        else
        {
            ctrlCCount = 0;
        }

        // Wait for start of packet
        if (!packetStarted)
        {
            if (strippedByte == k->r_soh)
            {
                packetStarted = 1;
            }
            continue;
        }

        // Check for packet terminator
        if (strippedByte == k->r_eom || strippedByte == '\012')
        {
            /*
            printf("Kermit Rx: ");
            for (size_t i = 0; i < packetLength; i++)
            {
                printf("%c", ptr[i]);
            }
            printf("\n");
            */
            
            return packetLength;
        }

        // Store packet content, ensuring we don't exceed max length
        if (packetLength > k->r_maxlen)
        {
            return X_RC_OK;
        }

        *p++ = receivedByte & 0xFF;
        packetLength++;
    }

    return X_RC_ERROR; // Should never reach here, but included for safety
}

int kermit_io_tx_data(struct k_data *k, unsigned char *p, int length)
{
    (void)k;

    if (context.serialWrite == NULL)
    {
        return SUCCESS;
    }

    if (context.serialWrite(p, length) < 0)
    {
        return X_RC_ERROR;
    }

    /*
    printf("Kermit Tx: ");
    for (size_t i = 0; i < length; i++)
    {
        //printf("%02X ", p[i]);
        printf("%c", p[i]);
    }
    printf("\n");
    */
    

    p += length;
    length = 0;

    return SUCCESS;
}


int kermit_io_inchk(struct k_data * k)
{
    (void)k;

    if (context.serialPeek != NULL)
    {
        return context.serialPeek();
    }

    return X_RC_ERROR;
}

long kermit_io_filesize(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        return -1; // Error opening file
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fclose(file);

    return fileSize;
}


int kermit_io_openfile(struct k_data *k, unsigned char *filename, int mode)
{
    int result = X_RC_ERROR;

    switch (mode)
    {
        /* Read Mode */
        case 1:
            iFile = fopen((const char *)(filename), "rb");
            if (iFile != NULL)
            {
                k->s_first = 1;         /* Set up for getkpt */
                k->zinbuf[0] = '\0';    /* Initialize buffer */
                k->zinptr = k->zinbuf;  /* Set up buffer pointer */
                k->zincnt = 0;          /* Reset count */

                result = SUCCESS;
            }
            else
            {
            }
            break;

        /* Write and append not used in this implementation */
        default:
            break;
    }

    return result;
}

int kermit_io_readfile(struct k_data *k)
{
    if (!k->zinptr)
    {
        return X_RC_ERROR;
    }

    /* Nothing in buffer - must refill */
    if (k->zincnt < 1)
    {
        /* Binary mode - read raw buffers */
        if (k->binary)
        {
            k->dummy = 0;
            k->zincnt = fread(k->zinbuf, 1, k->zinlen, iFile);
        }
        else
        {
            /* Text mode needs K_LF/CRLF handling */
            int currentChar; /* Current character */
            for (k->zincnt = 0; (k->zincnt < (k->zinlen - 2)); (k->zincnt)++)
            {
                if ((currentChar = getc(iFile)) == EOF)
                {
                    break;
                }

                /* Handle newlines */
                if (currentChar == '\n')
                {
                    k->zinbuf[(k->zincnt)++] = '\r'; /* Insert K_CR */
                }
                k->zinbuf[k->zincnt] = currentChar;
            }
            k->zinbuf[k->zincnt] = '\0';
        }
        k->zinbuf[k->zincnt] = '\0'; /* Null terminate */

        /* Check for EOF */
        if (k->zincnt == 0)
        {
            return -1;
        }

        k->zinptr = k->zinbuf; /* Reset pointer */
    }

    (k->zincnt)--; /* Decrease count */

    return (*(k->zinptr)++ & 0xFF);
}

int kermit_io_closefile(struct k_data *k, unsigned char character, int mode)
{
    (void)character;
    (void)k;
    int result = X_RC_ERROR;

    switch (mode)
    {
        /* Closing input file */
        case 1:
            /* If file is open */
            if (iFile != NULL)
            {
                if (fclose(iFile) > 0)
                {
                    result = SUCCESS;
                }
            }
            break;

        /* Writing is not supported */
        default:
            break;
    }

    return result;
}


int kermit_io_init_string(void)
{
    int rVal = X_RC_ERROR;
    const char kermitInitString[] = "kermit -ir\r";

    if (context.serialWrite != NULL)
    {
        if (context.serialWrite(kermitInitString, 11) >= 0)
        {
            rVal = SUCCESS;
        }
    }

    return rVal;
}

