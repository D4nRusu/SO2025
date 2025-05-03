CC = gcc
CFLAGS = -Wextra -Wall

SRC = main.c treasure-manager.c treasure-hub.c
HEAD = treasure-manager.h treasure-hub.h

all: tm th

tm: $(SRC) $(HEAD)
	$(CC) $(CFLAGS) -o tm $(SRC)

th: $(SRC) $(HEAD)
	$(CC) $(CFLAGS) -o th $(SRC)

clean:
	rm -f tm th