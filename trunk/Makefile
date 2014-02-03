
OUT=opt

SRC= parse.c main.c opt.c cmdline.c
OBJ= $(SRC:.c=.o)


$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm *.o

