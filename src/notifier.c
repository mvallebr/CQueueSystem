#include <notifier.h>
#include <processor.h>
#include <logmgr.h>
#include <config_mgr.h>
#include <ginsc.h>

/*Aloca memória para o notificador*/
notifier *notifier_init(void) {
	notifier *n;
	n = (notifier *) malloc(sizeof(notifier));
	n->main_socket = socketmgr_init();
	n->server_list.num_servers = 0;
	
	return  n;
}


/*Libera a memória do notificador*/
void notifier_destroy(notifier *n) {
	socketmgr_destroy(n->main_socket);
   free (n);
}

/*Realiza todas as operações de conexão e inicialização*/
int notifier_connect(notifier *n) {
 	int con_status;
	configmgr *cfg;	
	int i, num_serv;
	configValueStr *temp;
	configValueStr hosts[MAX_SERVERS], users[MAX_SERVERS], passwords[MAX_SERVERS];

	con_status = NTInitializationServer(&(n->status));
	logmgr_writef(NULL, "NOTIFIER: Initialization Status: %d numitens=%d", con_status, n->status.numItems);

   	/* Connect to socket */
  	if(!socketmgr_bindPort(n->main_socket, 0)){
    	logmgr_write(NULL, "NOTIFIER: Error creating socket to service, quiting...");
      	return 0;
   	}
	if(!socketmgr_listenPort(n->main_socket, 0)){
    	logmgr_write(NULL, "NOTIFIER: Error creating socket to service, quiting...");
      	return 0;
   	}   
   	n->port = n->main_socket->port;
  	n->fd   = n->main_socket->fd;
   	logmgr_writef(NULL, "NOTIFIER: Socket created, port=%ld\n", n->port);

	/* Read ist of servers from config file */
	cfg = configmgr_getglobal();
	num_serv = configmgr_getvalues(cfg, &temp, "//config/ars_notifier/host");
	memcpy(hosts, temp, MAX_SERVERS*MAX_STR_SIZE);
	if (num_serv!=configmgr_getvalues(cfg, &temp, "//config/ars_notifier/user")) {
		logmgr_write(NULL, "NOTIFIER: Error on parsed xml config file! Quiting...");
		return 0;
	}
	memcpy(users, temp, MAX_SERVERS*MAX_STR_SIZE);
	if (num_serv!=configmgr_getvalues(cfg, &temp, "//config/ars_notifier/password")) {
		logmgr_write(NULL, "NOTIFIER: Error on parsed xml config file! Quiting...");
		return 0;
	}
	memcpy(passwords, temp, MAX_SERVERS*MAX_STR_SIZE);
	/* Now, connect to each server */
	for (i=0; i<num_serv; i++) {		
		strcpy(n->server_list.server[i].host, hosts[i]);
		strcpy(n->server_list.server[i].user, users[i]);
		strcpy(n->server_list.server[i].password, passwords[i]);
		n->server_list.num_servers++;
		logmgr_writef(NULL, "\n");
		logmgr_writef(NULL, "NOTIFIER: Connecting to server '%s' (u='%s',p='%s') ...", 
			n->server_list.server[i].host, n->server_list.server[i].user, n->server_list.server[i].password);
	   	
	   	con_status = NTRegisterServer( 
	   		n->server_list.server[i].host, n->server_list.server[i].user, n->server_list.server[i].password,
	   		NT_CLIENT_COMMUNICATION_SOCKET,
			n->port, NT_PROTOCOL_TCP, 1/*unix*/, &(n->status));
		if ( !( n->server_list.server[i].connected = (con_status<2) )) {
			logmgr_writef(NULL, "NOTIFIER: COULD NOT CONNECT TO NOTIFIER SERVER '%s' (u='%s',p='%s') !!!", 
				n->server_list.server[i].host, n->server_list.server[i].user, n->server_list.server[i].password);
		}
		logmgr_writef(NULL, "NOTIFIER: Registration Status: %d numitens=%d", con_status, n->status.numItems);
   		notifier_printStatusList(n, n->status);
   		if (con_status>1)   { /* ars status: 0 = ok, 1= warning, >1 = error */
			logmgr_writef(NULL, "NOTIFIER: Connection to notifier server '%s' returned status %d! ", con_status);
			notifier_disconnect(n);
			return 0;
		}
	}
	
   return 1;
}

/* Finish and disconnect */
int notifier_disconnect(notifier *n){
	int i, discon_status;;
	logmgr_writef(NULL, "\n");
	for (i=0; i<n->server_list.num_servers; i++) {
		if (n->server_list.server[i].connected) {
			logmgr_writef(NULL, "NOTIFIER: Disconnecting from server '%s'... ", n->server_list.server[i].host);
			discon_status = NTDeregisterServer(
										n->server_list.server[i].host, n->server_list.server[i].user, n->server_list.server[i].password,
										&(n->port), &(n->status));
	   		logmgr_writef(NULL, "NOTIFIER: Deregistration Status: %d numitens=%d", discon_status, n->status.numItems);
	   		notifier_printStatusList(n, n->status);
		} else {
			logmgr_writef(NULL, "NOTIFIER: Server '%s' wasn't connected. I won't disconnect so... ", n->server_list.server[i].host);
		}
	}
	logmgr_writef(NULL, "NOTIFIER: All notifier servers has been disconnected!");
   	discon_status = NTTerminationServer(&(n->status));
   	logmgr_writef(NULL, "NOTIFIER: Termination Status: %d numitens=%d", discon_status, n->status.numItems);
   	logmgr_writef(NULL, "\n");

	return 1;
}

int notifier_receiveData(notifier *n){
	int i, j;
	char buffer[BUFFERSIZE];
	struct pollfd ufds;
	processor *p;
	int rvalue=0;
	ginsc *g;
	
	ufds.fd = n->fd;		
	/* Set which events we will monitor for*/
	ufds.events = POLLIN || POLLNVAL;	
	logmgr_write(NULL, "NOTIFIER: Receiving data...");
	/* Alloc a processor */
	p = processor_init();
	while (1) {
		while ((i=poll(&ufds, 1, RECV_TIMEOUT))==0); /* Wait for some event */
		/* Verify the event */
		if (ufds.revents & 	POLLIN) { /* //Evento de entrada de dados*/
  			j = recv(n->fd, buffer, BUFFERSIZE, 0);
			buffer[j]='\0';			
			/* Runs the buffer on processor, if not empty*/
			if (buffer[0]!='\0') {
				logmgr_writef(NULL, "NOTIFIER: data read='%s' data len=%d ", buffer, j);
				processor_run(p, buffer);
			} /* else {
				logmgr_writef(NULL, "Buffer len is 0, not processing...");
			}*/
		}
		if (j<=0) { /* connection finished or "poll" system call returned error */
			logmgr_write(NULL, "NOTIFIER: Connection is over. Quiting...  ");
			break;
		} else { /* write ack back and process queue*/
			logmgr_write(NULL, "NOTIFIER: Sending ACK back...  ");
			write (n->fd, NT_NOTIFY_ACK, strlen(NT_NOTIFY_ACK));
			/* Open ginsc connection and send queue*/
			g = ginsc_init();
			if (!ginsc_connect(g)) {
				logmgr_write(NULL, "PROCESSOR: Error! Cannot open communication with ginsc. Verify config file!");
				return 0;
			}
			rvalue = ginsc_processQueue(g, p->pginsc_output_folder, p->pginsc_error_folder, p->ars_output_folder);
			rvalue = rvalue && ars_processQueue(p->ars_output_folder, p->ars_error_folder);
			ginsc_disconnect(g);
			ginsc_destroy(g);		
		}
	}
	/* free the processor from memory */
	processor_destroy(p);	
	logmgr_write(NULL, "NOTIFIER: Reception complete!\n");
	return rvalue;
}

/* //Fica aguardando pelo servidor de notificações realizar a conexão*/
int notifier_waitConnection(notifier *n){
   logmgr_writef(NULL, "NOTIFIER: Waiting for connection on port %d...", n->main_socket->port);
   if (!(n->fd = socketmgr_waitConnection(n->main_socket) )){
		/*//n->main_socket.aceptarConexion())) {*/
      logmgr_write(NULL, "NOTIFIER: Couldn't accept connection...");
      return 0;
   }
   return 1;
}

/*//Imprime uma statusList do ARS*/
void notifier_printStatusList(notifier *n, NTStatusList status) {
   int j;
	
   for (j=0; j< status.numItems; j++) {		
      logmgr_writef(NULL, "NOTIFIER: Type = %d", status.statusList[j].messageType);	   
      logmgr_writef(NULL, "NOTIFIER: Num  = %d", status.statusList[j].messageNum);	   
      logmgr_writef(NULL, "NOTIFIER: Text = '%s'", status.statusList[j].messageText);	   
   }
	fflush(stdout);	
}
