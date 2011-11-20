#include <ars.h>
#include <logmgr.h>
#include <notification.h>
#include <config_mgr.h>
#include <string.h>
#include <time.h>
#include <processor.h>
#include <primitive.h>

ars * ars_init(void){
	return (ars *)malloc(sizeof(ars));
}

int ars_destroy(ars *a){
	free(a);
	return 0;
}

node_field_list * ars_nfl_init(void) {
	return (node_field_list *) malloc(sizeof(node_field_list));
}

int ars_nfl_destroy(node_field_list *nfl) {
	free(nfl);
	return 0;
}

int ars_connect(ars *a, primitive *p, notification n){
   int con_status;
	/* Set and get some properties from primitive cfg */
	if (!configmgr_getfirst_string(p->cfg, a->ctrl.user, "//primitive/legacy/user")) {
		logmgr_writef(NULL, "ARS: Error reading ars system user from primitive cfg! NotifID=%d", n.code);
		return 0;
	}
	if (!configmgr_getfirst_string(p->cfg, a->ctrl.password, "//primitive/legacy/password")) {
		logmgr_writef(NULL, "ARS: Error reading ars system password from primitive cfg! NotifID=%d", n.code);
		return 0;
	}	
	trim_string(n.servername);
	strcpy(a->ctrl.server, 		n.servername);		
	if (!configmgr_setNodeContent(p->cfg, "//primitive/legacy/host", n.servername)) {
		logmgr_writef(NULL, "ARS: Error setting ARS System Host on primitive! NotifID=%d", n.code);
		return 0;
	}	
	trim_string(n.Schema);
	if (!configmgr_setNodeContent(p->cfg, "//primitive/legacy/schema", n.Schema)) {
		logmgr_writef(NULL, "ARS: Error setting ARS System Schema on primitive! NotifID=%d", n.code);
		return 0;
	}	
	/* Connect on the server */
   	con_status = ARInitialization(&(a->ctrl), &(a->status));
   	logmgr_writef(NULL, "ARS: Initialization Status: %d numitens=%d\n", con_status, a->status.numItems);
   	return (con_status<2);
}

int ars_disconnect(ars *a){
	int i;
    i = ARTermination(&(a->ctrl), &(a->status));
    logmgr_writef(NULL, "ARS Termination Status: %d numitens=%d\n", i, a->status.numItems);
	return a->status.numItems;
}


int ars_setRecord(ars *a, primitive *p, notification n, char  *configpath) {	
	node_field_list *nfl;
	char *dataType, *dataValue, *fieldType;
	char *nodeValue;
	int i, j;
	char childpath[1024];
	ARValueStruct *value=NULL;
	int year=0, month=0, day=0, hour=0, minute=0, second=0;
	struct tm timestamp;
	nfl =  ars_nfl_init();
	/***********************/
	/* Set names structure to call  ARSetEntry */
	nfl->num_nodes = 0;	
	strcpy(nfl->nodespath, configpath);
	if (nfl->nodespath[strlen(nfl->nodespath)]!='/')
		strcat(nfl->nodespath, "/");	
	strcat(nfl->nodespath, "*");
	nfl->num_nodes = configmgr_getNodes(p->cfg, nfl->nodespath, &(nfl->nodeArray));	
	/* 
	 * First, count nodes with type "field". 
	 * Nodes with this type will have its contents replaced by values received from ARS 
	*/
 	nfl->num_fields=0;
	for (i=0; i<nfl->num_nodes; i++) {
		dataType = xmlGetProp(nfl->nodeArray[i], "data_type");
		dataValue = xmlGetProp(nfl->nodeArray[i], "data_value");
		if  ((dataValue==NULL) || (dataType==NULL)) {
			strcpy(childpath, configpath);
			if (childpath[strlen(childpath)]!='/')
				strcat(childpath, "/");	
			strcat (childpath, nfl->nodeArray[i]->name);
			ars_setRecord(a, p, n, childpath);
			continue;
		}				
		if (!strcasecmp(dataType, "field")) nfl->num_fields++;
		xmlFree(dataType);	
		xmlFree(dataValue);
	}
	/* Number of field now is knew, format everything and send to ARSGetEntry */	
	nfl->fields = (ARInternalIdList *) malloc(sizeof(ARInternalIdList));
	nfl->values	=NULL;
	nfl->fields->numItems = nfl->num_fields;
	nfl->fields->internalIdList = (ARInternalId *) malloc(nfl->fields->numItems*sizeof(ARInternalId));
	j=0;
	nfl->values = (ARFieldValueList *) malloc(sizeof(ARFieldValueList));
	nfl->values->numItems = nfl->fields->numItems;
	nfl->values->fieldValueList = (ARFieldValueStruct *) malloc(nfl->values->numItems * sizeof(ARFieldValueStruct));	
	for (i=0; i<nfl->num_nodes; i++) {
		dataType = xmlGetProp(nfl->nodeArray[i], "data_type");
		if (dataType==NULL)  continue;		
		dataValue = xmlGetProp(nfl->nodeArray[i], "data_value");
		if (dataValue==NULL) continue;
		fieldType = xmlGetProp(nfl->nodeArray[i], "field_type");
		nodeValue = xmlNodeGetContent(nfl->nodeArray[i]);
		/*dd/mm/yyyy hh24:mi:ss*/
		logmgr_writef(NULL, "ARS: REQUEST: NODE NAME='%s' data_type='%s' data_value='%s'", 
				nfl->nodeArray[i]->name, dataType, dataValue);
		if (!strcasecmp(dataType, "field")) {
			nfl->fields->internalIdList[j]=atoi(dataValue); 
			nfl->values->fieldValueList[j].fieldId = nfl->fields->internalIdList[j];
			value = &(nfl->values->fieldValueList[j].value);
			logmgr_writef(NULL, "ARS: REQUEST: NODE VALUE='%s'", nodeValue);
			if (fieldType==NULL) { /* defaults to string */
				logmgr_writef(NULL, "ARS: WARNING: Type of primitive field '%s' defaults to string", nfl->nodeArray[i]->name);
				value->dataType = AR_DATA_TYPE_CHAR;
				value->u.charVal = nodeValue;
			} else if (!strcasecmp(fieldType, "string")) {
				value->dataType = AR_DATA_TYPE_CHAR;
				value->u.charVal = nodeValue;
			} else if (!strcasecmp(fieldType, "datetime")) {
				value->dataType = AR_DATA_TYPE_TIME;
				printf("************************************************************\n");
				printf("Parsing datetime value '%s'\n ", nodeValue);
				logmgr_writef(NULL, "ARS: Parsing datetime value '%s'\n ", nodeValue);
				if (sscanf(nodeValue, "%2d/%2d/%4d %2d:%2d:%2d", &day, &month, &year, &hour, &minute, &second)!=6) {
					logmgr_writef(NULL, "ARS: FATAL ERROR: Invalid date time format on value '%s'", nodeValue);
					return 0;
				}
				timestamp.tm_year = year - 1900;
				timestamp.tm_mon = month -1;
				timestamp.tm_mday = day;
				timestamp.tm_hour = hour;
				timestamp.tm_min = minute;
				timestamp.tm_sec = second;				
				timestamp.tm_isdst = 0;
				value->u.timeVal = mktime(&timestamp);
				printf("DATE TIME %2d/%2d/%4d %2d:%2d:%2d tm= %ld\n", 
					timestamp.tm_year, timestamp.tm_mon, timestamp.tm_mday, 
					timestamp.tm_hour, timestamp.tm_min, timestamp.tm_sec, value->u.timeVal);
				/* ************************ */				
			} else if (!strcasecmp(fieldType, "sla")) {
				value->dataType = AR_DATA_TYPE_TIME;
				logmgr_writef(NULL, "ARS: got SLA value '%s'\n ", nodeValue);
				if (atof(nodeValue)==0.0) {
					logmgr_writef(NULL, "ARS: FATAL ERROR: Invalid sla received: '%s'", nodeValue);
					return 0;
				}
				{ /* get initial datetime from ars */
					int con_status=0;
					ARInternalIdList  *fields; 
					ARFieldValueList  *values;
					char *ini_date_id;
					n.idlist.entryIdList = (AREntryIdType *) n.EntryID;
					n.idlist.numItems = 1;	
					trim_string(n.Schema);
					trim_string(n.EntryID);
					fields = (ARInternalIdList *) malloc(sizeof(ARInternalIdList));
					fields->numItems = 1;
					fields->internalIdList = (ARInternalId *) malloc(fields->numItems*sizeof(ARInternalId));
					values = (ARFieldValueList *) malloc(sizeof(ARFieldValueList));
					values->numItems = fields->numItems;
					values->fieldValueList = (ARFieldValueStruct *) malloc(values->numItems * sizeof(ARFieldValueStruct));						
					ini_date_id = xmlGetProp(nfl->nodeArray[i], "ini_date_id");
					fields->internalIdList[0]= atoi(ini_date_id);
					printf("schema='%s'\n", n.Schema);
					printf("entrid   ='%s'\n", n.EntryID);
					printf("ini_date_id=%s\n", ini_date_id);
					printf("internalIdList[0]=%ld\n", fields->internalIdList[0]);
					ARGetEntry( &(a->ctrl), n.Schema, 
						&(n.idlist), 
						fields, values,
						&(a->status)  );
					logmgr_writef(NULL, "\nARS: REQUEST: SLAToTimestamp: ARGetEntry status result: %d", con_status);
					ars_printStatusList(a, a->status);
					/* debug 
					printf("values=%d\n", values->numItems);
					printf("values->fieldValueList[0]=%p\n", &(values->fieldValueList[0]));
					printf("values->fieldValueList[0].value=%p\n", &(values->fieldValueList[0].value));
					printf("nodeValue=%s\n", nodeValue);
					printf("values->fieldValueList[0].value.u.timeVal=%ld\n", values->fieldValueList[0].value.u.timeVal);
					printf("values->fieldValueList[0].value.u.timeVal=%ld\n", (long) (values->fieldValueList[0].value.u.timeVal+atof(nodeValue)*60*60));
					*/
					value->u.timeVal = (long)(values->fieldValueList[0].value.u.timeVal + atof(nodeValue)*60*60); /* SLA is given in hours */					
					logmgr_writef(NULL, "ARS: writing timestamp on ARS: '%ld'\n ", value->u.timeVal);
					free(fields);
					free(values);
				}
			} else if (!strcasecmp(fieldType, "integer")) {
				value->dataType = AR_DATA_TYPE_INTEGER;
				value->u.intVal = atoi(nodeValue);
			} else if (!strcasecmp(fieldType, "real")) {
				value->dataType = AR_DATA_TYPE_REAL;
				value->u.realVal = atof(nodeValue);
			} else {
				logmgr_writef(NULL, "ARS: WARNING: Type of primitive field '%s' defaults to string", nfl->nodeArray[i]->name);
				value->dataType = AR_DATA_TYPE_CHAR;
				value->u.charVal = nodeValue;
			}
		}
		xmlFree(dataType);
		xmlFree(dataValue);		
		j++;
	}	

	
	/* We will get the entry corresponding to the id we got on notification. We will, so, search only 1 field (EntryId) */
	n.idlist.entryIdList = (AREntryIdType *) n.EntryID;
	n.idlist.numItems = 1;	
	
	/* Finally, get the entry from ars server*/
	trim_string(n.Schema);
	trim_string(n.EntryID);
	/***********************/	
	printf(" *******************      GONNA WRITE THESE VALUES...");
	{
		char fvalue[1024];
		printf("\n\n%d\n\n\n", nfl->values->numItems);
		for (i=0; i<nfl->values->numItems; i++)
			ars_format_arsvalue(a, fvalue, 1023, i, nfl);
	}
	printf("  ... TO ARS. ***********************************");
	fflush(stdout);
	/* Get the entry values */	
	ARSetEntry( &(a->ctrl), n.Schema, 
		&(n.idlist), 
		nfl->values, time(NULL), AR_JOIN_SETOPTION_REF, /**/
		&(a->status));	
	logmgr_writef(NULL, "\nARS: REQUEST: ARSetEntry status result:");
	ars_printStatusList(a, a->status);
	/***********************/	
	/* Now, format values and update the primitive with the values we got from ars */
	if (nfl->values->numItems != nfl->num_fields) {
		logmgr_writef(NULL, "ARS: REQUEST: FATAL: Number of values received from ars (%d) is diferent from number of values asked (%d)! Be sure the specified ars fields ID exists!",
			nfl->values->numItems, nfl->num_fields);
		return 0;
	}
	ars_nfl_destroy(nfl);
	return 1;
}

int ars_getRecord(ars *a, primitive *p, notification n, char  *configpath) {
	char tempBuffer[1024];
	char *dataType, *dataValue;
	int i, j;
	char nodepath[1024];
	node_field_list *nfl;
	char childpath[1024];
		
	nfl =  ars_nfl_init();	
	/***********************/
	/* Set names structure to call  ARGetEntry */			
	nfl->num_nodes = 0;	
	strcpy(nfl->nodespath, configpath);
	if (nfl->nodespath[strlen(nfl->nodespath)]!='/')
		strcat(nfl->nodespath, "/");	
	strcat(nfl->nodespath, "*");
	nfl->num_nodes = configmgr_getNodes(p->cfg, nfl->nodespath, &(nfl->nodeArray));	
	/* 
	 * First, count nodes with type "field". 
	 * Nodes with this type will have its contents replaced by values received from ARS 
	 */
 	nfl->num_fields=0;
	for (i=0; i<nfl->num_nodes; i++) {
		dataType = xmlGetProp(nfl->nodeArray[i], "data_type");
		dataValue = xmlGetProp(nfl->nodeArray[i], "data_value");
		if  ((dataValue==NULL) || (dataType==NULL)) {
			strcpy(childpath, configpath);
			if (childpath[strlen(childpath)]!='/')
				strcat(childpath, "/");	
			strcat (childpath, nfl->nodeArray[i]->name);
			ars_getRecord(a, p, n, childpath);
			continue;
		}				
		if (!strcasecmp(dataType, "field")) nfl->num_fields++;
		xmlFree(dataType);	
	}
	/* Number of field now is knew, format everything and send to ARSGetEntry */	
	nfl->fields = (ARInternalIdList *) malloc(sizeof(ARInternalIdList));
	nfl->values	=NULL;
	nfl->fields->numItems = nfl->num_fields;
	nfl->fields->internalIdList = (ARInternalId *) malloc(nfl->fields->numItems*sizeof(ARInternalId));
	j=0;
	for (i=0; i<nfl->num_nodes; i++) {
		dataType = xmlGetProp(nfl->nodeArray[i], "data_type");
		if (dataType==NULL)  continue;		
		dataValue = xmlGetProp(nfl->nodeArray[i], "data_value");
		if (dataValue==NULL) continue;
		logmgr_writef(NULL, "ARS: REQUEST: NODE NAME='%s' data_type='%s' data_value='%s'", nfl->nodeArray[i]->name, dataType, dataValue);
		if (!strcasecmp(dataType, "field")) {
			nfl->fields->internalIdList[j++]= atoi(dataValue); 
		}
		xmlFree(dataType);
		xmlFree(dataValue);
	}
	/* Alloc memory to store result */
	nfl->values = (ARFieldValueList *) malloc(sizeof(ARFieldValueList));
	nfl->values->numItems = nfl->fields->numItems;
	nfl->values->fieldValueList = (ARFieldValueStruct *) malloc(nfl->values->numItems * sizeof(ARFieldValueStruct));	
	for (i=0; i<nfl->num_nodes; i++) {
		nfl->values->fieldValueList[i].fieldId = nfl->fields->internalIdList[i];
	}
	
	/* We will get the entry corresponding to the id we got on notification. We will, so, search only 1 field (EntryId) */
	n.idlist.entryIdList = (AREntryIdType *) n.EntryID;
	n.idlist.numItems = 1;	
	
	/* Finally, get the entry from ars server*/
	trim_string(n.Schema);
	trim_string(n.EntryID);
	/***********************/		
	/* Get the entry values */	
	logmgr_writef(NULL, "ARS: REQUEST: Accessing EntryID from Noification");
	ARGetEntry( &(a->ctrl), n.Schema, 
		&(n.idlist), 
		nfl->fields, nfl->values,
		&(a->status));	
	logmgr_writef(NULL, "\nARS: REQUEST: ARGetEntry status result:");
	ars_printStatusList(a, a->status);	
	/***********************/	
	/* Now, format values and update the primitive with the values we got from ars */
	if (nfl->values->numItems != nfl->num_fields) {
		logmgr_writef(NULL, "ARS: REQUEST: FATAL: Number of values received from ars (%d) is diferent from number of values asked (%d)! Be sure the specified ars fields ID exists!",
			nfl->values->numItems, nfl->num_fields);
		return 0;
	}
	j=0;		
	for (i=0; i<nfl->num_nodes; i++) {
		dataType = xmlGetProp(nfl->nodeArray[i], "data_type");
		if (dataType==NULL) continue;
		dataValue = xmlGetProp(nfl->nodeArray[i], "data_value");
		if (dataValue==NULL) continue;	
		/* ******** format according each possible primitive type */
		if (!strcasecmp(dataType, "field")) {
			/* Format the value by type and place formated string on buffer */
			if (!ars_format_arsvalue(a, tempBuffer, sizeof(tempBuffer), j++, nfl)) {
				logmgr_writef(NULL, "ARS: REQUEST: Can't format data_type attribute on primitive node '%s'!", nfl->nodeArray[i]->name);
				return 0;
			}			
		} else if (!strcasecmp(dataType, "value")) {
			/* It is simple the own value */
			strcpy(tempBuffer, dataValue);
		} else if (!strcasecmp(dataType, "timestamp")) {
			/* Pass the timestamp got from notification */
			if (strlen(dataValue)>0) {
				if (!ars_format_datetime(a, tempBuffer, sizeof(tempBuffer), dataValue, n.timestamp)) {
					logmgr_writef(NULL, "ARS: REQUEST: Can't format timestamp primitive node '%s'. Be sure the format is correct!", nfl->nodeArray[i]->name);
					return 0;
				}	
			} else {
				sprintf(tempBuffer, "%ld", n.timestamp);
			}						
		} else {
			logmgr_writef(NULL, "ARS: REQUEST: INVALID PRIMITIVE DATA TYPE '%s'!", dataType);
			return 0;
		}
		/* ******** Data formated - Change the primitive */
		logmgr_writef(NULL, "ARS: REQUEST: NODE NAME='%s' data_type='%s' data_value='%s'", nfl->nodeArray[i]->name, dataType, tempBuffer);
		strcpy(nodepath, configpath);
		if (nodepath[strlen(nodepath)]!='/')
			strcat(nodepath, "/");	
		strcat(nodepath, nfl->nodeArray[i]->name);
		if (!configmgr_setNodeContent(p->cfg, nodepath, tempBuffer)) {
			logmgr_writef(NULL, "ARS: REQUEST: Error setting request data! NodePath='%s', value='%s'", nodepath, tempBuffer);
			return 0;
		}		
		xmlFree(dataType);
		xmlFree(dataValue);
	}
		
	/* Free memory allocated to store field IDs*/
	free (nfl->fields);
	free (nfl->values);
	ars_nfl_destroy(nfl);
	return 1;
}

int ars_format_datetime(ars *a, char *value, int valuelen, char *format, long timestamp) {
	struct tm *dt;
	printf("TIMESTAMP = %ld\n", timestamp);
   	dt = localtime(&timestamp);
   	strftime(value, valuelen, format, dt);
   	return 1;
}

int ars_format_arsvalue(ars *a, char *fvalue, int fvaluelen, int index, node_field_list *nfl) {
	ARValueStruct value;
	configmgr *cfg;
	char dateformat[10240];
	cfg = configmgr_getglobal();
	value = nfl->values->fieldValueList[index].value;
	switch(value.dataType) {
       case AR_DATA_TYPE_NULL:
	 		sprintf( fvalue, "%s",  "" );
	 		logmgr_writef(NULL, "ARS: \tvalue type=NULL (%d) value='%s'", value.dataType, "NULL");
	 		break;
       case AR_DATA_TYPE_KEYWORD: case AR_DATA_TYPE_INTEGER:
       case AR_DATA_TYPE_ENUM:        case AR_DATA_TYPE_BITMASK:
	 		sprintf( fvalue, "%ld", value.u.ulongVal );
	 		logmgr_writef(NULL, "ARS: \tvalue type=INT (%d) value='%ld'", value.dataType, value.u.ulongVal);
	 		break;
       case AR_DATA_TYPE_REAL:
	 		sprintf(fvalue,"%f", value.u.realVal);
	 		logmgr_writef(NULL, "ARS: \tvalue type=REAL (%d) value='%lf'", value.dataType, value.u.realVal);
	 		break;
       case AR_DATA_TYPE_CHAR:  case AR_DATA_TYPE_DIARY: case AR_DATA_TYPE_DECIMAL:
			sprintf(fvalue, "%s", value.u.charVal );
			logmgr_writef(NULL, "ARS: \tvalue type=STRING (%d) value='%s'", value.dataType, value.u.charVal);
	 		break; 
       case AR_DATA_TYPE_TIME:
       		/* Get date time format string from configuration */
     	  	if (!configmgr_getfirst_string(cfg, dateformat, "//config/datetime_format")) {
				logmgr_writef(NULL, "ARS: \tError reading DATETIME format from default configuration");
				return 0;
			}
			if (!ars_format_datetime(a, fvalue, fvaluelen, dateformat, value.u.timeVal)) {
				logmgr_writef(NULL, "ARS: \tError on formating date time!");
				return 0;
			}	 		
	 		logmgr_writef(NULL, "ARS: \tvalue type=DATETIME (%d) value='%s'", value.dataType, fvalue );
	 		break;  
       case AR_DATA_TYPE_BYTES:
	 		sprintf(fvalue,"%s", "*BYTE LIST");
	 		logmgr_writef(NULL, "ARS: \tvalue type=BYTELIST (%d) value='%s'", value.dataType, "*BYTE LIST");
	 		break;          
       case AR_DATA_TYPE_ATTACH:
	 		sprintf(fvalue,"%s", "*ATTACH");
	 		logmgr_writef(NULL, "ARS: \tvalue type=ATTACH (%d) value='%s'", value.dataType, "*ATTACH");	 		
			break;
       default:
	 		logmgr_writef(NULL, "ARS: \tINVALID DATA TYPE (%d) ", value.dataType);
			return 0;
	 		break;
      }	
      return 1;	
}
void ars_printStatusList(ars *a, ARStatusList status) {
   int j;
	char msg[10240];
	logmgr_writef(NULL, "ARS STATUS: number of messages: %d", status.numItems);
   for (j=0; j< status.numItems; j++) {
	   /* sprintf(msg, "%d item %d\n",  j); */
	   logmgr_write(NULL, msg);
       sprintf(msg, "Type = %d\n", status.statusList[j].messageType);
	   logmgr_write(NULL, msg);
       sprintf(msg, "Num  = %ld\n", status.statusList[j].messageNum);
	   logmgr_write(NULL, msg);
       sprintf(msg, "Text = '%s'\n", status.statusList[j].messageText);
	   logmgr_write(NULL, msg);
   }
}

void trim_string(char *text) {
	int i;
	for (i=strlen(text)-1; i>-1; i--) {			
		/*printf (" %d ", (int)test[i]);*/
		if (text[i]>32) 		
			break;
		text[i]='\0';
	}
}

int ars_processQueue(const char *ars_output_folder, const char *ars_error_folder) {
	DIR *dir;
	struct dirent *current_file;
	char fullpath[10240];
	orderedlist *ol;
	listitem *li;
	primitive *p;
	ars *a; notification n;
		
	ol = orderedlist_init();
	if (ol==NULL) {
		logmgr_write(NULL, "ARS: QUEUE: Error initializing the ordered list!");
		return 0;
	}
	
	dir = opendir(ars_output_folder);
	if (!dir) {
		logmgr_writef(NULL, "ARS: QUEUE: Cannot open directory '%s' to search files on it", ars_output_folder);
		return 0;
	}		
	
	processor_wait_for_no_lock(ars_output_folder);
	processor_write_lock(ars_output_folder);	
	
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
		logmgr_writef(NULL, "ARS: QUEUE: Sending primitive file  '%s'...", orderedlist_get_current_value(ol) );
		/* if needed to move to errores, skip this file */		
		if (processor_move_to_dest_if_needed(orderedlist_get_current_value(ol), 
										ars_output_folder, ars_error_folder)) {
			logmgr_writef(NULL, "ARS: QUEUE: NOT SENDING -> moved primitive file '%s' to folder '%s'", 
							orderedlist_get_current_value(ol),  ars_error_folder);
			li = orderedlist_seek_next(ol);
			continue;
		}		
		fflush(stdout);
		strcpy(fullpath, ars_output_folder);
		strcat(fullpath, orderedlist_get_current_value(ol));
		p = primitive_open(fullpath);	  	
	  	/********************************/
	  	/* Connect to ars from primitive parameters, update ars entry and get response */
	  	
		n = *notification_new_from_primitive(p);
	  	a = ars_init();
	  	if (!ars_connect(a, p, n)) {
	  		logmgr_writef(NULL, "ARS: QUEUE: Can't connect to ARS server to send primitive '%s'", orderedlist_get_current_value(ol));
	  		li = orderedlist_seek_next(ol);
	  		continue;
	  	}  		  	
	  	if (!ars_setRecord(a, p, n, "//primitive/response/")) {
	  		logmgr_writef(NULL, "ARS: QUEUE: Error on sending primitive '%s' to ars ", orderedlist_get_current_value(ol));
	  		processor_move_to_dest(orderedlist_get_current_value(ol), ars_output_folder, ars_error_folder);
	  		li = orderedlist_seek_next(ol);
	  		continue;
	  	}  	
		ars_disconnect(a);
		ars_destroy(a);				
			  	
	  	/********************************/	  	
	  	/* Process response */	  	
	  	
	  	if (unlink(fullpath)!=0) {
				logmgr_writef(NULL, "ARS: QUEUE: FATAL ERROR! Primitive file '%s' could not be deleted! It may be sent again in future!!", 
					orderedlist_get_current_value(ol));	  					
		}
		/**/
	  	/********************************/	  		
	  	/* Ok, next! */
	  	primitive_close(p);
	  	li = orderedlist_seek_next(ol);
	} 
		
	processor_delete_lock(ars_output_folder);
	orderedlist_destroy(ol);
	return 1;
}
