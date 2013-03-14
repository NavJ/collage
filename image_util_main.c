#include <getopt.h>

#include "image_util.h"

void print_usage(char *argv[]) {
  // Not sure whether to leave it like this, or to do the following:
  // ./img image folder [-k -v -o output] OR ./img [-v] -p folder
  fprintf(stderr,
          "Usage: %s [-k] [-v] -f folder -i image [-o output]\n", argv[0]);
  fprintf(stderr,
          "   OR: %s -p [-v] -f folder\n\n", argv[0]);
  fprintf(stderr, "Summary of commands:\n");
  fprintf(stderr,
          "    -p: parses folder, then exits\n");
  fprintf(stderr,
          "    -k: uses kdtree algorithm to generate output image.\n");
  fprintf(stderr,
          "    -h: prints this help message.\n");
  fprintf(stderr,
          "    -v: toggle verbosity.\n");
  return;
}

// Add this to lib perhaps
void free_folder(img_data **img_folder, unsigned int load) {
  for (unsigned int i = 0; i < load; i++) {
    if (img_folder[i] != NULL) {
      if (img_folder[i]->avg != NULL) {
        free(img_folder[i]->avg);
      }
      if (img_folder[i]->filename != NULL) {
        free(img_folder[i]->filename);
      }
      free(img_folder[i]);
    }
  }
  free(img_folder);
  return;
}

int main(int argc, char *argv[]) {
  int opt;
  bool kd = false, just_parse = false, verbose = false;
  char *src_file = NULL, *src_folder = NULL, *save_file = NULL;

  // Parse flags and IO
  while ((opt = getopt(argc, argv, "f:hi:ko:pv")) != -1) {
    switch (opt) {
    case 'f':
      src_folder = optarg;
      break;
    case 'h':
      print_usage(argv);
      return 0;
    case 'i':
      src_file = optarg;
      break;
    case 'k':
      kd = true;
      break;
    case 'o':
      save_file = optarg;
      break;
    case 'p':
      just_parse = true;
      break;
    case 'v':
      verbose = true;
      break;
    default:
      fprintf(stderr, "Unknown argument: %c\n", optopt);
      print_usage(argv);
      exit(EXIT_FAILURE);
    }
  }

  if (src_folder == NULL) {
    fprintf(stderr, "Missing required argument -f\n");
    print_usage(argv);
    exit(EXIT_FAILURE);
  }

  unsigned int load = 0, min_dim = 30;
  if (verbose) printf("Parsing folder \"%s\"...\n", src_folder);
  img_data **img_folder = parse_folder(src_folder, &load, min_dim, min_dim);
  if (verbose) printf("%d images loaded successfully!\n", load);
  if (just_parse) {
    if (verbose) printf("Parsing completed. Exiting...\n");
    free_folder(img_folder, load);
    return 0;
  }
  assert(load != 0);

  if (src_file == NULL) {
    fprintf(stderr, "Missing required argument -i\n");
    print_usage(argv);
    exit(EXIT_FAILURE);
  }

  if (verbose) printf("Loading and parsing image %s... ", src_file);
  img *I = image_load(src_file, 0, 0);
  if (I == NULL) {
    fprintf(stderr, "Failed to load image %s, aborting.\n", src_file);
    exit(EXIT_FAILURE);
  }
  img *I_crp = crop_to_square(I);
  image_free(I);
  unsigned int shrink_size = 50;
  img *I_std = shrink(I_crp,
                      shrink_size > I_crp->w ? I_crp->w : shrink_size,
                      shrink_size > I_crp->h ? I_crp->h : shrink_size);
  image_free(I_crp);
  if (verbose) printf("completed.\n");

  if (verbose) printf("Fitting images from %s to %s using %s algorithm...\n",
                      src_folder, src_file, kd ? "kdtree" : "linear");
  img *collaged =
    kd ?
    make_collage_kdtree(I_std, img_folder, load, 30) :
    make_collage(I_std, img_folder, load, 30);
  
  bool free_save_file = false;
  if (save_file == NULL) {
    unsigned int src_name_len = strlen(src_file);
    save_file = xmalloc(sizeof(char) * (5 + src_name_len));
    save_file = strncpy(save_file, src_file, src_name_len - 4);
    save_file[src_name_len - 4] = '\0';
    save_file = strcat(save_file, "_col.png");
    free_save_file = true;
  }

  image_save(collaged, save_file);
  if (verbose) printf("Completed! Image saved to \"%s\".\n", save_file);
  if (free_save_file) free(save_file);
  image_free(I_std);
  image_free(collaged);
  free_folder(img_folder, load);
  return 0;
}
