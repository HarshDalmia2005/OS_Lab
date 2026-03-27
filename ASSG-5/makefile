CC = gcc
CFLAGS = -Wall -g

# Targets
ALL = ludo board players

all: $(ALL)

ludo: ludo.c
	$(CC) $(CFLAGS) -o ludo ludo.c

board: board.c
	$(CC) $(CFLAGS) -o board board.c

players: players.c
	$(CC) $(CFLAGS) -o players players.c

clean:
	rm -f $(ALL)

.PHONY: all clean
