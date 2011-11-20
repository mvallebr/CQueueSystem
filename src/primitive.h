#ifndef __ARS_IF_GINSC_PRIMITIVE_H__
#define __ARS_IF_GINSC_PRIMITIVE_H__
#include <config_mgr.h>

#define PRIMITIVE_SEPARATOR "__"

typedef struct _primitive {
	configmgr *cfg; /* handler to primitive xml parsed file */
	char filename[10240];	
    int notify_code;
} primitive;

primitive *primitive_open(char *filename);
primitive *primitive_open_from_xml(xmlNodePtr root_node);
void primitive_close(primitive *p);

#endif /*__ARS_IF_GINSC_PRIMITIVE_H__*/
