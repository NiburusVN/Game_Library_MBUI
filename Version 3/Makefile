CC ?= gcc
CFLAGS ?= $(INC_FLAGS) -Wall -Wextra -Wno-unused-parameter

EXEC_C = client/client
EXEC_S = server/main

ALL = $(EXEC_C) $(EXEC_S)

all: $(ALL)

client/client: client/client.c client/Cfichier/utils.o
	$(CC) -o $@ $^ $(CFLAGS) -lpthread

client/Cfichier/utils.o: client/Cfichier/utils.c
	$(CC) $(CFLAGS) -c $< -o $@

server/main: server/main.c
	$(CC) -o $@ $^ $(CFLAGS) -lpthread

clean:
	rm -v client/Cfichier/utils.o client/client server/main
