#include <notification.h>
#include <logmgr.h>

char *notification_strtok(notification *notif, char *ptr, const char delimiter) {
	static char* oldptr;
	char *ptr2;	
	if (ptr==NULL) ptr=oldptr;
	ptr2=ptr;
	while ( (ptr[0]!=delimiter)	&& (ptr[0]!='\0') ) ptr++;
	oldptr=ptr;	
	if (oldptr[0]==delimiter) {
		oldptr[0]='\0';
		oldptr++;	
	}
	return ptr2;	
}
int notification_parse(notification *notif, char *buffer) {
	char *ptrtemp, *token;
	int tipo_msg, tam_msg;		
	char delimitador=NT_NOTIFY_STRING_SEP;	
	ptrtemp = buffer; /*Copia o ponteiro*/
	/*
	*  Exemplo de string que queremos extrair:
	*  1076335012\2\20\teste de notificacao\123456789012345123456789012345678901234567890hostname'
	*  delimitador nesse caso é \
	*  timestamp\tipo de notificação\tamanho da mensagem\texto da mensagem(código, 10 ou 11)\##EntryID(15)+SchemaName(30)+Hostname(até \0)
	*  Para o código, 10 significa criação de registro
	*  11 significa alteração
	*/ 
	/*//Agora faz o parse da notificação	
	//Processa primeira parte, timestamp*/
	token = notification_strtok(notif, ptrtemp, delimitador);	
	notif->timestamp = atoi(token);	
	/*//Verifica o tipo de mensagem. Só nos interessa o tipo 2, ou seja, aquelas provenientes do sistema ARS*/
	token = notification_strtok(notif, NULL, delimitador);	
	tipo_msg = (token!=NULL) ? atoi(token) : 0;
	if (tipo_msg!=2) {
		logmgr_writef(NULL, "NOTIFICATION: Type!=2, type=%d, ignoring", tipo_msg);
		return 0;
	}
	/*//Código (tamanho 2)*/
	token = notification_strtok(notif, NULL, delimitador);
	tam_msg = atoi(token); /*//Vai retornar 2*/
	token = notification_strtok(notif, NULL, delimitador);
	notif->code = atoi(token);
		
	logmgr_writef(NULL, "NOTIFICATION: Código: %s\n", token);	
		
	/*//Resto da mensagem, entry id, schema name e host name
	//Atenção -> considerar offset de 2 caracteres (o ars gera algum código maluco)*/
	token = notification_strtok(notif, NULL, delimitador);
	strncpy(notif->EntryID, token+2, AR_MAX_ENTRYID_SIZE);
	notif->EntryID[AR_MAX_ENTRYID_SIZE]='\0';
	strncpy(notif->Schema, token+2+AR_MAX_ENTRYID_SIZE, AR_MAX_NAME_SIZE);
	notif->Schema[AR_MAX_NAME_SIZE]='\0';
	strcpy(notif->servername, token+2+AR_MAX_ENTRYID_SIZE+AR_MAX_NAME_SIZE);
	notif->servername[AR_MAX_SERVER_SIZE]='\0';
	
	logmgr_writef(NULL, "NOTIFICATION: Entry ID: '%s'\n", notif->EntryID);		
	logmgr_writef(NULL, "NOTIFICATION: Schema: '%s'\n", notif->Schema);	
	logmgr_writef(NULL, "NOTIFICATION: servername:'%s'\n\n", notif->servername);
	
	return 1;
}

notification *notification_new_from_primitive(primitive *p) {
	notification *n;	
	const char ERROR_MSG[]="NOTIFICATION: FATAL ERROR! Error reading notification data from primitive! ";
	n = (notification *) malloc(sizeof(notification));
	
	n->code = configmgr_getfirst_integer(p->cfg, "//primitive/legacy/notifyid");
	n->timestamp = configmgr_getfirst_integer(p->cfg, "//primitive/ident/timestamp");
	if (!configmgr_getfirst_string(p->cfg, n->EntryID, "//primitive/ident/id")) {
		logmgr_writef(NULL, ERROR_MSG);
		return NULL;
	}
	if (!configmgr_getfirst_string(p->cfg, n->Schema, "//primitive/legacy/schema")) {
		logmgr_writef(NULL, ERROR_MSG);
		return NULL;
	}
	printf("\n\nSCHEMA   =  %s\n\n", n->Schema);
	if (!configmgr_getfirst_string(p->cfg, n->servername, "//primitive/legacy/host")) {
		logmgr_writef(NULL, ERROR_MSG);
		return NULL;
	}
	return n;
}


