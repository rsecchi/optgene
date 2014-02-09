
OUT=opt

SRC= parse.c main.c opt.c cmdline.c proc.c
OBJ= $(SRC:.c=.o)

CFLAGS= -g -O2 -Wall
LDFLAGS= -g

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm *.o

indent:
	indent -kr -i8 *.c *.h

