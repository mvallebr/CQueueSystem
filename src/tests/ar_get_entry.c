#include <ar.h>
#include <arstruct.h>
#include <arextern.h>

#include <stdio.h>
#include <stdlib.h>

void ars_printStatusList(ARStatusList status) {
   int j;
	char msg[10240];
   for (j=0; j< status.numItems; j++) {
       sprintf(msg, "Type = %d\n", status.statusList[j].messageType);
	   printf(msg);
       sprintf(msg, "Num  = %ld\n", status.statusList[j].messageNum);
	   printf(msg);
       sprintf(msg, "Text = '%s'\n", status.statusList[j].messageText);
	   printf(msg);
	   sprintf(msg, "AppText = '%s'\n", status.statusList[j].appendedText);
	   printf(msg);
   }
   printf("\n");
}

int ars_format_arsvalue(ARFieldValueList  *values, int index) {
	ARValueStruct value;
	value = values->fieldValueList[index].value;		
	
   	switch(value.dataType) {
       case AR_DATA_TYPE_NULL:
	 		printf("ARS: \tvalue type=NULL (%d) value='%s'", value.dataType, "NULL");
	 		break;
       case AR_DATA_TYPE_KEYWORD: case AR_DATA_TYPE_INTEGER:
       case AR_DATA_TYPE_ENUM:        case AR_DATA_TYPE_BITMASK:
	 		printf("ARS: \tvalue type=INT (%d) value='%ld'", value.dataType, value.u.ulongVal);
	 		break;
       case AR_DATA_TYPE_REAL:
	 		printf("ARS: \tvalue type=REAL (%d) value='%f'", value.dataType, value.u.realVal);
	 		break;
       case AR_DATA_TYPE_CHAR:  case AR_DATA_TYPE_DIARY: case AR_DATA_TYPE_DECIMAL:
			printf("ARS: \tvalue type=STRING (%d) value='%s'", value.dataType, value.u.charVal);
	 		break; 
       case AR_DATA_TYPE_TIME:       		
	 		printf("ARS: \tvalue type=DATETIME (%d) value='%ld'", value.dataType, value.u.timeVal );
	 		break;  
       case AR_DATA_TYPE_BYTES:
	 		printf("ARS: \tvalue type=BYTELIST (%d) value='%s'", value.dataType, "*BYTE LIST");
	 		break;          
       case AR_DATA_TYPE_ATTACH:
	 		printf("ARS: \tvalue type=ATTACH (%d) value='%s'", value.dataType, "*ATTACH");	 		
			break;
       default:
	 		printf("ARS: \tINVALID DATA TYPE (%d) ", value.dataType);
			return 0;
	 		break;
      }	
      printf("\n");
      return 1;	
}


int main (int argc, char *argv[]) {
	int con_status;
	int i;
	ARControlStruct ctrl;
	ARStatusList status;
	ARInternalIdList  *fields; 
	ARFieldValueList  *values;
	AREntryIdList idlist;
	char SCHEMA[]= "Solicitação de serviço";
	const int num_items = 3;
	
	if (argc<2) {
		fprintf(stderr, "Use this way: \n\t%s ENTRYID \n\n", argv[0]);
		return 1;
	}
	strcpy(ctrl.user, "GINSC");
	strcpy(ctrl.password, "GINSC");
	strcpy(ctrl.server, "bauru.hi.inet");
	
	con_status = ARInitialization(&(ctrl), &(status));
	printf("ARInitialization returned status %d \n", con_status);
	ars_printStatusList(status);
	fields = (ARInternalIdList *) malloc(sizeof(ARInternalIdList));
	fields->numItems = num_items;
	fields->internalIdList = (ARInternalId *) malloc(fields->numItems*sizeof(ARInternalId));
	values = (ARFieldValueList *) malloc(sizeof(ARFieldValueList));
	values->numItems = fields->numItems;
	values->fieldValueList = (ARFieldValueStruct *) malloc(values->numItems * sizeof(ARFieldValueStruct));	
	
	fields->internalIdList[0]= 1;
	fields->internalIdList[1]= 600000028;
	fields->internalIdList[2]= 600000041;
	for (i=0; i<fields->numItems; i++) {
		values->fieldValueList[i].fieldId = fields->internalIdList[i];
	}
	idlist.entryIdList = (AREntryIdType *) malloc(15);
	strcpy(idlist.entryIdList, argv[1]);
	idlist.numItems = 1;
	
	printf("SCHEMA='%s'\n", SCHEMA);
	printf("server='%s'\n", ctrl.server);
	printf("user='%s'\n", ctrl.user);
	printf("password='%s'\n", ctrl.password);
	   
	fflush(stdout);
	con_status = ARGetEntry( &(ctrl), SCHEMA, 
		&(idlist), 
		fields, values,
		&(status));
	printf("ARGetEntry returned status %d \n", con_status);
	ars_printStatusList(status);
	
	printf("Printing values...\n");
	for (i=0; i<values->numItems; i++) {
		ars_format_arsvalue(values, i);
	}	
	
	con_status = ARTermination(&(ctrl), &(status));	
	printf("ARTermination returned status %d \n", con_status);
	ars_printStatusList(status);
	return 0;	
}


