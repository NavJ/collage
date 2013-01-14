#ifndef IMAGE_UTIL_
#define IMAGE_UTIL_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

#include "lib/uba.h"
#include "lib/xalloc.h"
#include "lib/contracts.h"
#include "lib/stb_image.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image_write.h"

typedef uint32_t pixel;
typedef unsigned char rgb;

struct img_t {
  uint32_t w;
  uint32_t h;
  pixel *data;
};

typedef struct img_t img;

struct img_data_t {
  pixel *avg;
  char *filename;
};

typedef struct img_data_t img_data;

bool is_img(img *I);
bool is_square(img *I);

img *image_load(char *filename, unsigned int min_w, unsigned int min_h);
img *image_new(unsigned int width, unsigned int height);
void image_free(img *I);
void image_save(img *I, char *filename);

rgb get_red(pixel p);
rgb get_green(pixel p);
rgb get_blue(pixel p);
pixel make_pixel(rgb r, rgb g, rgb b);

unsigned int get_y(unsigned int width, unsigned int index);
unsigned int get_x(unsigned int width, unsigned int index);
unsigned int get_index(unsigned int width, unsigned int x, unsigned int y);
pixel *get_pixel(img *I, unsigned int x, unsigned int y);
pixel get_average(img *I);

img *shrink(img *I, unsigned int new_w, unsigned int new_h);
img *crop(img *I, unsigned int new_w, unsigned int new_h);
img *crop_to_square(img *I);
img *standardize(img *I, unsigned int size);
img *stitch(img *I[], unsigned int im_w, unsigned int im_h);

void dump_folder_data(const char *dirname, img_data *I[], unsigned int len);
img_data **parse_folder_data(const char *dirname, unsigned int *loaded);
img_data **parse_folder(const char *dirname,
			unsigned int *loaded,
                        unsigned int min_w,
			unsigned int min_h);
img *make_collage(img *source,
		  img_data *I[],
		  unsigned int I_len,
		  unsigned int std);

#endif
