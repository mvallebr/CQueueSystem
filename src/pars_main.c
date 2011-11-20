#include <pars_main.h>
#include <signal.h>
#include <unistd.h>
#include <ginsc.h>
#include <ars.h>
#include <processor.h>

/* Operation System Signal Handler */
arsmain *gam;
void arsmain_take_signal(int signal) {
	if (signal)
		logmgr_writef(NULL, "MAIN: Signal %d received...\n", signal);
	arsmain_close(gam);	
	exit(0);
}

/* Become a daemon, not ocupating the terminal */
int arsmain_become_daemon(arsmain *am) {
	/*become daemon*/
   if (fork()>0)  exit(0);
   setsid();
   if (fork()>0) exit(0);
   chroot("/");
   return 1;
}

/* Init operations, connect to servers, alloc memory, etc. */
arsmain *arsmain_open(int argc, char *argv[]) {
	arsmain *am;
	primitivelist *primitives;
	char primitive_folder[10240];
	am = (arsmain *) malloc(sizeof(arsmain));
	if (!am) return NULL;
	
	/* Load configuration */
	/* Get a copy of configuration, to use on other functions */
	am->cfg = configmgr_getglobal();
	if (!configmgr_parsecmdline(am->cfg, argc, argv)) {
		fprintf(stderr, "MAIN: Error opening config file, quiting!\n");
		exit(-1);
	}
	/* Open log system */
	if (!logmgr_open(NULL)) {
		fprintf(stderr, "MAIN: Error opening log file, quiting!\n");
		exit(-1);
	}	
		
	logmgr_write(NULL, "\n=================================\n");
	
	/* Become a daemon */
 	if (!arsmain_become_daemon(am)) {
		logmgr_write(NULL, "MAIN: Cannot become daemon!!\n");
		exit(-1);
	}
	
	/* Activate signal handling */
   	signal(15, arsmain_take_signal);    
   	/* Connect to notifier */
  	am->n = notifier_init();
  	printf("2   %p \n", am->n);
   	fflush(stdout);
   	if (!notifier_connect(am->n)) {
   		logmgr_write(NULL, "MAIN: Error connection to notifier server, quiting!\n");
   		exit(-1);
   	}
   	printf("3\n");
   	fflush(stdout);
   	
   	/* Load the configuration of each primitive */
   	primitives = primitivelist_getglobal();
   	configmgr_getfirst_string(am->cfg, primitive_folder, "//config/cfg_primitives_folder");
   	logmgr_writef(NULL, "MAIN: Loading primitives from folder '%s'", primitive_folder);   	
   	if (!primitivelist_readfolder(primitives, primitive_folder)) {
   		logmgr_write(NULL, "MAIN: Cannot read primitives configuration!!\n");
		exit(-1);
	}
   	
   	/* Process ginsc queue */
   	if (!arsmain_process_ginsc_queue(am)) {
   		logmgr_write(NULL, "MAIN: Cannot process primitives GINSC queue !!\n");
		exit(-1);
	}   	
   	   	
	return am;
}

/* Free memory, finish operations, disconnect from servers, etc */
void arsmain_close(arsmain *am) {
	notifier_disconnect(am->n);
	primitivelist_empty_list(primitivelist_getglobal());
	free(am);	
	logmgr_write(NULL, "\n=================================\n");
}

/* Process ginsc queue */
int arsmain_process_ginsc_queue(arsmain *am) {
	ginsc *g;	
	processor *p;	
	int rvalue;
	/* Open processor */
	p = processor_init();

	/* GINSC queue */
	g = ginsc_init();
	if (!ginsc_connect(g)) {
		logmgr_write(NULL, "MAIN: Error! Cannot open communication with ginsc. Verify config file!");
		return 0;
	}
	rvalue = ginsc_processQueue(g, p->pginsc_output_folder, p->pginsc_error_folder, p->ars_output_folder);
	ginsc_disconnect(g);	
	ginsc_destroy(g);
	
	/* ARS queue - ars_processQueue do the connection */
	rvalue = rvalue && ars_processQueue(p->ars_output_folder, p->ars_error_folder);
	
	/* Close processor */
	processor_destroy(p);
	
	return rvalue;
}

/* MAIN PROGRAM */
int main(int argc, char *argv[])
{	
	gam = arsmain_open(argc, argv);   		   	
   		
   	/* Wait for notifications from ars servers */
   	while (1){
     	if (notifier_waitConnection(gam->n)) {
		  	if(!fork()){
		  		logmgr_writef(NULL, "MAIN: ------------------------------------START OF PROCESS %d", getpid());
		    	notifier_receiveData(gam->n);
		    	logmgr_writef(NULL, "MAIN: ------------------------------------END OF PROCESS %d", getpid());
		      	return 0;
	         } 
     	}
  	}	
		
	return 0;
}
