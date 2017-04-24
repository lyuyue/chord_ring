# complier
CC = gcc

# flags
CFLAGS = -Wall -pthread

# source
HOST_SOURCE	= main
# target
HOST_TARGET = host

all:$(HOST_TARGET)

$(HOST_TARGET): $(HOST_SOURCE).c
	$(CC) $(CFLAGS) $(HOST_SOURCE).c join.c query.c helper.c -o $(HOST_TARGET)

clean:
	rm $(HOST_TARGET)
