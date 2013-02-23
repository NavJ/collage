#include "image_util.h"

// TODO: make a better unified command line interface for this
int main (int argc, char *argv[]) {
  if (argc <= 2) exit(EXIT_FAILURE);

  char *src_file = argv[1];
  char *src_foldr = argv[2];

  printf("Loading and parsing image %s... ", src_file);
  img *I = image_load(src_file, 0, 0);
  if (I == NULL) exit(EXIT_FAILURE);
  img *I_crp = crop_to_square(I);
  image_free(I);
  img *I_std = shrink(I_crp,
		      50 > I_crp->w ? I_crp->w : 50,
		      50 > I_crp->h ? I_crp->h : 50);
  image_free(I_crp);
  printf("completed.\n");

  unsigned int load = 0;
  printf("Parsing file \"%s/img_data.dat\"...\n", src_foldr);
  img_data **img_foldr = parse_folder(src_foldr, &load, 30, 30);
  printf("%d images loaded successfully!\n", load);
  assert(load != 0);

  printf("Fitting images from %s to %s...\n",
         src_foldr, src_file);  
  img *collaged = make_collage(I_std, img_foldr, load, 30);

  unsigned int src_name_len = strlen(src_file);
  char *save_file = xmalloc(sizeof(char) * (5 + src_name_len));
  save_file = strncpy(save_file, src_file, src_name_len - 4);
  save_file[src_name_len - 4] = '\0';
  save_file = strcat(save_file, "_col.png");

  image_save(collaged, save_file);
  printf("Completed! Image saved to \"%s\".\n", save_file);
  free(save_file);
  image_free(I_std);
  image_free(collaged);
  for (unsigned int i = 0; i < load; i++) {
    if (img_foldr[i] != NULL) {
      if (img_foldr[i]->avg != NULL) {
        free(img_foldr[i]->avg);
      }
      if (img_foldr[i]->filename != NULL) {
        free(img_foldr[i]->filename);
      }
      free(img_foldr[i]);
    }
  }
  free(img_foldr);
  return 0;
}
