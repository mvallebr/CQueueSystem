#ifndef __ARS_IF_GINSC_PRIMITIVELIST_H__
#define __ARS_IF_GINSC_PRIMITIVELIST_H__
#include <config_mgr.h>
#include <primitive.h>

#define MAX_NUM_PRIMITIVES 100
typedef struct _primitivelist {
	primitive *primitv[MAX_NUM_PRIMITIVES];
    int num_primitives;
} primitivelist;

primitivelist *primitivelist_open();
primitivelist *primitivelist_getglobal();
int primitivelist_readfolder(primitivelist *pl, char *foldername);
void primitivelist_empty_list(primitivelist *pl);
void primitivelist_close(primitivelist *pl);

#endif /*__ARS_IF_GINSC_PRIMITIVELIST_H__*/
