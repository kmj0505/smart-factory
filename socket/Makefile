#CC:=arm-linux-gcc
CC:=gcc

TARGET_SRV=smart_factory_server
OBJECT_SRV=$(TARGET_SRV).o 

TARGET_CLN=smart_factory_client
OBJECT_CLN=$(TARGET_CLN).o 

TARGET_CLN_SQL=smart_factory_client_sql
OBJECT_CLN_SQL=$(TARGET_CLN_SQL).o 

TARGET_CLN_BT=smart_factory_client_bt
OBJECT_CLN_BT=$(TARGET_CLN_BT).o 

LDFLAGS=-D_REENTRANT -pthread
LDFLAGS_SQL=-D_REENTRANT -pthread -lmysqlclient
LDFLAGS_BT=-l bluetooth -pthread

all : $(TARGET_SRV) $(TARGET_CLN) $(TARGET_CLN_SQL) $(TARGET_CLN_BT)

$(TARGET_SRV):$(OBJECT_SRV)
	$(CC) -o $@ $(OBJECT_SRV) $(LDFLAGS)
$(TARGET_CLN):$(OBJECT_CLN)
	$(CC) -o $@ $(OBJECT_CLN) $(LDFLAGS)
$(TARGET_CLN_SQL):$(OBJECT_CLN_SQL)
	$(CC) -o $@ $(OBJECT_CLN_SQL) $(LDFLAGS_SQL)
$(TARGET_CLN_BT):$(OBJECT_CLN_BT)
	$(CC) -o $@ $(OBJECT_CLN_BT) $(LDFLAGS_BT)
%.o:%.c
	$(CC) -c -o $@ $<
clean:
	rm -f *.o 
clean_all:
	rm -f *.o $(TARGET_SRV) $(TARGET_CLN) $(TARGET_CLN_SQL) $(TARGET_CLN_BT)
	rm -f *.o $(TARGET_SRV) $(TARGET_CLN) $(TARGET_CLN_SQL) $(TARGET_CLN_BT)
