#include <notifier.h>
#include <logmgr.h>
#include <config_mgr.h>


notifier *n;

int main(int argc, char *argv[]) {
   configmgr *cfg;
   char ch;
   char tipo[10], id[50], str_notif[200];
   char host[1024], user[1024];
   	cfg = configmgr_getglobal();
	if( !configmgr_parsecmdline(cfg, argc, argv) )
		return -1;
	
   	n = notifier_init();
   	/* notifier_connect(n); */

   	printf("\nComandos:\n");
   	printf("\nn -> envia notificacao.");
   	printf("\ne -> sai do programa.");
   	while( 1 ) {
    	printf("\nDigite o comando: "); 
      	ch =  getchar();

      	if( ch == 'n' || ch == 'N' ) {
	  		printf("\nEnviando notificacao");
	  		printf("\nTipos possíveis: 10->Solicitação 11->Movimento");
	  		printf("\nTipo: ");
	  		scanf("%s", tipo);
	  		printf("\nRecordID: ");
	  		scanf("%s", id);
			printf("\nNotifier Host: ");
	  		scanf("%s", host);
	  		printf("\nNotifier User: ");
	  		scanf("%s", user);
	  		
	  		sprintf(str_notif, "01%s   Solicitação de serviço        10.95.2.157", id );
	  		printf("\nConfirmar (s/n): ");
	  		ch='\0';
	  		while ((ch!='s') && (ch!='S') && (ch!='n') && (ch!='N'))
	  			ch = getchar();
	  		
	  		printf("ch='%c'\n", ch);
	  		if ((ch == 's') || (ch == 'S')){
	  			printf("Sending: host='%s' user='%s'  tipo='%s' str_notif='%s' \n", host, user,  tipo, str_notif);
            	if (NTNotificationServer(host, user,  tipo, 2, str_notif, &(n->status) )<2) 
			      	printf("\nConfirmado envio.");
			     else 
			     	fprintf(stderr, "ERRO NO ENVIO!\n");
            	printf("\n");
	      		printf(str_notif);
	      		printf("\n");
				/*			NTNotificationServer(globalcfg.notifier_user.host, globalcfg.notifier_user.us, "10", 2,
							"01PEE-00000016   Solicitação de serviço        10.95.2.157", &(n->status) );
				*/	      
				printf("Notification Status: numitens=%d\n", n->status.numItems);
		      	notifier_printStatusList(n, n->status);
		   	}else{
		    	printf("\nNão Confirmado envio.");
	        }
      	} else if( ch == 'e' || ch == 'E' ) {
	 		printf("\nSaindo do programa test.\n");
        	break;
     	}	
	}

  	/* notifier_disconnect(n); */
   
   return 0;
}
