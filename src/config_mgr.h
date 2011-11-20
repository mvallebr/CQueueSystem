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

configmgr *configmgr_getglobal();
configmgr *configmgr_open();
void configmgr_close(configmgr* cfg);
int configmgr_parsecmdline(configmgr *cfg, int argc, char *argv[]);
int configmgr_setDefaults(configmgr *cfg);
int configmgr_loadCfg(configmgr *cfg);
int configmgr_parseCfg(configmgr *cfg, char *filename);
/* Instead of parsing a xml file, take a root node of a xml file as argument and create the configmgr instance*/
int configmgr_doc_from_rootnode(configmgr *cfg, xmlNodePtr root_node);
int configmgr_getvalues(configmgr *cfg, configValueStr **values, const char *name);/* Returns number of found elements */
int configmgr_getfirst_integer(configmgr *cfg, const char *name);
int configmgr_getfirst_string(configmgr *cfg, char *strvalue, const char *name);
int  configmgr_getNodes(configmgr *cfg, const char *xpathName, xmlNodePtr **nodes);
int  configmgr_setNodeContent(configmgr *cfg, const char *xpathName, const char *content);

#endif /*__ARS_IF_GINSC_CONFIGMGR_H__*/
