/*
** ipfix_zout.h - export declarations for ipfix_zout funcs
**
*/
#ifndef _IPFIX_ZOUT_H
#define _IPFIX_ZOUT_H

#include <time.h>
#include <sys/time.h>
#include "ipfix.h"

#ifdef   __cplusplus
extern "C" {
#endif

int ipfix_start_zout(char *filepath);
void ipfix_stop_zout();

#ifdef   __cplusplus
}
#endif
#endif 
