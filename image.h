#ifndef IMG_HEAD_
#define IMG_HEAD_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

#endif
