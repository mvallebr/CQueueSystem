#ifndef __ARS_IF_GINSC_ARS_H__
#define __ARS_IF_GINSC_ARS_H__

#include <ar.h>
#include <arstruct.h>
#include <arextern.h>

#include <nt.h>
#include <nterrno.h>
#include <ntsextrn.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <primitive.h>
#include <notification.h>

#include <libcsoap/soap-env.h>
#include <libcsoap/soap-client.h>
#include <dirent.h>
#include <ordered_list.h>
#include <unistd.h>

typedef struct _node_field_list {
	xmlNodePtr *nodeArray;	
	int num_nodes, num_fields;
	char nodespath[10240];	
	ARInternalIdList  *fields; /* list of needed fields on entry */
	ARFieldValueList  *values;/* values of fields */
} node_field_list;

typedef struct _ars {
	ARControlStruct ctrl;
	ARStatusList status;	
} ars;
/* Connection, initializationa nd finalization methods */
ars * ars_init(void);
int ars_destroy(ars *a);
node_field_list * ars_nfl_init(void);
int ars_nfl_destroy(node_field_list *nfl);
int ars_connect(ars *a, primitive *p, notification n);
int ars_disconnect(ars *a);
/* Write and read fields from ars */
int ars_getRecord(ars *a, primitive *p, notification n, char  *configpath);
int ars_setRecord(ars *a, primitive *p, notification n, char  *configpath);
/* Useful methods */
void ars_printStatusList(ars *a, ARStatusList status);
void trim_string(char *text);
int ars_format_datetime(ars *a, char *value, int valuelen, char *format, long timestamp);
int ars_format_arsvalue(ars *a, char *fvalue, int fvaluelen, int index, node_field_list *nfl);
/* Process queue of primitives to send to ars */
int ars_processQueue(const char *ars_output_folder, const char *ars_error_folder);

#endif /* __ARS_IF_GINSC_ARS_H__*/
