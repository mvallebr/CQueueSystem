#ifndef __DDR_GINSCPROC_F1_GINSC_H__
#define __DDR_GINSCPROC_F1_GINSC_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int ginsc_proc_connect();
int ginsc_proc_disconnect();
int ginsc_proc_sendData(char *valores[11][50], int numCampos, char *sqllog_file);
int ginsc_proc_llamarServlet(char *pSistema , char *pServicio , long pParametro , char *pInfo );
int escrever_sqllog(char *texto, int len);


#endif /*__DDR_GINSCPROC_F1_GINSC_H__*/
