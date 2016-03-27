CC=gcc -std=gnu99
CFLAGS=-Wall -Wextra -pedantic -g `pkg-config --cflags gtk+-3.0`
LDFLAGS=`pkg-config --libs gtk+-3.0`
OBJ=main.o theme.o color.o
NAME=fluxstyle

all: $(OBJ)
	$(CC) -o $(NAME) $^ $(LDFLAGS)

main.o: src/main.c src/theme.h src/color.h
	$(CC) -c $(CFLAGS) $^

theme.o: src/theme.c src/theme.h src/color.h
	$(CC) -c $(CFLAGS) $^

color.o: src/color.c src/color.h
	$(CC) -c $(CFLAGS) $^

clean:
	rm -f $(NAME) $(OBJ) src/*.gch
