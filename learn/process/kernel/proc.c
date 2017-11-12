#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "global.h"
#include "string.h"

PUBLIC int  sys_get_ticks(){
    return ticks;
}

/*选出优先最大的而且ticks 不为0的进程 */
PUBLIC void schedule()
{
        PROCESS *p;
        int max_ticks = 0;

        while (!max_ticks){
            for (p = proc_table; p < proc_table + NR_TASKS; p ++){
                if (p->ticks > max_ticks){
                    max_ticks = p->ticks;
                    p_proc_ready = p;
                }
            }

            // 如果所有的进程的时间都为 0 的话，那么我们重置所有的ticks 
            if (!max_ticks){
                for (p = proc_table; p < proc_table + NR_TASKS; p++){
                    p->ticks = p->priority;
                }
            }
        } 
        
}