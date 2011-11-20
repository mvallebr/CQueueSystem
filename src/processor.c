#include <processor.h>
#include <logmgr.h>
#include <ginsc.h>
#include <primitive_list.h>
#include <libcsoap/soap-env.h>
#include <libcsoap/soap-client.h>

processor *processor_init(void){
	processor *p;	
	configmgr *cfg;	
	
	p = (processor *) malloc(sizeof(processor));
	
	cfg = configmgr_getglobal();
	if (!configmgr_getfirst_string(cfg, p->pginsc_output_folder, "//config/pginsc_output_folder")) {
		logmgr_write(NULL, "PROCESSOR: Error on loading pginsc_output_folder from config file! I won't be able to send anything...");
		return 0;
	}
	if (!configmgr_getfirst_string(cfg, p->pginsc_error_folder, "//config/pginsc_error_folder")) {
		logmgr_write(NULL, "PROCESSOR: Error on loading pginsc_error_folder from config file! I won't be able to send anything...");
		return 0;
	}
	if (!configmgr_getfirst_string(cfg, p->ars_output_folder, "//config/ars_output_folder")) {
		logmgr_write(NULL, "PROCESSOR: Error on loading ars_output_folder from config file! I won't be able to send anything...");
		return 0;
	}
	if (!configmgr_getfirst_string(cfg, p->ars_error_folder, "//config/ars_error_folder")) {
		logmgr_write(NULL, "PROCESSOR: Error on loading ars_error_folder from config file! I won't be able to send anything...");
		return 0;
	}
	if (p->pginsc_output_folder[strlen(p->pginsc_output_folder)]!='/')
		strcat(p->pginsc_output_folder, "/");
	if (p->pginsc_error_folder[strlen(p->pginsc_error_folder)]!='/')
		strcat(p->pginsc_error_folder, "/");
	if (p->ars_output_folder[strlen(p->ars_output_folder)]!='/')
		strcat(p->ars_output_folder, "/");
	if (p->ars_error_folder[strlen(p->ars_error_folder)]!='/')
		strcat(p->ars_error_folder, "/");
	return p;
}

int processor_destroy(processor *p){
	free(p);
	return 1;
}

int processor_run(processor *p, char *newbuffer) {
	ars *a;
	ginsc *g;	
	primitivelist *primitives;
	int i;
	int rvalue; /* Return value */
	
	/* Read primitive configuration */
	primitives = primitivelist_getglobal();
		
	strcpy(p->buffer, newbuffer); /*//Copia o buffer para uso interno*/
	if (!notification_parse(&(p->notif), newbuffer)) {
		logmgr_write(NULL, "PROCESSOR: Cannot parse notification or notification wasn't type I need");
		return 0;
	}	
	/* Open connection */
	g = ginsc_init();
	if (!ginsc_connect(g)) {
		logmgr_write(NULL, "PROCESSOR: Error! Cannot open communication with ginsc. Verify config file!");
		return 0;
	}
	
	/* Verify each primitive, and process those which notification notify code is equal primitive notify code */		
	for (i=0; i<primitives->num_primitives; i++) {
		if (primitives->primitv[i]->notify_code == p->notif.code) {
			/* Create a connection to ARS system and get the data*/
			a = ars_init();
			ars_connect(a, primitives->primitv[i], p->notif);
			if (!ars_getRecord(a, primitives->primitv[i], p->notif, "//primitive/request")) {
				logmgr_write(NULL, "PROCESSOR: Error on getting ARS record!");
				return 0;
			}
			/* Change the IDENT of primitive, from fields we changed */
			if (!ars_getRecord(a, primitives->primitv[i], p->notif, "//primitive/ident")) {
				logmgr_write(NULL, "PROCESSOR: Error on changing primitive IDENT!");
				return 0;
			}
			/* Change the NOTIFY/QUERY of primitive, from fields we changed */
			if (!ars_getRecord(a, primitives->primitv[i], p->notif, "//primitive/notif/query")) {
				logmgr_write(NULL, "PROCESSOR: Error on changing primitive NOTIF/QUERY!");
				return 0;
			}
			printf("1\n");
			fflush(stdout);
			ars_disconnect(a);
			ars_destroy(a);			
			/* Enqueue data to send to pginsc */
			processor_enqueueData(primitives->primitv[i], p->pginsc_output_folder);
		}
	}
	/* Now all the data is enqueued, process the queue*/		
	/*rvalue = ginsc_processQueue(g);*/
	ginsc_disconnect(g);
	
	ginsc_destroy(g);	
	
	return rvalue;
}

int processor_enqueueData(primitive *p, const char *queue_folder) {
	SoapEnv  *env;
	xmlDocPtr doc;
	xmlNodePtr body;	
	char entryid[1024], timestamp[1024], pfilename[2*1024];	
		
	if (!configmgr_getfirst_string(p->cfg, entryid, "//primitive/ident/id")) {
		logmgr_write(NULL, "PROCESSOR: Error on loading entry ID from primitive! Cannot enqueue!");
		return 0;
	}
	if (!configmgr_getfirst_string(p->cfg, timestamp, "//primitive/ident/timestamp")) {
		logmgr_write(NULL, "PROCESSOR: Error on loading timestamp from primitive! Cannot enqueue!");
		return 0;
	}
	sprintf(pfilename, "%s%s%s%015ld.xml", queue_folder, 
		entryid, PRIMITIVE_SEPARATOR, atol(timestamp));
	
    /*Add content to xml*/
    doc = xmlCopyDoc(p->cfg->doc, 1);
  	/*env = soap_env_new_empty();
  	soap_env_add_envelope(env, "http://schemas.xmlsoap.org/soap/envelope/");
  	soap_env_add_global_ns_value(env, "SOAP-ENV", "encoding", "http://schemas.xmlsoap.org/soap/encoding/");
    soap_env_add_global_ns(env, "xsi", "http://www.w3.org/1999/XMLSchema-instance");
    soap_env_add_global_ns(env, "xsd", "http://www.w3.org/1999/XMLSchema");
    soap_env_add_body(env);    */
    
    env = soap_env_new_from_buffer("<?xml version='1.0' encoding='ISO-8859-1' ?>"\
    														"<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encoding=\"http://schemas.xmlsoap.org/soap/encoding/\"" \
  															" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\""\
  															" xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">" \
															"<SOAP-ENV:Body>"\
															"</SOAP-ENV:Body>"\
															"</SOAP-ENV:Envelope>"
															);/**/
    body = soap_env_get_body(env);
    /*printf("\n\n\n");
    soap_xml_doc_print(env->root->doc);	
    printf("\n\n\n");*/
	if (!xmlAddChild(body, xmlDocGetRootElement(doc))) {
		logmgr_write(NULL, "PROCESSOR: Fatal Error merging xml!");
		return 0;
	}	
	xmlSetTreeDoc			(env->root, env->root->doc);
	/* Write primitive to a file on output directory */
	/* soap_xml_doc_print(env->root->doc);	 	 */
	/*soap_xml_doc_print(env->root->doc);	*/
	logmgr_writef(NULL, "PROCESSOR: Writing primitive to file '%s' ... ", pfilename);
	/*
	{
		int i, j;
		char *btemp, boutput[10240];
		buffer = xmlBufferCreate();		
		xmlNodeDump(buffer, env->root->doc, env->root, 1 ,0);		
		if (!(f = fopen(pfilename, "w"))) {
			logmgr_writef(NULL, "PROCESSOR: FATAL ERROR: I couldn't write the primitive file '%s'!", pfilename);
			return 0;
		}
		btemp = (char*)xmlBufferContent(buffer);
		i = 10240;
		j = strlen(btemp);
		UTF8Toisolat1(boutput, &i, btemp, &j);		
		boutput[i]='\0';
		fputs("<?xml version='1.0' encoding='ISO-8859-1' ?>\n", f); 
		fputs( boutput, f);	
		xmlBufferFree(buffer);  
		fclose(f);
	}*/
	if (!xmlSaveFileEnc(pfilename, env->root->doc, "ISO-8859-1")==-1) {
			logmgr_writef(NULL, "PROCESSOR: FATAL ERROR: I couldn't write the primitive file '%s'!", pfilename);
			return 0;
	}	
	/*****************
	{
		SoapEnv  *res;
		printf("\n\n\n");  	
		res = soap_client_invoke(env, g->url , "");
  		soap_xml_doc_print(res->root->doc);	
  		soap_env_free(res);
	}	
	*****************/
		
	/* Free soap environment */
  	soap_env_free(env);
  	logmgr_writef(NULL, "PROCESSOR: Primitive file saved ok!", pfilename);
  	return 1;
}

int processor_move_to_dest_if_needed(const char *primitivefile, const char *sourcepath, const char *destpath) {
	char *posfound;
	posfound = strstr(primitivefile, PRIMITIVE_SEPARATOR);
	if (posfound==NULL){
		logmgr_writef(NULL, "PROCESSOR: FATAL ERROR: Cannot find '%s' in the name of the file '%s' ", 
			PRIMITIVE_SEPARATOR, primitivefile);
		return 0;
	}
	if (processor_entryid_exists(primitivefile, posfound-primitivefile, destpath) )
		return processor_move_to_dest(primitivefile, sourcepath, destpath);
	return 0;
}

int processor_move_to_dest(const char *primitivefile, const char *sourcepath, const char *destpath) {
	char sourcefile[10240], destfile[10240];
	strcpy(sourcefile, sourcepath);
	strcat(sourcefile, primitivefile);
	strcpy(destfile, destpath);
	strcat(destfile, primitivefile);
	logmgr_writef(NULL, "PROCESSOR: moving file from '%s' to '%s'", sourcefile, destfile);
	if (rename (sourcefile, destfile)!=0) {
		logmgr_writef(NULL, "PROCESSOR: Error on moving file from '%s' to '%s'", sourcefile, destfile);
		return 0;
	} 
	return 1;	
}

int processor_entryid_exists(const char *entryid, int entryidlen, const char *folder) {
	DIR *dir;
	struct dirent *current_file;
	int rvalue=0;
	
	dir = opendir(folder);
	if (!dir) {
		logmgr_writef(NULL, "PROCESSOR: Cannot open directory '%s' to search '%s' files on it", folder, entryid);
		return 0;
	}		
	
	current_file = readdir(dir); /* Ignores . and .. */
	current_file = readdir(dir);
	while ((current_file = readdir(dir))) {
		if (strncmp(entryid, current_file->d_name, entryidlen)==0) {
			rvalue = 1;
			break;
		}
	};	
	
	closedir(dir);
	return rvalue;	
}

int processor_lock_exists(const char *folder) {
	FILE *f;
	char filename[1024];
	processor_get_lock_filename(filename, folder);
	f = fopen(filename, "rb");
	if (f==NULL) return 0;
	fclose(f);
	return 1;
}

int processor_wait_for_no_lock(const char *folder) {
	/*return 1;*/
	while (processor_lock_exists(folder)) {
		logmgr_writef(NULL, "PROCESSOR: Waiting for someone kill lock file on '%s' ...", folder);
		sleep(30);
	}
	return 1;
}

int processor_write_lock(const char *folder) {
	FILE *f;
	char filename[1024];
	processor_get_lock_filename(filename, folder);
	f = fopen(filename, "wb");
	if (f==NULL) {
		logmgr_writef(NULL, "PROCESSOR: Error on writing lock file on '%s'! ", folder);
		return 0;
	}
	fclose(f);
	return 1;
}

int processor_delete_lock(const char *folder) {	
	char filename[1024];
	processor_get_lock_filename(filename, folder);
	return (unlink(filename)==0) ? 1 : 0;
}

int processor_get_lock_filename(char *filename, const char *folder) {
	strcpy(filename, folder);
	if (filename[strlen(filename)]!='/')
		strcat(filename, "/");
	strcat(filename, "lock");
	return 1;
}
