CC=gcc -std=gnu99
CFLAGS=-Wall -Wextra -pedantic -g `pkg-config --cflags gtk+-3.0`
LDFLAGS=`pkg-config --libs gtk+-3.0`
OBJ=main.o
NAME=fluxstyle

all: $(OBJ)
	$(CC) -o $(NAME) $^ $(LDFLAGS)

main.o: src/main.c
	$(CC) -c $(CFLAGS) $^

clean:
	rm -f $(NAME) $(OBJ)
