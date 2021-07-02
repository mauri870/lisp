CFLAGS=-std=c99 -Wall -g
LDFLAGS?=-ledit -lm
SOURCES=main.c lispy.c readline.c mpc.c

all: lispy

lispy: $(SOURCES)
	$(CC) $(CFLAGS) -o lispy $(SOURCES) $(LDFLAGS)

clean:
	-rm lispy
