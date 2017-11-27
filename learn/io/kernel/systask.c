/**********************************************************
 * ********************************************************
 *                              systask.c
 *********************************************************
 *                              zhaokai, 2017
 **********************************************************/
#include "type.h"
#include "global.h"
#include "systask.h"
#include "assert.h"

/********************************************************
 *                              task_sys    
 **********************************************************
 *<ring 1> The task sys
 *********************************************************/
PUBLIC void task_sys()
{
    MESSAGE msg;

    while (1){
        send_recv(RECEIVE, ANY, &msg);
        int src = msg.source;

        switch (msg.type) {
        case GET_TICKS:
            msg.RETVAL = ticks;
            send_recv(SEND, src, &msg);
            break;
        default:
            panic("unknown msg type");
            break;
        }
    }
}