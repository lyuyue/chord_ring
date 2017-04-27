# complier
CC = gcc

# flags
CFLAGS = -Wall -pthread

# source
HOST_SOURCE	= main
CLIENT_SOURCE = client

# target
HOST_TARGET = host
CLIENT_TARGET = client

all:$(HOST_TARGET)

$(HOST_TARGET): $(HOST_SOURCE).c $(CLIENT_SOURCE).c
	$(CC) $(CFLAGS) $(HOST_SOURCE).c join.c query.c helper.c -o $(HOST_TARGET)
	$(CC) $(CFLAGS) $(CLIENT_SOURCE).c -o $(CLIENT_TARGET)

clean:
	rm $(HOST_TARGET)
	rm $(CLIENT_TARGET)
