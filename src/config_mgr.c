#include <config_mgr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*****************/
configmgr globalcfg;
/*****************/

configmgr *configmgr_open() {
	configmgr *c;
	c = (configmgr *) malloc(sizeof(configmgr));
	
	return c;
}

void configmgr_close(configmgr* cfg) {
	free(cfg);
}

configmgr *configmgr_getglobal() {
	return &globalcfg;
}

int configmgr_setDefaults(configmgr *cfg) {
	/*Add some default variable values to the registry*/
	return 1;
}

int configmgr_loadCfg(configmgr *cfg){
	FILE *f;
	int i, loaded;
	char temp[10240];
	#define NUM_CONFIG_FILES 2	
	char file_list[NUM_CONFIG_FILES][200] =  { "if_ginsc_config.xml",
				  ".if_ginsc_config.xml"};
	/* First, open theconfiguration file */
	loaded=0;
	for (i=0; i<NUM_CONFIG_FILES; i++) {
		/* Search on /etc*/
		strcpy(temp, "/etc/");
		strcat(temp, file_list[i]);
		f = fopen(temp, "r");
		printf("CONFIG: trying to open '%s'\n", temp);
		if (f!=NULL) {
			loaded=1;
			printf("CONFIG: Using config file '%s'\n", temp);
			break;
		}		
		/* Search on  /home */
		strcpy(temp, getenv("HOME"));
		strcat(temp, "/");
		strcat(temp, file_list[i]);
		f = fopen(temp, "r");
		printf("CONFIG: trying to open '%s'\n", temp);
		if (f!=NULL) {
			loaded=1;
			printf("CONFIG: Using config file '%s'\n\n", temp);
			break;
		}
		/* Search on current folder */
		strcpy(temp, file_list[i]);
		f = fopen(temp, "r");
		printf("CONFIG: trying to open '%s'\n", temp);
		if (f!=NULL) {
			loaded=1;
			printf("CONFIG: Using config file '%s'\n\n", temp);
			break;
		}
	}
	if (!loaded) {
		fprintf(stderr, "CONFIG: Fatal - can't read any configuration files. Have you created some?\n");
		return 0;
	}
	/* Close the file descritor, once we alredy know it can be read*/
	fclose(f);
	
	return configmgr_parseCfg(cfg, temp);	    
}

int configmgr_parseCfg(configmgr *cfg, char *filename) {
	FILE *f;
	xmlParserCtxtPtr pctx;
	/*Verify if we can read the file... */
	f = fopen(filename, "r");
	printf("CONFIG: trying to open '%s'\n", filename);
	if (f==NULL) {
		printf("CONFIG: Problems to read file '%s'\n\n", filename);
		return 0;
	}
	fclose(f);
	/* Read configuration from file */
	xmlInitParser();
	cfg->doc = NULL;
	cfg->xpathCtx = NULL;
	cfg->xpathObj = NULL;
	cfg->doc = xmlParseFile(filename);
	pctx = xmlNewParserCtxt();
	xmlCtxtReadFile		(pctx, filename, NULL, 0);
	if (cfg->doc == NULL) {
		fprintf(stderr, "CONFIG:  Unable to parse file \"%s\"\n", filename);
		return(0);
	}
	xmlFreeParserCtxt(pctx);
	
	/* Create xPath context */
    cfg->xpathCtx = xmlXPathNewContext(cfg->doc);
    if(cfg->xpathCtx == NULL) {
    	fprintf(stderr,"CONFIG:  Unable to create new XPath context\n");
	    xmlFreeDoc(cfg->doc); 
	    return(0);
	}
	return 1;
}

int configmgr_doc_from_rootnode(configmgr *cfg, xmlNodePtr root_node) {
	/* Read configuration from file */
	xmlInitParser();
	cfg->doc = NULL;
	cfg->xpathCtx = NULL;
	cfg->xpathObj = NULL;
	cfg->doc = xmlNewDoc(BAD_CAST "1.0");
	if (cfg->doc == NULL) {
		fprintf(stderr, "CONFIG:  Unable to create XML document \n");
		return(0);
	}
	if (root_node==NULL) {
		fprintf(stderr, "CONFIG:  Unable to create doc, root_node can't be NULL!\n");
		return(0);
	}	
	xmlDocSetRootElement(cfg->doc, root_node);

	/* Create xPath context */
    cfg->xpathCtx = xmlXPathNewContext(cfg->doc);
    if(cfg->xpathCtx == NULL) {
    	fprintf(stderr,"CONFIG:  Unable to create new XPath context\n");
	    xmlFreeDoc(cfg->doc); 
	    return(0);
	}
	return 1;
}


int configmgr_parsecmdline(configmgr *cfg, int argc, char *argv[]) {	
	/* First, init with defaults */
	configmgr_setDefaults(cfg);
	
	/* Now, overwrite with configuration */
	return configmgr_loadCfg(cfg);
}

int configmgr_getfirst_integer(configmgr *cfg, const char *name) {
	char strvalue[MAX_STR_SIZE];
	if (!configmgr_getfirst_string(cfg, strvalue, name)) return 0;
	return atoi(strvalue);
}

int configmgr_getfirst_string(configmgr *cfg, char *strvalue, const char *name) {
	configValueStr *temp;
		
	if (!configmgr_getvalues(cfg, &temp, name)) {
		fprintf(stderr, "CONFIG: Can't read value of '%s'\n", name);
		return 0;
	}
	strcpy(strvalue, temp[0]);
	return 1;
}

int configmgr_getvalues(configmgr *cfg, configValueStr **values, const char *name) {
	int i;
	char *btemp, boutput[MAX_STR_SIZE];
	int a, b;
	/* Executa a chamada xPath e consegue todos os nodes que correspondem à expressão */
    cfg->xpathObj = xmlXPathEvalExpression(name, cfg->xpathCtx);
    if(cfg->xpathObj == NULL) {
        fprintf(stderr,"CONFIG: Unable to evaluate xpath expression \"%s\"\n", name);
        return(0);
    }
    
    if(cfg->xpathObj->nodesetval == NULL) {
        fprintf(stderr,"CONFIG: Unable to evaluate xpath expression \"%s\"\n", name);
        return(0);
    }
    
	for (i=0; i<cfg->xpathObj->nodesetval->nodeNr;i++) {	
		fprintf(stderr, "CONFIG MGR: %s [%d] = '%s'\n", cfg->xpathObj->nodesetval->nodeTab[i]->name, i,
			xmlNodeGetContent(cfg->xpathObj->nodesetval->nodeTab[i]) );
		btemp = xmlNodeGetContent(cfg->xpathObj->nodesetval->nodeTab[i]);
		a = MAX_STR_SIZE;
		b = strlen(btemp);
		UTF8Toisolat1(boutput, &a, btemp, &b);	
		boutput[a]='\0';
		strncpy(cfg->valueArray[i], 
			  boutput, 
			  MAX_STR_SIZE);
	}
	/* Point the argument to the global cfg array */
	*values = cfg->valueArray;
	
    /* Returns the number of resultng elements */
	return cfg->xpathObj->nodesetval->nodeNr;
}

int configmgr_free(configmgr *cfg) {
    /* Libera memória */
    if (cfg->xpathObj!=NULL)
	    xmlXPathFreeObject(cfg->xpathObj);
	if (cfg->xpathCtx!=NULL)
	    xmlXPathFreeContext(cfg->xpathCtx); 
	if (cfg->doc!=NULL)
	    xmlFreeDoc(cfg->doc); 
	xmlCleanupParser();
	return 1;
}

/* Returns number of corresponding nodes */
int  configmgr_getNodes(configmgr *cfg, const char *xpathName, xmlNodePtr **nodes) {
	/* executes xpath call and get all the names which correspond to the expression */
    cfg->xpathObj = xmlXPathEvalExpression(xpathName, cfg->xpathCtx);
    if(cfg->xpathObj == NULL) {
        fprintf(stderr,"CONFIG: Unable to evaluate xpath expression \"%s\"\n", xpathName);
        return(0);
    }
    if(cfg->xpathObj->nodesetval == NULL) {
        fprintf(stderr,"CONFIG: Unable to evaluate xpath expression \"%s\"\n", xpathName);
        return(0);
    }
    /* Set output array of nodes */
	*nodes = cfg->xpathObj->nodesetval->nodeTab;
    /* Returns the number of resultng elements */
	return cfg->xpathObj->nodesetval->nodeNr;
}

/* Set the primitive node content  */
int  configmgr_setNodeContent(configmgr *cfg, const char *xpathName, const char *content) {
	const char *btemp;
	char boutput[MAX_STR_SIZE];
	int i, j;
	/* executes xpath call and get all the names which correspond to the expression */
    cfg->xpathObj = xmlXPathEvalExpression(xpathName, cfg->xpathCtx);
    if(cfg->xpathObj == NULL) {
        fprintf(stderr,"CONFIG: Unable to evaluate xpath expression \"%s\"\n", xpathName);
        return(0);
    }
    
    if(cfg->xpathObj->nodesetval == NULL) {
        fprintf(stderr,"CONFIG: Unable to evaluate xpath expression \"%s\"\n", xpathName);
        return(0);
    }
    if (cfg->xpathObj->nodesetval->nodeNr!=1 ){
    	return 0;	
    }
    btemp = content;
	i = MAX_STR_SIZE;
	j = strlen(btemp);
	isolat1ToUTF8(boutput, &i, btemp, &j);	
	boutput[i]='\0';
    xmlNodeSetContent(cfg->xpathObj->nodesetval->nodeTab[0], boutput);
	return 1;
}
