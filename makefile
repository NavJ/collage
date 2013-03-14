CC=gcc
CCFLAGS=-g -Wall -Wextra -std=c99 -pedantic -lm
OPTFLAGS=-Wall -Wextra -std=c99 -pedantic -lm -O3
DEBUGFLAGS=-DDEBUG
GPFLAGS=-pg
ALLDEPS=$(wildcard lib/*.h) $(wildcard lib/*.c) $(wildcard *.h) $(wildcard *.c)
LINKS=lib/xalloc.c lib/uba.c lib/kdtree.c lib/stb_image.c image_util.c image_util_main.c

img: $(ALLDEPS)
	$(CC) $(DEBUGFLAGS) $(CCFLAGS) -o img $(LINKS)
img-u: $(ALLDEPS)
	$(CC) $(OPTFLAGS) -o img-u $(LINKS)
img-gp: $(ALLDEPS)
	$(CC) $(CCFLAGS) $(GPFLAGS) $(LINKS)
