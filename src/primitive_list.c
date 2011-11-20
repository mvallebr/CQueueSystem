#include <primitive_list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <logmgr.h>

primitivelist pl_global;
primitivelist *primitivelist_getglobal() {
	return &pl_global;
}
primitivelist *primitivelist_open() {
	primitivelist *pl;
	pl = (primitivelist *) malloc(sizeof(primitivelist));
	pl->num_primitives=0;
	return pl;
}
int primitivelist_readfolder(primitivelist *pl, char *foldername) {	
	DIR *dir;
	struct dirent *current_file;
	char folder[10240], fullpath[10240];
	
	dir = opendir(foldername);
	if (!dir) {
		logmgr_writef(NULL, "PRIMITIVE_LIST: Cannot open directory '%s' to search files on it", foldername);
		return 0;
	}		
	strcpy(folder, foldername);
	if (folder[strlen(folder)]!='/')
		strcat(folder, "/");
	pl->num_primitives=0;
	current_file = readdir(dir); /* Ignores . and .. */
	current_file = readdir(dir);
	while ((current_file = readdir(dir))) {
		strcpy(fullpath, folder);
		strcat(fullpath, current_file->d_name);
		logmgr_writef(NULL, "PRIMITIVE LIST: Reading primitive file '%s' ", fullpath);
		pl->primitv[pl->num_primitives] = primitive_open(fullpath);
		if (!pl->primitv[pl->num_primitives]) {
			logmgr_writef(NULL, "PRIMITIVE_LIST: Error on loading primitive config file '%s'! ", fullpath);
			primitivelist_empty_list(pl);
			return 0;
		}
		/*xmlDocDump(stdout, pl->primitv[pl->num_primitives]->cfg->doc);*/
		/*{
			xmlBufferPtr buffer;
			char boutput[10240], *btemp;
			int i, j;	
			
			buffer = xmlBufferCreate();
			xmlNodeDump(buffer, pl->primitv[pl->num_primitives]->cfg->doc, xmlDocGetRootElement(pl->primitv[pl->num_primitives]->cfg->doc), 1 ,0);			
			printf("\n\n");
			puts("<?xml version='1.0' encoding='ISO-8859-1' ?>\n"); 
			btemp = (char*)xmlBufferContent(buffer);
			
			i = 10240;
			j = strlen(btemp);
			UTF8Toisolat1(boutput, &i, btemp, &j);
			boutput[i]='\0';
		
			puts( boutput);	
			printf("\n\n");
			xmlBufferFree(buffer);  
		}*/
		pl->num_primitives++;
	};
	
	closedir(dir);
	return 1;
}

void primitivelist_empty_list(primitivelist *pl) {
	int i;
	for (i=0; i<pl->num_primitives; i++) {
		free(pl->primitv[i]);
	}
	pl->num_primitives=0;
}
void primitivelist_close(primitivelist *pl) {
	primitivelist_empty_list(pl);
	free(pl);
}
