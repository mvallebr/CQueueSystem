
#ifndef __DDR_GINSC_F1_ARS_H__
#define __DDR_GINSC_F1_ARS_H__

#include <ar.h>
#include <arstruct.h>
#include <arextern.h>

#include <nt.h>
#include <nterrno.h>
#include <ntsextrn.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <notification.h>

typedef struct _ars {
	ARControlStruct ctrl;
	ARStatusList status;
		
	ARInternalIdList  *campos; /*lista de campos relevantes da entry*/
	ARFieldValueList  *valores;/*valores dos campos*/
} ars;

ars * ars_init(void);
int ars_destroy(ars *a);
int ars_connect(ars *a, notification n);
int ars_disconnect(ars *a);
int ars_getRecord(ars *a, notification n);
void ars_printStatusList(ars *a, ARStatusList status);
void trim_string(char *text);

#endif /* __DDR_GINSC_F1_ARS_H__*/
