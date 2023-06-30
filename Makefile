OBJS	= dataServer.o remoteClient.o queue.o
OUT		= dataServer remoteClient

all: dataServer remoteClient

dataServer: server.c queue.c
	gcc -o dataServer server.c queue.c -lpthread

remoteClient: remoteClient.c
	gcc -o remoteClient remoteClient.c

clean:
		rm -f $(OBJS) $(OUT)