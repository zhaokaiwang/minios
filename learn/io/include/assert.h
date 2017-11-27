/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                            assert.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                            zhaokai 2017 */
#ifndef _MINIOS_ASSERT_H
#define _MINIOS_ASSERT_H
#include  "type.h"

#define ASSERT 
#ifdef  ASSERT
void assertion_failure(char* exp, char* file, char *base_file, int line);
#define assert(exp) if (exp) ;\
        else assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)
#else
#define assert(exp)
#endif

PUBLIC void spin(char * func_name);
PUBLIC void panic(const char* fmt, ...);
#endif