#ifndef __ARS_IF_GINSC_ORDEREDLIST_H__
#define __ARS_IF_GINSC_ORDEREDLIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_ITEM_SIZE 1024

typedef struct _listitem{
	char value[MAX_ITEM_SIZE];
	struct _listitem *next;
} listitem;

/* This class implements a list of ordered strings */
typedef struct _orderedlist{
	listitem *first;
	listitem *current;
} orderedlist;

orderedlist *orderedlist_init(void);
void orderedlist_destroy(orderedlist *ol);
listitem *orderedlist_add_value(orderedlist *ol, char *v);
char *orderedlist_get_current_value(orderedlist *ol);
listitem *orderedlist_seek_next(orderedlist *ol);
listitem *orderedlist_seek_first(orderedlist *ol);

#endif /*__ARS_IF_GINSC_ORDEREDLIST_H__*/
