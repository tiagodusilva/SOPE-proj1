# Based on suckless' st
.POSIX:

CC = gcc
CFLAGS = -Wall # -Werror

SRC = simpledu.c queue.c utils.c showDirec.c handleLog.c handleSignal.c
OBJ = $(SRC:.c=.o)

.c.o:
	$(CC) $(CFLAGS) -c $<

simpledu: $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

clean:
	rm -f simpledu $(OBJ)

.PHONY: clean