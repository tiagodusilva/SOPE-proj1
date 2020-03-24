# Based on suckless' st
.POSIX:

CC = gcc
CFLAGS = -Wall # -Werror

SRC = simpledu.c utils.c showDirec.c handleLog.c queue.c
OBJ = $(SRC:.c=.o)

.c.o:
	$(CC) $(CFLAGS) -c $<

simpledu: $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

clean:
	rm -f simpledu $(OBJ)

.PHONY: clean