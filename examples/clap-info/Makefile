CFLAGS = -I../../include -std=c99 -Wall
LDFLAGS = -ldl

all: clap-info

clap-info: clap-info.c
	gcc $(CFLAGS) $< $(LDFLAGS) -o $@

clean:
	rm -f clap-info
