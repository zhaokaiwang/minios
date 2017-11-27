/*==============================================================================================
                                                                        proc.c
============================================================================================
                                                                        zhaokai, 2017
*/
#include "type.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
#include "assert.h"
#include "printf.h"

PRIVATE  int deadlock(int src, int dest);
PRIVATE void block (struct proc* p);
PRIVATE void unblock(struct proc* p);
PRIVATE int msg_receive(struct proc *current, int src, MESSAGE * m);
PRIVATE int msg_send(struct proc* current, int dest, MESSAGE* m);

/*选出优先最大的而且ticks 不为0的进程 */
PUBLIC void schedule()
{
        PROCESS *p;
        int max_ticks = 0;

        while (!max_ticks){
            for (p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p ++){
                if (p->p_flags == 0)
                    if (p->ticks > max_ticks){
                        max_ticks = p->ticks;
                        p_proc_ready = p;
                    }
            }

            // 如果所有的进程的时间都为 0 的话，那么我们重置所有的ticks 
            if (!max_ticks){
                for (p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++){
                    if (p->p_flags == 0)
                        p->ticks = p->priority;
                }
            }
        } 
        
}

/****************************************************************************
 *                      ldt_seg_linear
 * *************************************************************************
 * @para  进程指针
 * @para  index
 * 返回段地址
 *************************************************************************/
PUBLIC u32 ldt_seg_linear(struct proc *p, int index)
{
    struct descriptor* d = &p->ldts[index];

    return d->base_high << 24 | d->base_mid << 16 | d->base_low;
}


/****************************************************************************
 *                      va2la
 * *************************************************************************
 * @para  进程pid
 * @para  虚拟地址
 * 返回线性地址
 *************************************************************************/
PUBLIC void* va2la(int pid, void* va)
{
    struct proc* p = &proc_table[pid];

    u32 seg_base = ldt_seg_linear(p, INDEX_LDT_RW);
    u32 la = seg_base + (u32) va;

    if (pid < NR_TASKS + NR_PROCS){
        assert(la == (u32)va);
    }

    return (void*) la;
}

/****************************************************************************
 *                      send_recv
 * *************************************************************************
 * <ring 1-3> 进程间通信的接口
 * 
 * @para  表示发送还是接受还是都有
 * @para  根据第一个参数决定是发送还是接收
 * @para m  消息
 *************************************************************************/
PUBLIC int send_recv(int function, int src_dest, MESSAGE* msg)
{
    int ret = 0;

    if (function == RECEIVE)
        reset_msg(msg);

    switch(function){
    case BOTH:
        ret = sendrec(SEND, src_dest, msg);
        if (ret == 0)
            ret = sendrec(RECEIVE, src_dest, msg);
        break;
    case SEND:
    case RECEIVE:
        ret = sendrec(function, src_dest, msg);
        break;
    default:
        assert((function == BOTH)|| (function == SEND)
                 || (function == RECEIVE));
    }
    return ret;
}


/****************************************************************************
 *                      sys_sendrve
 * *************************************************************************
 * <ring 0> syscall sendrec
 * @para  表示发送还是接受
 * @para  根据第一个参数决定是发送还是接收
 * @para m  消息
 * @para p  调用进程
 *************************************************************************/
PUBLIC int sys_sendrec(int function, int src_dest, MESSAGE* m, struct proc* p)
{
        assert(k_reenter == 0);
        assert((src_dest >= 0 && src_dest < NR_TASKS + NR_PROCS) ||
            src_dest == ANY || src_dest == INTERRUPT );
        
        int ret = 0;
        int caller = proc2pid(p);
        MESSAGE* mla = (MESSAGE*)va2la(caller, m);
        mla->source = caller;

        assert(mla->source != src_dest);

        /**
         * There is some three message type: BOTH. However, it is not 
         */
        if (function == SEND) {
            ret = msg_send(p, src_dest, m);
            if (ret != 0)
                return ret;
        } else if (function == RECEIVE){
            ret = msg_receive(p, src_dest, m);
            if (ret != 0)
                return ret;
        } else {
            panic("{sys_sendrev} invalid function: " 
                  "%d (SEND:%d, RECEIVE:%d).", function, SEND, RECEIVE);
        }
        
        return 0;
    }
    

/*********************************************************************
 *                              msg_send
 *********************************************************************
 **<Ring 0> 发送消息到目的进程，如果目的进程正准备接受这个消息，接受并且解除阻塞状态
 * 如果目的地址还没有准备好接受，我就将他进入阻塞状态，直到消息被读取
 * @param current 发送者
 * @param dest    接受者
 * @param m       消息 
 * 
 * @return Zero if success.
 ************************************************************************/
PRIVATE int msg_send(struct proc* current, int dest, MESSAGE* m)
{
    struct proc* sender = current;
    struct proc* p_dest = proc_table + dest; /* proc dest*/

    /*check for deadlock here*/
    if (deadlock(proc2pid(sender), dest)){
        panic(">>DEADLOKC<< %s->%s", sender->name, p_dest->name);
    }

    if ((p_dest->p_flags & RECEIVING) &&
        (p_dest->p_recvfrom == proc2pid(sender) ||
         p_dest->p_recvfrom == ANY)) {
        
        assert(p_dest->p_msg);
        assert(m);
        
        phys_copy(va2la(dest, p_dest->p_msg),
                va2la(proc2pid(sender),m),
                sizeof (MESSAGE));
        p_dest->p_msg = 0;
        p_dest->p_flags &= ~RECEIVING;
        p_dest->p_recvfrom = NO_TASK;
        unblock(p_dest);

        assert(p_dest->p_flags == 0);
        assert(p_dest->p_msg == 0);
        assert(p_dest->p_recvfrom == NO_TASK);
        assert(p_dest->p_sendto == NO_TASK);
        assert(sender->p_flags == 0);
        assert(sender->p_msg == 0);
        assert(sender->p_recvfrom == NO_TASK);
        assert(sender->p_sendto == NO_TASK);
    }
    else { /* dest is not waiting for the msg*/
        sender->p_flags |= SENDING;
        assert(sender->p_flags == SENDING);
        sender->p_sendto = dest;
        sender->p_msg = m;

        /* 添加到等待队列*/
        struct proc *p;
        if (p_dest->q_sending) {
            p = p_dest->q_sending;
            while (p->next_sending)
                p = p->next_sending;
            p->next_sending = sender;
        } else {
            p_dest->q_sending = sender;
        }
        sender->next_sending = 0;

        block(sender);

        assert(sender->p_flags == SENDING);
        assert(sender->p_msg != 0);
        assert(sender->p_recvfrom == NO_TASK);
        assert(sender->p_sendto == dest);
    }

    return 0;
}

/*********************************************************************
 *                              msg_receive
 *********************************************************************
 **<Ring 0> 接受消息从发送者，如果发送者被阻塞，把消息复制到接受者空间，然后解除源
 *的阻塞，否则的话调用函数的人会被阻塞
 * @param current The caller, 接受者
 * @param dest    发送者
 * @param m       The message 
 * 
 * @return Zero if success.
 ************************************************************************/
PRIVATE int msg_receive(struct proc *current, int src, MESSAGE * m)
{
    struct proc* p_who_wanna_recv = current;

    struct proc* p_from = 0;
    struct proc* prev = 0;
    int copyok = 0;

    /* 接收者和发送者不是同一个进程*/
    assert(proc2pid(p_who_wanna_recv) != src);

    if ((p_who_wanna_recv->has_int_msg) &&
        ((src == ANY)|| (src == INTERRUPT))){
        /* 有中断需要我门这个想要接收的进程进程处理，而且这个进程准备处理他
        */

        MESSAGE msg;
        
        reset_msg(&msg);
        msg.source = INTERRUPT;
        msg.type = HARD_INT;
        assert(m);

        phys_copy(va2la(proc2pid(p_who_wanna_recv), m), &msg,
                 sizeof(MESSAGE));
        
        p_who_wanna_recv->has_int_msg = 0;
        assert(p_who_wanna_recv->p_flags == 0);
        assert(p_who_wanna_recv->p_msg == 0);
        assert(p_who_wanna_recv->p_sendto == NO_TASK);
        assert(p_who_wanna_recv->has_int_msg == 0);
        
        return 0;
    }

    /* 到达这里如果没有中断发生*/
    if (src == ANY){
        /* 这个进程准备接受从任何进程发来的消息，我们检查等待序列然后选择第一个进程*/
        if (p_who_wanna_recv->q_sending){
            p_from = p_who_wanna_recv->q_sending;
            copyok = 1;
            
            assert(p_who_wanna_recv->p_flags == 0);
            assert(p_who_wanna_recv->p_msg == 0);
            assert(p_who_wanna_recv->p_recvfrom == NO_TASK);
            assert(p_who_wanna_recv->p_sendto == NO_TASK);

            assert(p_from->p_flags == SENDING);
            assert(p_from->p_msg != 0);
            assert(p_from->p_recvfrom == NO_TASK);
            assert(p_from->p_sendto == proc2pid(p_who_wanna_recv));
        }
    }else {
        /*进程只想接特定的进程发送的数据
        */
        p_from = &proc_table[src];

        if ((p_from->p_flags & SENDING) && 
            (p_from->p_sendto == proc2pid(p_who_wanna_recv))){
            /**
             * 恰好别人准备发送消息给我们
             */
            copyok = 1;

            struct proc* p = p_who_wanna_recv->q_sending;
            assert(p); /* 因为有进程对当前进程发消息的时候，我们的等待队列自然不为空
                        */
            /*从等待队列中删除进程，所以我们需要指针去记录这个位置*/
            while (p){
                assert(p_from->p_flags & SENDING);
                if (proc2pid(p) == src){
                    p_from = p;
                    break;
                }
                prev = p;
                p = p->next_sending;
            }

            assert(p_who_wanna_recv->p_flags == 0);
            assert(p_who_wanna_recv->p_msg == 0);
            assert(p_who_wanna_recv->p_recvfrom == NO_TASK);
            assert(p_who_wanna_recv->p_sendto == NO_TASK);
            assert(p_who_wanna_recv->q_sending != 0);
            assert(p_from->p_flags == SENDING);
            assert(p_from->p_msg != 0);
            assert(p_from->p_recvfrom == NO_TASK);
            assert(p_from->p_sendto == proc2pid(p_who_wanna_recv));

        }
    }

    if (copyok){
        /* 我们需要在等待队列中去掉这个等待我们的进程*/
        if (p_from == p_who_wanna_recv->q_sending){ /*队首元素*/
            assert(prev == 0);
            p_who_wanna_recv->q_sending = p_from->next_sending;
            p_from->next_sending = 0;
        } else {
            assert(prev);
            prev->next_sending = p_from->next_sending;
            p_from->next_sending = 0;
        }

        assert(m);
        assert(p_from->p_msg);

        /*复制信息*/
        phys_copy(va2la(proc2pid(p_who_wanna_recv), m),
                  va2la(proc2pid(p_from), p_from->p_msg),
                  sizeof (MESSAGE));
        
        p_from->p_msg = 0;
        p_from->p_sendto = NO_TASK;
        p_from->p_flags &= ~SENDING;
        unblock(p_from);
    } else { /*没有人发送消息，我们把当前进程的状态变为接收状态
            * 表明我们当前想要接受信息，所以我们需要进入阻塞状态，  
            */
        p_who_wanna_recv->p_flags |= RECEIVING;
        
        p_who_wanna_recv->p_msg = m;
        
        if (src == ANY)
            p_who_wanna_recv->p_recvfrom = ANY;
        else 
            p_who_wanna_recv->p_recvfrom = proc2pid(p_from);

        block(p_who_wanna_recv);

        assert(p_who_wanna_recv->p_flags == RECEIVING);
        assert(p_who_wanna_recv->p_msg != 0);
        assert(p_who_wanna_recv->p_recvfrom != NO_TASK);
        assert(p_who_wanna_recv->p_sendto == NO_TASK);
        assert(p_who_wanna_recv->has_int_msg == 0);
    }
    return 0;
}



/*****************************************************************************
 *                                reset_msg
 *****************************************************************************/
/**
 * <Ring 0~3> 把一个消息清空
 * 
 * @param p  将要被清空的消息
 *****************************************************************************/
PUBLIC void reset_msg(MESSAGE *m)
{
    memset(m, 0, sizeof(MESSAGE));
}



/**************************************************************
 *                          deadlock            
 **************************************************************
 **
 *  <Ring 0>检测我们的等待序列是否变成了环
 * 比如a->b->c->a
 * 
 * @param src 发送消息
 * @param dest 发送目的地
 * 
 * @return 0 如果没有死锁
 */
PRIVATE  int deadlock(int src, int dest)
{
    struct proc* p = proc_table + dest;

    while (1){
        if (p->p_flags & SENDING){
            if (p->p_sendto == src){
                /*打印这个链*/
                p = proc_table + dest;
                printl("==%s", p->name);
                do {
                    assert(p->p_msg);
                    p = proc_table + p->p_sendto;
                    printl("->%s", p->name);
                } while (p != proc_table + src);
            printl("==");
            
            return 1;
            }
            p = proc_table + p->p_sendto;
        } else {
            break;
        }
    }
    return 0;
}

/**************************************************************
 *                      block 
 **************************************************************
 <Ring 0>这个函数被调用当 'p_flags' 已经被正确的置位, 然后他去调用别的
 进程在就绪队列中的，这个函数不改变进程的的p_flags位
 @param p 将要被阻塞的进程
 *************************************************************/
PRIVATE void block (struct proc* p)
{
    assert(p->p_flags);
    schedule();
}

/**************************************************************
 *                      unblock 
 **************************************************************
 <Ring 0>这个函数被调用当 'p_flags' 已经被清除了所有的位
 @param p 将要被阻塞的进程
 *************************************************************/
PRIVATE void unblock(struct proc* p)
{
    assert(p->p_flags == 0);
}