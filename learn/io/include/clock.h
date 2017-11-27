/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                            clock.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                            zhaokai 2017 */

#ifndef _MINIOS_CLOCK_H
#define _MINIOS_CLOCK_H                                            
#include "type.h"

/* 8253/8254 PIT (Programmable Interval Timer) */
#define TIMER0         0x40 /* I/O port for timer channel 0 */
#define TIMER_MODE     0x43 /* I/O port for timer mode control */
#define RATE_GENERATOR 0x34 /* 00-11-010-0 :
			     * Counter0 - LSB then MSB - rate generator - binary
			     */
#define TIMER_FREQ     1193182L/* clock frequency for timer in PC and AT */
#define HZ             100  /* clock freq (software settable on IBM-PC) */

PUBLIC void clock_handler (int irq);
PUBLIC void milli_delay(int milli_sec);
PUBLIC void init_clock ();
#endif