
OUT=opt

SRC= parse.c main.c
OBJ= $(SRC:.c=.o)


$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

