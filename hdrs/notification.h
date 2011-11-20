#ifndef __DDR_GINSC_F1_NOTIFICATION_H__
#define __DDR_GINSC_F1_NOTIFICATION_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <ar.h>
#include <arstruct.h>
#include <arextern.h>

#include <nt.h>
#include <nterrno.h>
#include <ntsextrn.h>

#define HOSTNAME  "telesp.hi.inet"
#define USER      "GINSC"
#define PASSWORD  "GINSC"

#define DEFAULT_SERVICE_NAME "ArsDdrNotifBridge"

#define BUFFERSIZE 10240
#define RECV_TIMEOUT 1000 /*in miliseconds*/

enum NOTIFICATION_TYPES  {
	NOTIFICATION_CREATE=10,
	NOTIFICATION_MODIFY=11
};

typedef struct _notification {
	long timestamp;
	int codigo;
	AREntryIdList idlist;
	char EntryID[AR_MAX_ENTRYID_SIZE+1];
	char Schema[AR_MAX_NAME_SIZE+1];
	char servername[AR_MAX_SERVER_SIZE+1];
} notification;


int notification_parse(notification *notif, char *buffer);

#endif /* __DDR_GINSC_F1_NOTIFICATION_H__*/
