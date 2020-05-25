
OUT=opt

SRC= parse.c main.c opt.c cmdline.c proc.c
OBJ= $(SRC:.c=.o)

CFLAGS= -g -O0 -Wall 
LDFLAGS= -g -lpthread -lrt

$(OUT): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

plotc: plot_circuit.o
	$(CC) $(LDFLAGS) $^ -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o *~

indent:
	indent -kr -i8 *.c *.h

