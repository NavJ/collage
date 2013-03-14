#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "image_util.h"

bool is_img(img *I) {
  if (I == NULL) return false;
  if (I->data == NULL) return false;
  return true;
}

bool is_square(img *I) {
  REQUIRES(is_img(I));
  return (I->w == I->h);
}

img *image_load(char *filename, unsigned int min_w, unsigned int min_h) {
  if (filename == NULL) {
    fprintf(stderr, "Filename must not be null.\n");
    exit(EXIT_FAILURE);
  }
  if (strstr(filename, ".jpg") == NULL && strstr(filename, ".png") == NULL &&
      strstr(filename, ".bmp") == NULL && strstr(filename, ".gif") == NULL) {
    fprintf(stderr, "File \"%s\" has an invalid extension.\n", filename);
    return NULL;
  }


  int w,h,n;
  img *I = xmalloc(sizeof(img));

  // last arg forces 4 bytes/pixel regardless of actual size
  I->data = (pixel *) stbi_load(filename, &w, &h, &n, 4);
  I->w = w;
  I->h = h;

  if (I->data == NULL) {
    fprintf(stderr, "Error loading image \"%s\": %s\n", 
	    filename, stbi_failure_reason());
    free(I);
    return NULL;
  }

  if (I->w < min_w || I->h < min_h) {
    //printf("Rejecting image %s: dimensions not met\n", filename);
    image_free(I);
    return NULL;
  }
  //else printf("Image %s loaded successfully!\n", filename);

  ENSURES(is_img(I));
  return I;
}

img *image_new(unsigned int width, unsigned int height) {
  //printf("Allocating image struct.\n");
  img *I = xmalloc(sizeof(img));
  I->w = width;
  I->h = height;
  //printf("Allocating %d pixels for image.\n", width * height);
  I->data = (pixel *) xcalloc(width*height, sizeof(pixel));

  ENSURES(is_img(I));
  return I;
}

void image_free(img *I) {
  REQUIRES(is_img(I));

  stbi_image_free(I->data);
  free(I);
  return;
}

void image_save(img *I, char *filename) {
  REQUIRES(is_img(I));

  if (stbi_write_png(filename, I->w, I->h, 4, I->data, 0) == 0) {
    fprintf(stderr, "Image save to \"%s\" failed.\n", filename);
    exit(EXIT_FAILURE);
  }
  //else printf("Image saved to %s.\n", filename);

  ENSURES(is_img(I));
  return;
}

rgb get_red(pixel p) {
  return ((rgb) ((p >> 16) & 0xFF));
}

rgb get_green(pixel p) {
  return ((rgb) ((p >> 8) & 0xFF));
}

rgb get_blue(pixel p) {
  return ((rgb) (p & 0xFF));
}

pixel make_pixel(rgb r, rgb g, rgb b) {
  return ((pixel) 
	  (0xFF << 24) |
	  (((pixel) r) << 16) |
	  (((pixel) g) << 8) |
	  ((pixel) b));
}

unsigned int get_y(unsigned int width, unsigned int index) {
  return (index / width);
}

unsigned int get_x(unsigned int width, unsigned int index) {
  return (index % width);
}

unsigned int get_index(unsigned int width, unsigned int x, unsigned int y) {
  return ((width * y) + x);
}

pixel *get_pixel(img *I, unsigned int x, unsigned int y) {
  REQUIRES(is_img(I));
  REQUIRES(I->w >= x && I->h >= y);

  unsigned int index = get_index(I->w, x, y);
  //printf("Size: %dx%d, getting pixel %d,%d\n", I->w, I->h, x, y);
  ASSERT(index < I->w * I->h);
  pixel *p = &(I->data[index]);

  ENSURES(is_img(I));
  ENSURES(p != NULL);
  return p;
}

pixel get_average(img *I) {
  REQUIRES(is_img(I));
  unsigned int r=0, g=0, b=0;
  for (unsigned int x = 0; x < I->w; x++) {
    for (unsigned int y = 0; y < I->h; y++) {
      pixel p = *(get_pixel(I, x, y));
      r += get_red(p);
      g += get_green(p);
      b += get_blue(p);
    }
  }
  unsigned int max = I->w * I->h;
  return (make_pixel((rgb) (r / max),
		     (rgb) (g / max),
		     (rgb) (b / max)));
} 

/* Prototype area avg function
pixel area_avg(img *I, unsigned int new_w, unsigned int new_h,
	       unsigned int x, unsigned int y) {
  float width_ratio = I->w / new_w;
  float height_ratio = I->h / new_h;
  float start_x = x * width_ratio;
  float end_x = (x + 1) * width_ratio;
  float start_y = y * height_ratio;
  float end_y = (y + 1) * height_ratio;
  float left_weight = 1 - (start_x - floor(start_x));
  float top_weight = 1 - (start_y - floor(start_y));
  float right_weight = (end_x - floor(end_x));
  float bot_weight = (end_y - floor(end_y));

  float total_weight = 0;
  float avgr, avgg, avgb;
  avgr = 0; avgg = 0; avgb = 0;

  for (int i = floor(start_x); i <= floor(end_x); i++) {
    for (int j = floor(start_y); j <= floor(end_y); j++) {
      float weight = 1;
      if (i == floor(start_x)) weight *= left_weight;
      else if (i == floor(end_x)) weight *= right_weight;
      if (i == floor(start_y)) weight *= top_weight;
      else if (i == floor(end_y)) weight *= bot_weight;
      total_weight += weight;
      pixel p = I->data[get_index(I->w, i, j)];
      avgr += get_red(p) / 255;
      avgg += get_green(p) / 255;
      avgb += get_blue(p) / 255;
    }
  }

  pixel p = 0;
  p = set_red((avgr/total_weight) * 255);
  p = set_green((avgg/total_weight) * 255);
  p = set_blue((avgb/total_weight) * 255);
  return p;
}
*/

pixel area_avg(img *I,
	       unsigned int new_w,
	       unsigned int new_h,
	       unsigned int x,
	       unsigned int y) {
  REQUIRES(is_img(I));
  REQUIRES(x < new_w && y < new_h);
  REQUIRES(I->w >= new_w && I->h >= new_h);
  REQUIRES(I->w * new_h == I->h * new_w); // aspect ratio must be same

  unsigned int total_weight = 0, r_weight = 0, g_weight = 0, b_weight = 0,
  wf = 15; // weight fraction, higher = more precision but more overflow risk

  unsigned int start_x, start_y, end_x, end_y;
  start_x = x * I->w / new_w;
  start_y = y * I->h / new_h;
  end_x = (x + 1) * I->w / new_w;
  end_y = (y + 1) * I->h / new_h;
  /*printf("smaller x,y: %u,%u.
  start_x=%u, end_x=%u, start_y=%u, end_y=%u\n",
  x, y, start_x, end_x, start_y, end_y);*/
  // single edge weights are now from 0 to wf
  unsigned int left_weight, right_weight, top_weight, bot_weight;
  left_weight = wf * (new_w - (x * I->w % new_w)) / new_w;
  right_weight = wf * ((x + 1) * I->w % new_w) / new_w;
  top_weight = wf * (new_h - (y * I->h % new_h)) / new_h;
  bot_weight = wf * ((y + 1) * I->w % new_h) / new_h;
  /*printf("edge weights (lrtb): %u, %u, %u, %u\n", 
  left_weight, right_weight, top_weight, bot_weight);*/

  for (unsigned int i = start_x; i < end_x; i++) {
    for (unsigned int j = start_y; j < end_y; j++) {
      // Reset weight every iteration; each pixel has new weight
      unsigned int weight = 1;

      // area weight ranges from 0 to wf*wf
      if (i == start_x) weight *= left_weight;
      else if (i == end_x) weight *= right_weight;
      else weight *= wf;

      if (j == start_y) weight *= top_weight;
      else if (j == end_y) weight *= bot_weight;
      else weight *= wf;

      total_weight += weight;

      pixel p = *(get_pixel(I, i, j));
      r_weight += weight * ((unsigned int) get_red(p));
      g_weight += weight * ((unsigned int) get_green(p));
      b_weight += weight * ((unsigned int) get_blue(p));
    }
  }
  pixel p = make_pixel((rgb) (r_weight / total_weight),
		       (rgb) (g_weight / total_weight),
		       (rgb) (b_weight / total_weight));

  //printf("Final pixel rgba: %X\n", p);
  ENSURES(is_img(I));
  return p;
}

img *shrink(img *I, unsigned int new_w, unsigned int new_h) {
  REQUIRES(is_img(I));
  REQUIRES(I->w >= new_w && I->h >= new_h);
  REQUIRES(I->w * new_h == I->h * new_w); // aspect ratio must be same

  //printf("Shrinking...");

  img *re = image_new(new_w, new_h);
  for (unsigned int i = 0; i < new_w; i++) {
    for (unsigned int j = 0; j < new_h; j++) {
      *(get_pixel(re, i, j)) = area_avg(I, new_w, new_h, i, j);
    }
  }

  ENSURES(is_img(re));
  ENSURES(re->w == new_w && re->h == new_h);
  ENSURES(is_img(I));
  return re;
}

img *crop(img *I, unsigned int new_w, unsigned int new_h) {
  REQUIRES(is_img(I) && I->w >= new_w && I->h >= new_h);
  unsigned int leftskip = (I->w - new_w) / 2;
  unsigned int topskip = (I->h - new_h) / 2;
  img *crp = image_new(new_w, new_h);

  for (unsigned int y = topskip; y < new_h + topskip; y++) {
    for (unsigned int x = leftskip; x < new_w + leftskip; x++) {
      pixel p = *(get_pixel(I, x, y));
      *(get_pixel(crp, x - leftskip, y - topskip)) = p;
    }
  }

  ENSURES(is_img(I) && is_img(crp));
  ENSURES(crp->w == new_w && crp->h == new_h);
  return crp;
}

img *crop_to_square(img *I) {
  REQUIRES(is_img(I));

  unsigned int dim = I->w > I->h ? I->h : I->w;
  img *crp = crop(I, dim, dim);

  ENSURES(is_img(I));
  ENSURES(is_img(crp) && is_square(crp));
  return crp;
}

img *standardize(img *I, unsigned int size) {
  REQUIRES(is_img(I));

  img *cropped = crop_to_square(I);
  ASSERT(is_img(cropped));
  ASSERT(is_square(cropped));
  img *resized = shrink(cropped, size, size);
  image_free(cropped);

  ENSURES(is_img(I));
  ENSURES(is_img(resized) && is_square(resized));
  return resized;
}

img *stitch(img *I[], unsigned int im_w, unsigned int im_h) {
  REQUIRES(I != NULL);
  REQUIRES(is_img(I[0]) && is_square(I[0]));

  unsigned int w, h, total_w, total_h;
  // Assumes list is standardized to first image's dimensions
  w = I[0]->w;
  h = I[0]->h;
  total_w = w * im_w;
  total_h = h * im_h;

  img *I_done = image_new(total_w, total_h);

  unsigned int x, y, x_off, y_off;

  for (unsigned int i = 0; i < total_h; i++) {
    y = i / h;
    y_off = i % h;
    for (unsigned int j = 0; j < total_w; j++) {
      x = j / w;
      x_off = j % w;
      // Copy the pixel over
      *(get_pixel(I_done, j, i)) =
	*(get_pixel(I[get_index(im_w, x, y)], x_off, y_off));
    }
  }

  return I_done;
}

void dump_folder_data(const char *dirname, img_data *I[], unsigned int len) {
  char *fname = xmalloc(sizeof(char) * (strlen(dirname) + 14));
  fname = strcpy(fname, dirname);
  fname = strcat(fname, "/img_data.dat");

  FILE *f = fopen(fname, "w");
  if (f == NULL) {
    fprintf(stderr, "Failed to dump data to \"%s\".\n", fname);
    return;
  }
  for (unsigned int i = 0; i < len; i++) {
    img_data *id = I[i];
    assert(id != NULL);
    if (id->avg == NULL) fprintf(f, "NULL ");
    else fprintf(f, "%X ", *(id->avg));
    fprintf(f, "%s\n", id->filename);
  }
  if (fclose(f) != 0) {
    printf("Failed to close \"%s\" stream, aborting.\n", fname);
    exit(EXIT_FAILURE);
  }
  printf("Folder data written to \"%s\".\n", fname);
  free(fname);
  return;
}

img_data **parse_folder_data(const char *dirname, unsigned int *loaded) {
  char *fname = xmalloc(sizeof(char) * (strlen(dirname) + 14));
  fname = strcpy(fname, dirname);
  fname = strcat(fname, "/img_data.dat");

  FILE *f = fopen(fname, "r");
  if (f == NULL) {
    fprintf(stderr, "No file \"%s\" exists.\n", fname);
    return NULL;
  }

  char *linebuf = xcalloc(200, sizeof(char));
  char *null_chk = xcalloc(17, sizeof(char));
  uba *U = uba_new(10);
  unsigned int count = 0;
  while (fgets(linebuf, 200, f) != NULL) {
    img_data *id = malloc(sizeof(img_data));
    id->filename = xcalloc(strlen(linebuf), sizeof(char));
    sscanf(linebuf, "%s %[^\n]", null_chk, id->filename);
    if (strcmp(null_chk, "NULL") == 0) id->avg = NULL;
    else {
      id->avg = xmalloc(sizeof(pixel));
      sscanf(null_chk, "%X", id->avg);
    }
    uba_add(U, (void *) id);
    count++;
  }
  free(linebuf);
  free(null_chk);
  if (fclose(f) != 0) {
    printf("Failed to close \"%s\" stream, aborting.\n", fname);
    exit(EXIT_FAILURE);
  }
  free(fname);

  img_data **img = (img_data **) uba_free(U);
  img = xrealloc(img, count * sizeof(img_data *));
  (*loaded) = count;
  return img;
}

img_data **parse_folder(const char *dirname,
			unsigned int *loaded,
                        unsigned int min_w,
			unsigned int min_h) {
  REQUIRES(dirname != NULL);

  img_data **imgd = parse_folder_data(dirname, loaded);
  if (imgd != NULL) {
    return imgd;
  }

  DIR *d = opendir(dirname);
  if (d == NULL) {
    fprintf(stderr, "Unable to open directory %s\n", dirname);
    exit(EXIT_FAILURE);
  }
  
  uba *U = uba_new(4); // 4 is arbitrary
  struct dirent *current_file = readdir(d);
  current_file = readdir(d);
  current_file = readdir(d); // skip . and ..

  img *I;
  unsigned int dirname_len = strlen(dirname);
  char *filename;
  unsigned int count;

  for (count = 0; current_file != NULL; count++) {
    filename = xcalloc(dirname_len + strlen(current_file->d_name) + 2, 
		       sizeof(char));
    filename = strncpy(filename, dirname, dirname_len);
    filename[dirname_len] = '/';
    filename[dirname_len + 1] = '\0';
    filename = strcat(filename, current_file->d_name);
    //printf("Loading %s\n", filename);
    I = image_load(filename, min_w, min_h);

    img_data *id = xmalloc(sizeof(img_data));
    id->filename = filename;

    if (is_img(I)) {
      pixel avg = get_average(I);
      image_free(I);
      id->avg = xmalloc(sizeof(pixel));
      *(id->avg) = avg;
      //printf("UBA size: %d, UBA elems: %d\n", uba_limit(U), uba_size(U));
    }
    else id->avg = NULL;
    uba_add(U, (void *) id);

    current_file = readdir(d);
    if (count % 1000 == 0) printf("Loaded %d files...\n", count);
  }

  printf("Load completed! Loaded %d files.\n", count);

  if (closedir(d) == -1) {
    fprintf(stderr, "Unable to close directory stream\n");
    exit(EXIT_FAILURE);
  }

  unsigned int len = uba_size(U);
  ASSERT(len == count);
  imgd = (img_data **) uba_free(U);
  imgd = xrealloc(imgd, sizeof(img_data *) * len);
  (*loaded) = len;
  dump_folder_data(dirname, imgd, len);
  ENSURES(imgd != NULL);
  return imgd;
}

int get_distance(pixel p1, pixel p2) {
  int r_dist = ((int) get_red(p1)) - ((int) get_red(p2));
  int g_dist = ((int) get_green(p1)) - ((int) get_green(p2));
  int b_dist = ((int) get_blue(p1)) - ((int) get_blue(p2));
  return (r_dist * r_dist + g_dist * g_dist + b_dist * b_dist);
}

// TODO: allow limited reusing based on a tolerance!
img *make_collage(img *source,
		  img_data *I[],
		  unsigned int I_len,
		  unsigned int std) {  
  REQUIRES(is_img(source) && I_len >= source->w * source->h);
  // resize source image as necessary
  // (pretend it's been standardized before coming here)
  img *source_std = source;

  img **sources_img = xmalloc(source_std->h * source_std->w * sizeof(img *));

  int tolerance = 0;

  // brute force closest RGB values for each pixel
  for (unsigned int src_pix = 0;
       src_pix < source_std->w * source_std->h;
       src_pix++) {
    pixel p = source_std->data[src_pix];
    img_data *best_img = NULL;
    unsigned int best_img_index;
    int best_img_distance;
    for (unsigned int i = 0; i < I_len; i++) {
      img_data *s = I[i];
      if (s != NULL && s->avg != NULL) {
	int distance = get_distance(p, *(s->avg));
	if (best_img == NULL || distance < best_img_distance) {
	  best_img = s;
	  best_img_index = i;
	  best_img_distance = distance;
	  if (abs(distance) <= tolerance) {
            //printf("Found match before end of array.\n");
            break;
          }
	}
      }
    }
    img *best_i = image_load(best_img->filename, std, std);
    if (best_i == NULL) src_pix--; // repeat pixel if load fails
    else {
      img *best_i_std = standardize(best_i, std);
      sources_img[src_pix] = best_i_std;
      image_free(best_i);
    }
    // Remove these next lines to allow duplicate images in the collage
    if (I[best_img_index]->avg != NULL) free(I[best_img_index]->avg);
    I[best_img_index]->avg = NULL;
  }
  
  // stitch back together the sorted version
  img *stitched = stitch(sources_img, source_std->w, source_std->h);
  for (unsigned int i = 0; i < source_std->w * source_std->h; i++)
    image_free(sources_img[i]);
  free(sources_img);
  ASSERT(is_img(stitched));
  return stitched;
}

img *make_collage_kdtree(img *source,
                         img_data *I[],
                         unsigned int I_len,
                         unsigned int std) {  
  REQUIRES(is_img(source) && I_len >= source->w * source->h);
  // resize source image as necessary
  // (pretend it's been standardized before coming here)
  img *source_std = source;

  img **sources_img = xmalloc(source_std->h * source_std->w * sizeof(img *));

  struct kdtree *K = kd_create(3);
  // populate the tree
  for (unsigned int i = 0; i < I_len; i++) {
    img_data *imgd = I[i];
    pixel *p = imgd->avg;
    if (p != NULL) {
      kd_insert3f(K,
                  (float) get_red(*p),
                  (float) get_green(*p),
                  (float) get_blue(*p),
                  (void *) imgd);
    } 
  }
  //printf("Tree populated!\n");
  for (unsigned int src_pix = 0;
       src_pix < source_std->w * source_std->h;
       src_pix++) {
    pixel p = source_std->data[src_pix];
    struct kdres *best_kdres = kd_nearest3f(K,
                                            (float) get_red(p),
                                            (float) get_green(p),
                                            (float) get_blue(p));
    //printf("Got result for pixel %d\n", src_pix);
    img_data *best = (img_data *) kd_res_item_data(best_kdres);
    assert(best != NULL);
    img *best_i = image_load(best->filename, std, std);
    img *best_i_std = standardize(best_i, std);
    sources_img[src_pix] = best_i_std;
    //printf("Freeing...");
    kd_res_free(best_kdres);
    image_free(best_i);
  }
  //printf("All lookups completed.\n");
  kd_free(K);

  // stitch back together the sorted version
  img *stitched = stitch(sources_img, source_std->w, source_std->h);
  for (unsigned int i = 0; i < source_std->w * source_std->h; i++)
    image_free(sources_img[i]);
  free(sources_img);
  ASSERT(is_img(stitched));
  return stitched;
}
