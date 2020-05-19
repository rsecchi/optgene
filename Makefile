
OUT=opt

SRC= parse.c main.c opt.c cmdline.c proc.c
OBJ= $(SRC:.c=.o)

CFLAGS= -g -O0 -Wall
LDFLAGS= -g

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

plotc: plot_circuit.o
	$(CC) $(LDFLAGS) $^ -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm *.o *~

indent:
	indent -kr -i8 *.c *.h

