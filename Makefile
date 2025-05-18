CC = gcc
CFLAGS = -Wextra -Wall

SRC = main.c treasure-manager.c treasure-hub.c
HEAD = treasure-manager.h treasure-hub.h

all: tm th tc

tm: $(SRC) $(HEAD)
	$(CC) $(CFLAGS) -o tm $(SRC)

th: $(SRC) $(HEAD)
	$(CC) $(CFLAGS) -o th $(SRC)

tc: calculate-score.c
	$(CC) $(CFLAGS) -o tc calculate-score.c

clean:
	rm -f tm th tc