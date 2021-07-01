CC=gcc
CFLAGS=-std=c99 -Wall
LDFLAGS=-ledit -lm
SOURCES=lispy.c readline.c mpc.c

all: lispy

lispy: $(SOURCES)
	$(CC) $(CFLAGS) -o lispy $(SOURCES) $(LDFLAGS)

clean:
	-rm lispy