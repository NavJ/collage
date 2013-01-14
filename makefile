CC=gcc
CCFLAGS=-g -Wall -Wextra -std=c99 -pedantic -lm
DEBUGFLAGS=-DDEBUG
GPFLAGS=-pg
ALLDEPS=$(wildcard lib/*.h) $(wildcard lib/*.c) $(wildcard *.h) $(wildcard *.c)

img: $(ALLDEPS)
	$(CC) $(DEBUGFLAGS) $(CCFLAGS) -o img lib/xalloc.c lib/uba.c image_util_test.c
img-u: $(ALLDEPS)
	$(CC) $(CCFLAGS)  -o img-u lib/xalloc.c lib/uba.c image_util_test.c
img-gp: $(ALLDEPS)
	$(CC) $(DEBUGFLAGS) $(CCFLAGS) $(GPFLAGS) lib/xalloc.c lib/uba.c image_util_test.c
