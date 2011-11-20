#include <ordered_list.h>

orderedlist *orderedlist_init(void) {
	orderedlist *ol;
	ol = (orderedlist *) malloc(sizeof(orderedlist));
	ol->first = NULL;
	ol->current =NULL;	
	return ol;
}

void orderedlist_destroy(orderedlist *ol) {
	listitem *item, *nitem;
	item = ol->first;
	while (item != NULL) {
		nitem=item->next;
		free(item);
		item = nitem;
	}
	free(ol);
}

listitem *orderedlist_add_value(orderedlist *ol, char *v) {
	listitem *newitem, *item;
	newitem = (listitem *) malloc(sizeof(listitem));
	newitem->next=NULL;
	strcpy(newitem->value, v);
	if (ol->first==NULL) {
		ol->first = ol->current = newitem;
	} else {		
		if (strcmp(v, ol->first->value)<0) { /* String comes before first item */
			item = ol->first;
			ol->first = newitem;
			ol->first->next = item;
		} else { /* String comes after first item */
			item = ol->first;
			/*Find place where item should be placed*/
			while (item->next!=NULL) {
				if (strcmp(v, item->value)<0) {
					break;
				}
				item = item->next;
			}
			newitem->next = item->next;
			item->next = newitem;
		}			
	}
	return  newitem;
}

char *orderedlist_get_current_value(orderedlist *ol) {
	if (ol->current==NULL) {
		return NULL;
	}
	return ol->current->value;
}

listitem *orderedlist_seek_next(orderedlist *ol) {
	if (ol->current==NULL) return NULL;
	ol->current = ol->current->next;
	return ol->current;
}

listitem *orderedlist_seek_first(orderedlist *ol) {
	return ol->first;	
}



