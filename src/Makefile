#makefile para projeto ARS_IF_GINSC
# ********* A SER RODADO COM GNU MAKE **********

#Variáveis principais
PROGNAME=pars.exe
OBJS= config_mgr.o logmgr.o notifier.o socket_mgr.o processor.o notification.o ars.o ginsc.o primitive.o primitive_list.o ordered_list.o
CC=gcc
ARS_HOME=/home/ginsc/IF_DDR_GINSC_FASE1D/ars_sun
ARS_HOME=/opt/ars.4.0/api

##############################################################
#INCLUDES
##############################################################
COMMON_INCLUDES= -I. -I/usr/include -I /usr/local/include  --ansi -Wall
XML_INCLUDES= $$(xml2-config --cflags) -I/usr/local/include/nanohttp-1.0/
CSOAP_INCLUDES= -I/usr/include/libxml2/libxml -I/usr/include/libxml2 -I/usr/local/include/libcsoap-1.0/ -I/usr/local/include/nanohttp-1.0/
#$$(pkg-config --cflags libcsoap)
ARS_INCLUDES=-I$(ARS_HOME)/include
CFLAGS= -g $(COMMON_INCLUDES) $(ARS_INCLUDES) $(XML_INCLUDES) $(CSOAP_INCLUDES)
          
##############################################################
#LIBRARIES LINKED
##############################################################
COMMON_LIBS= -L/usr/local/lib -L/usr/lib
XML_LIBS= $$(xml2-config --libs) -lnanohttp-1.0
CSOAP_LIBS=-L/usr/local/lib -lxml2 -lcsoap-1.0 -lnanohttp-1.0
ARS_LIBS=-L$(ARS_HOME)/lib -lar -lnts -lnsl -lpthread
#$$(pkg-config --libs libcsoap)
LDFLAGS  =$(COMMON_LIBS) $(ARS_LIBS) $(XML_LIBS) $(CSOAP_LIBS)

###############################################################
#TARGETS DE COMPILACAO
all: clean pars 


pars: $(OBJS) pars_main.o
	$(CC) $(OBJS) pars_main.o -o $(PROGNAME) $(LDFLAGS)

tests: cls ar_get_entry ar_set_entry send_notif csoap_server 

send_notif: $(OBJS) tests/send_notif.o
	$(CC) $(OBJS) tests/send_notif.o -o tests/send_notif $(LDFLAGS)
	
ar_get_entry:  tests/ar_get_entry.o
	$(CC)  tests/ar_get_entry.o -o tests/ar_get_entry $(LDFLAGS)
	
ar_set_entry: tests/ar_set_entry.o
	$(CC) tests/ar_set_entry.o -o tests/ar_set_entry $(LDFLAGS)
	
csoap_server: $(OBJS) tests/csoap_server.o
	$(CC) $(OBJS) tests/csoap_server.o -o tests/csoap_server $(LDFLAGS)

#Apaga todo o lixo
clean: cls
	rm -f *.o *~  $(PROGNAME) core tests/*.o tests/*~
	
cls: 
	clear

dist: all
	rm -fr ../dist
	mkdir ../dist
	mkdir ../dist/bin
	mkdir ../dist/pcfg
	mkdir ../dist/poutput
	mkdir ../dist/perrors
	mkdir ../dist/aoutput
	mkdir ../dist/aerrors
	chmod +x $(PROGNAME)
	cp $(PROGNAME) ../dist/bin
	cp if_ginsc_config.xml ../dist/bin
	cp ../pcfg/*.xml ../dist/pcfg

