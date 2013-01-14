CC=gcc
CCFLAGS=-g -Wall -Wextra -std=c99 -pedantic -lm
DEBUGFLAGS=-DDEBUG
GPFLAGS=-pg
ALLDEPS=$(wildcard lib/*.h) $(wildcard lib/*.c) $(wildcard *.h) $(wildcard *.c)
LINKS=lib/xalloc.c lib/uba.c lib/stb_image.c image_util.c image_util_test.c

img: $(ALLDEPS)
	$(CC) $(DEBUGFLAGS) $(CCFLAGS) -o img $(LINKS)
img-u: $(ALLDEPS)
	$(CC) $(CCFLAGS)  -o img-u $(LINKS)
img-gp: $(ALLDEPS)
	$(CC) $(DEBUGFLAGS) $(CCFLAGS) $(GPFLAGS) $(LINKS)
