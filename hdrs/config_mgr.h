#ifndef __ARS_IF_GINSC_CONFIGMGR_H__
#define __ARS_IF_GINSC_CONFIGMGR_H__
#include <libxml/xpath.h>

#define MAX_ARRAY_SIZE 100
#define MAX_STR_SIZE 10240

typedef char configValueStr[MAX_STR_SIZE];
typedef struct _configmgr {
	xmlDocPtr doc;	
	xmlXPathContextPtr xpathCtx; 
    xmlXPathObjectPtr 	xpathObj; 
    configValueStr valueArray[MAX_ARRAY_SIZE];
} configmgr;

configmgr configmgr_getglobal();
int configmgr_parsecmdline(int argc, char *argv[]);
int configmgr_setDefaults();
int configmgr_loadCfg();
int configmgr_getvalue(configValueStr **values, const char *name);/*Returns 0 if the value wasn't found*/

#endif /*__ARS_IF_GINSC_CONFIGMGR_H__*/
