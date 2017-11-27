/*==============================================================================================
                                                                        assert.c
============================================================================================
                                                                        zhaokai, 2017
*/
#include "type.h"
#include "assert.h"
#include "proc.h"
#include "printf.h"
#include "vsprintf.h"
/***************************************************
 *              spin    
 ****************************************************/
PUBLIC void spin(char * func_name)
{
    printl("\nspinning is %s ...\n", func_name);
    while (1) {}
}

/******************************************************************'
 *                              assertion_failure
 * *****************************************************************
 * Invoked by assert().
 * @param exp   The failure expression itself
 * @param file      __FILE__
 * @param base_file __BASE_FILE__
 * @param line      __LINE__
**********************************************************************/
PUBLIC void assertion_failure(char* exp, char* file, char* base_file, int line)
{
    printl("%c assert(%s) failed: file %s, base_file: %s, ln%d",
           MAG_CH_ASSERT, exp, file, base_file, line);

    /**
     * If assertion fail in a TASK, the system will halt before 
     * printl() returns, If it happens in a UEST PROC, printl() will
     * return like a common routine and arrive here
     * 
     * We ues a infinte loop to prevent proc from going on
     */
    
    spin("assertion_failure()");

    /* should never arrive here*/
        __asm__ __volatile__("ud2");
    }

/***********************************************************************
 *                      panic
 ************************************************************************/
PUBLIC void panic(const char* fmt, ...)
{
    int i;
    char buf[256];

    /* 4 is the size of fmt in the stack*/
    va_list arg = (va_list)((char*)&fmt + 4);

    i = vsprintf(buf, fmt, arg);

    printl ("%c !!panic!! %s", MAG_CH_PANIC, buf);

    /* should never arrive here*/
    __asm__ __volatile__("ud2");
}
