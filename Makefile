CC=gcc
CFLAGS=-Wextra -Wall

tm: main.c treasure-manager.c treasure-manager.h treasure-hub.c treasure-hub.h
	$(CC) $(CFLAGS) -o tm main.c treasure-manager.c treasure-manager.h treasure-hub.c treasure-hub.h

clean:
	rm tm th