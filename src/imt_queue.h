#ifndef IMT_QUEUE_H
#define IMT_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * @def IMT_QUEUE_SIZE
 * @brief Number of messages allowed in the internal message queue.
 */
#ifndef IMT_QUEUE_SIZE
    #define IMT_QUEUE_SIZE 1U
#endif

/**
 * @def IMT_CRC_SIZE
 * @brief Number of bytes reserved for CRC at the end of the payload.
 */
#define IMT_CRC_SIZE 2U

/**
 * @def IMT_PAYLOAD_SIZE
 * @brief Maximum payload size of a single message with the CRC added on.
 */
#ifdef ARDUINO
    #ifndef IMT_PAYLOAD_SIZE
        #define IMT_PAYLOAD_SIZE 5000U + IMT_CRC_SIZE
    #endif
#else
    #ifndef IMT_PAYLOAD_SIZE
        #define IMT_PAYLOAD_SIZE 100000U + IMT_CRC_SIZE
    #endif
#endif

/**
 * @struct imt_t
 * @brief Represents a single message in the queue.
 */
typedef struct
{
    uint16_t id;            /**< Message ID assigned by the modem */
    uint8_t * buffer;       /**< Pointer to the buffer where the message payload is stored */
    size_t length;          /**< Length of the message payload */
    uint16_t topic;         /**< Message topic ID */
    bool readyToProcess;    /**< Used to determine if the message is ready for processing */
    bool ready;             /**< Used to determine if the message is fully processed */
} imt_t;

/**
 * @struct imt_queue_t
 * @brief Circular buffer structure to hold queued messages.
 */
typedef struct
{
    imt_t messages[IMT_QUEUE_SIZE]; /**< Array of queued messages */
    uint16_t count;                 /**< Number of messages in queue */
    uint16_t head;                  /**< Index of the message at the front */
    uint16_t tail;                  /**< Index of the next free space for a message */
    uint16_t maxLength;             /**< Maximum length of the queue */
} imt_queue_t;

/**
 * @brief Add an outgoing mobile-originated (MO) message to the queue.
 * 
 * @param topic Message topic ID.
 * @param data Pointer to the message payload.
 * @param length Message payload length in bytes.
 * @return Bool indicating success or failure to add the message to the queue.
 */
bool imtQueueMoAdd(const uint16_t topic, const char * data, const size_t length);

/**
 * @brief Add an incoming mobile-terminated (MT) message to the queue.
 * 
 * @param topic Message topic ID.
 * @param id Unique identifier of the message assigned by the modem.
 * @param length Message payload length in bytes.
 * @return Bool indicating success or failure to add the message to the queue.
 */
bool imtQueueMtAdd(const uint16_t topic, const uint16_t id, const size_t length);

/**
 * @brief Remove an outgoing mobile-originated (MO) message from the queue.
 * 
 * @return Bool indicating success or failure to remove the message from the queue.
 */
bool imtQueueMoRemove(void);

/**
 * @brief Remove an outgoing mobile-terminated (MT) message from the queue.
 * 
 * @return Bool indicating success or failure to remove the message from the queue.
 */
bool imtQueueMtRemove(void);

/**
 * @brief Get the address of the head of the MO queue.
 * 
 * @return Pointer to the next message in the queue, NULL if queue is empty.
 */
imt_t * imtQueueMoGetFirst(void);

/**
 * @brief Get the address of the head of the MT queue.
 * 
 * @return Pointer to the next message in the queue, NULL if queue is empty.
 */
imt_t * imtQueueMtGetFirst(void);

/**
 * @brief Get the address of the latest message in the MT queue.
 * 
 * @return Pointer to the last message in the queue, NULL if queue is empty.
 */
imt_t * imtQueueMtGetLast(void);

/**
 * @brief Lock or unlock the MT queue to prevent messages from getting discarded if full.
 * 
 * @param lock Called with true to lock the queue and false to unlock the queue.
 * 
 * @note This is set to false by default.
 */
void imtQueueMtLock(bool lock);

/**
 * @brief Lock or unlock the MO queue to prevent messages from getting discarded if full.
 * 
 * @param lock Called with true to lock the queue and false to unlock the queue.
 * 
 * @note This is set to false by default.
 */
void imtQueueMoLock(bool lock);

/**
 * @brief Used to initialise both outgoing and incoming queues.
 * 
 */
void imtQueueInit (void);

/**
 * @brief Inline function used to get the current size of the selected queue.
 * 
 * @param queue Pointer to the selected queue.
 * @return size_t of the current queue size.
 */
inline size_t imtQueueSize(imt_queue_t * queue)
{
    return queue->count;
}

#ifdef __cplusplus
}
#endif

#endif