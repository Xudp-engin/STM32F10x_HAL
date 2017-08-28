/*--------------------------------------------------------------------------
 * Filename:    queue.h
 *
 * Description: Generic circular queue macros
 *
 * Notes
 *
 * The queue macros are safe when used in producer/consumer fashion.
 * User must check queue full/empty conditions prior to enqueuing/
 * dequeueing.
 *
 * Example Usage
 *
 * #include "queue.h"
 *
 * #define MY_Q_SIZE 10
 *
 * struct
 * {
 *     queue_hdr_t hdr;                             // must be named "hdr"
 *     my_type_t     items[MY_Q_SIZE+1];            // must be named "items", 1 space wasted
 * } my_q;
 *
 * my_type_t an_item;
 *
 * QUEUE_INIT(my_q);
 *
 * if (!QUEUE_FULL(q))
 * {
 *    QUEUE_PUT(my_q,an_item);
 * }
 *
 * if (!QUEUE_EMPTY(q))
 * {
 *    QUEUE_GET(my_q,an_item);
 * }
 *
--------------------------------------------------------------------------
 */
#ifndef __QUEUE_H__
#define __QUEUE_H__

/*-------------------------------- Includes
--------------------------------*/
#include "CommonType.h."

/*---------------------------- Defines & Types
-----------------------------*/
#define QUEUE_INIT(q)           \
  q.hdr.front = q.hdr.rear = 0; \
  q.hdr.size =  sizeof(q.items) / sizeof(q.items[0]);

#define QUEUE_DYN_INIT(q)           \
  q.hdr.front = q.hdr.rear = 0; \
  q.hdr.size =  q.length;

#define QUEUE_PUT(q,item)                     \
  q.items[q.hdr.rear] = item;               \
  q.hdr.rear = (q.hdr.rear+1) % q.hdr.size;

#define QUEUE_GET(q,item)                         \
  item = q.items[q.hdr.front];                  \
  q.hdr.front = (q.hdr.front + 1) % q.hdr.size;

#define QUEUE_FRONT(q,item) \
  item = q.items[q.hdr.front]

#define QUEUE_EMPTY(q) (q.hdr.front == q.hdr.rear)
#define QUEUE_FULL(q)  ((q.hdr.rear + 1) % q.hdr.size == q.hdr.front)


/* Private - do not access directly */
typedef struct
{
    U32 front;
    U32 rear;
    U32 size;
} queue_hdr_t;

/*---------------------------- Global Variables
----------------------------*/

/*--------------------------- Function Prototypes
--------------------------*/

#endif
