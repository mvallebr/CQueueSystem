#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <logmgr.h>

primitive *primitive_open(char *filename) {
	primitive *p;
	p = (primitive *) malloc(sizeof(primitive));
	
	p->cfg = configmgr_open();
	if (!configmgr_parseCfg(p->cfg, filename)) return 0;
	p->notify_code = configmgr_getfirst_integer(p->cfg, "//primitive/legacy/notifyid");
	logmgr_writef(NULL, "Opened primitive notify code=%d", 	p->notify_code);	
	return p;
}

primitive *primitive_open_from_xml(xmlNodePtr root_node){
	primitive *p;
	p = (primitive *) malloc(sizeof(primitive));
	
	p->cfg = configmgr_open();
	if (!configmgr_doc_from_rootnode(p->cfg, root_node)) return 0;
	p->notify_code = configmgr_getfirst_integer(p->cfg, "//primitive/legacy/notifyid");
	logmgr_writef(NULL, "Opened primitive notify code=%d", 	p->notify_code);	
	return p;
}

void primitive_close(primitive *p) {
	configmgr_close(p->cfg);
	free(p);
}
