CC = gcc
CFLAG = -O2 -fPIC -Wall
AR = ar rc

BASE_DIR = ./
PG_INCLUDE_DIR=./postgresql
SQLSVR_INCLUDE_DIR=./sqlserver
LIB_DIR = ./lib
EXT_CFLAGS = -I$(BASE_DIR) -I/usr/local/include -I$(PG_INCLUDE_DIR) -I$(SQLSVR_INCLUDE_DIR)
EXT_LDFLAGS = -L$(LIB_DIR) -L/usr/local/lib -L/usr/local/freetds/lib -L/usr/local/pgsql/lib -ltsbase -lpthread -lz -lssl -lcrypto -lm -liconv -lsybdb -lpq -ldl -Wl,-R /usr/local/lib

INCLUDES += $(EXT_CFLAGS)
CFLAGS   = $(INCLUDES) $(CFLAG)

LIBNAME  = libcdbl.a
LIBOBJ = cdbl.o   
LIBSRCS = cdbl.c

all: $(LIBNAME)

$(LIBOBJ):	$(LIBSRCS)
	$(CC) $(CFLAGS) -o $(LIBOBJ) -c $(LIBSRCS)

$(LIBNAME):	$(LIBOBJ)
	$(AR) $@ $(LIBOBJ)

clean:
	rm -f $(LIBNAME)  
	rm -f *.o *.gch *.plist

