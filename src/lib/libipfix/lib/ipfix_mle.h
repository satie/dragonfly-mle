/*
**     ipfix_mle.h - IPFIX message stream to Dragonfly MLE
**
**
*/
#ifndef _IPFIX_MLE_H
#define _IPFIX_MLE_H

#include <time.h>
#include <sys/time.h>
#include "ipfix.h"

#ifdef   __cplusplus
extern "C" {
#endif

int ipfix_start_mle(FILE *fpout);
void ipfix_stop_mle();

#ifdef   __cplusplus
}
#endif
#endif 
