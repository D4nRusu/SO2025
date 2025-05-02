CC=gcc
CFLAGS=-Wextra -Wall

tm: main.c treasure-manager.c treasure-manager.h
	$(CC) $(CFLAGS) -o tm main.c treasure-manager.c treasure-manager.h

th: main.c treasure-hub.c treasure-hub.h
	$(CC) $(CFLAGS) -o th main.c treasure-hub.c treasure-hub.h

clean:
	rm tm th