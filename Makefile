CC=gcc
CFLAGS=-Wall -g -O3 -I/Applications/Postgres.app/Contents/Versions/9.5/include/
LDFLAGS=
LDLIBS=-lpq -L/Applications/Postgres.app/Contents/Versions/9.5/lib

OBJS=main.o log.o

all: main

main: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: clean
clean:
	-rm main *.o
