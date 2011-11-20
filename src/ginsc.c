#include <ginsc.h>
#include <dirent.h>
#include <logmgr.h>
#include <time.h>
#include <config_mgr.h>
#include <libcsoap/soap-env.h>
#include <libcsoap/soap-client.h>
#include <ordered_list.h>
#include <unistd.h>
#include <processor.h>
#include <string.h>

ginsc *ginsc_init(void) {
   ginsc *g;
	g = (ginsc *) malloc(sizeof(ginsc));
	return g;
}

int ginsc_destroy(ginsc *g){
   free(g);
   return 1;
}

int ginsc_connect(ginsc *g) { 	
	configmgr *cfg;	
	
	cfg = configmgr_getglobal();
	if (!configmgr_getfirst_string(cfg, g->url, "//config/pginsc_url")) {
		logmgr_write(NULL, "GINSC: Error on loading pginsc_url from config file! I won't be able to send anything...");
		return 0;
	}
   	return 1;
}

int ginsc_disconnect(ginsc *g) {
   return 1;
}   

int ginsc_processQueue(ginsc *g, const char *pginsc_output_folder, const char *pginsc_error_folder, 
	const char *ars_output_folder) {
	SoapEnv  *env=NULL, *res=NULL;
	xmlNodePtr root_node;
	DIR *dir;
	struct dirent *current_file;
	char fullpath[10240];
	orderedlist *ol;
	listitem *li;
	primitive *p, *arsp;
	char status[1024], statusmsg[1024];
	int result=0;
	

	processor_wait_for_no_lock(pginsc_output_folder);
	processor_write_lock(pginsc_output_folder);
	
	ol = orderedlist_init();
	if (ol==NULL) {
		logmgr_write(NULL, "GINSC: QUEUE: Error initializing the ordered list!");
		return 0;
	}
	
	dir = opendir(pginsc_output_folder);
	if (!dir) {
		logmgr_writef(NULL, "GINSC: QUEUE: Cannot open directory '%s' to search files on it", pginsc_output_folder);
		return 0;
	}		
	current_file = readdir(dir); /* Ignores . and .. */
	current_file = readdir(dir);
	while ((current_file = readdir(dir))) {
		orderedlist_add_value(ol, current_file->d_name);
	};	
	closedir(dir);
	
	/* Now, send each file, in correct order */	
	li=orderedlist_seek_first(ol);
	while (li) { 				
		/* Skip lock file */
		if (strcmp(orderedlist_get_current_value(ol), "lock")==0) {			
			li = orderedlist_seek_next(ol);
			continue;
		}	
		logmgr_writef(NULL, "GINSC: QUEUE: Sending primitive file  '%s'...", orderedlist_get_current_value(ol) );
		/* if needed to move to errores, skip this file */		
		if (processor_move_to_dest_if_needed(orderedlist_get_current_value(ol), 
										pginsc_output_folder, pginsc_error_folder)) {
			logmgr_writef(NULL, "GINSC: QUEUE: NOT SENDING -> moved primitive file '%s' to folder '%s'", 
							orderedlist_get_current_value(ol),  pginsc_error_folder);
			li = orderedlist_seek_next(ol);
			continue;
		}		
		fflush(stdout);
		strcpy(fullpath, pginsc_output_folder);
		strcat(fullpath, orderedlist_get_current_value(ol));
		p = primitive_open(fullpath);		
		env = soap_env_new_from_doc(p->cfg->doc);
		/*{
			xmlBufferPtr buffer;
			char boutput[10240], *btemp;
			int i, j;	
			
			buffer = xmlBufferCreate();
			xmlNodeDump(buffer, p->cfg->doc, xmlDocGetRootElement(p->cfg->doc), 1 ,0);			
			btemp = (char*)xmlBufferContent(buffer);
			
			i = 10240;
			j = strlen(btemp);
			isolat1ToUTF8(boutput, &i, btemp, &j);
			boutput[i]='\0';
			env = soap_env_new_from_doc(xmlParseMemory(boutput, sizeof(boutput))); 
			xmlBufferFree(buffer);  
		}*/
		/*env = soap_env_new_empty();
		env->root = xmlDocGetRootElement(p->cfg->doc);
		env->cur = env->root;*/
		/* Send to ginsc and get the answer */
		/*soap_xml_doc_print(env->root->doc);		*/
		logmgr_writef(NULL, "GINSC: QUEUE:  &&&&&&&&&&&&&&&&& Sending file  '%s' now,waiting for answer... ", 
			orderedlist_get_current_value(ol) );
		res = soap_client_invoke(env, g->url , "");
		printf("\n\n");
	  	soap_xml_doc_print(res->root->doc);		
	  	printf("\n\n");
	  	/* Lets see what is the answer */
	  	/********************************/
	  	if (res==NULL) {
		  	logmgr_writef(NULL, "GINSC: QUEUE: No response from PGINSC, when sending '%s'. I will let it on folder '%s'", 
							orderedlist_get_current_value(ol), pginsc_output_folder);	  	
	  	} else {
	  		
	  		if (soap_env_is_fault(res)) {
	  			logmgr_writef(NULL, "GINSC: QUEUE: Fault received as response...");
	  			if (!strcmp(soap_env_get_fault_message(res), "Response is NULL")) {
	  				logmgr_writef(NULL, "GINSC: QUEUE: Fault 'Response is NULL' from PGINSC, when sending '%s'. I will let it on folder '%s'", 
							orderedlist_get_current_value(ol), pginsc_output_folder);	  	
					li = orderedlist_seek_next(ol);
					continue;
	  			}
	  		}	  		
			logmgr_writef(NULL, "GINSC: QUEUE: Primitive file '%s' sent succesfully, parsing response", 
							orderedlist_get_current_value(ol));	  	
			/* Process response, or be, create the primitive from response */
			root_node = soap_env_get_method(res);  	
			arsp = primitive_open_from_xml(root_node);
			/*xmlDocDump(stdout, arsp->cfg->doc);*/
			if (arsp==NULL) {
				logmgr_writef(NULL, "GINSC: QUEUE: FATAL ERROR on creating ars primitive (%s) from response!", 
							orderedlist_get_current_value(ol));
				/* move to errors dir */
				processor_move_to_dest(orderedlist_get_current_value(ol), pginsc_output_folder, pginsc_error_folder);
				li = orderedlist_seek_next(ol);
				continue;
			}			
			/* Now, verify status received */
			strcpy(status, "");
			strcpy(statusmsg, "");
			result = (configmgr_getfirst_string(arsp->cfg, status, "//primitive/status/code")) &&
						(configmgr_getfirst_string(arsp->cfg, statusmsg, "//primitive/status/message"));
			if ( (!result) || (strcasecmp(status, "ok")) ) {
				logmgr_writef(NULL, "GINSC: QUEUE: ERROR on status received from primitive (%s) ", 
					orderedlist_get_current_value(ol));				
				/* move to errors dir */
				processor_move_to_dest(orderedlist_get_current_value(ol), pginsc_output_folder, pginsc_error_folder);
				li = orderedlist_seek_next(ol);
				continue;
			}
			logmgr_writef(NULL, "GINSC: QUEUE: Reception from pginsc -> Status='%s' Status Message='%s'", 
					status, statusmsg);
			/* Enqueue primitive to ars */
			if (!processor_enqueueData(arsp, ars_output_folder)) {
				logmgr_writef(NULL, "GINSC: QUEUE: FATAL ERROR to enqueue primitive (%s) received from response to ars!", 
							orderedlist_get_current_value(ol));
				/* move to errors dir */
				processor_move_to_dest(orderedlist_get_current_value(ol), pginsc_output_folder, pginsc_error_folder);
				li = orderedlist_seek_next(ol);
				continue;
			}
			soap_env_free(res);
			/* Everything was fine. Delete the file from output folder  */
			logmgr_writef(NULL, "GINSC: QUEUE: &&&&& File  '%s' sent and processed ok, I'm gonna kill it!!! ", orderedlist_get_current_value(ol) );
			if (unlink(fullpath)!=0) {
				logmgr_writef(NULL, "GINSC: QUEUE: FATAL ERROR! Primitive file '%s' could not be deleted! It may be sent again in future!!", 
							orderedlist_get_current_value(ol));	  					
			} else {
				logmgr_writef(NULL, "GINSC: QUEUE: File  '%s' killed ", orderedlist_get_current_value(ol) );
			}
	  	}								  	
	  	/********************************/	  	
	  	/* Ok, next! */
	  	soap_env_free(env);
	  	primitive_close(p);		
	  	li = orderedlist_seek_next(ol);
	} 
	
	orderedlist_destroy(ol);
	processor_delete_lock(pginsc_output_folder);
	return 1;
}



