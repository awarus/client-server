CC = gcc
CFLAGS = -g -Wall -pthread

CLIENT = client
SERVER = server
PTPOOL = ptpool

all: $(CLIENT) $(PTPOOL) $(SERVER)

$(CLIENT): $(CLIENT).c
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT).c

#lib$(PTPOOL): lib$(PTPOOL).a
#	ar rcs lib$(PTPOOL).a $(PTPOOL).o

$(PTPOOL): $(PTPOOL).c
	$(CC) -pthread -c $(PTPOOL).c -o lib$(PTPOOL).o
	ar rcs lib$(PTPOOL).a lib$(PTPOOL).o

$(SERVER): $(SERVER).c
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER).c lib$(PTPOOL).a

clean:
	rm -f *.o *.a *.so $(CLIENT) $(SERVER)
