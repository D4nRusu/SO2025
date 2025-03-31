CC=gcc
CFLAGS=-Wextra -Wall

tm: main.c treasure-manager.c treasure-manager.h
	$(CC) $(CFLAGS) -o tm main.c treasure-manager.c treasure-manager.h

clean:
	rm tm