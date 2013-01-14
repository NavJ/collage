#include "uba.h"

struct uba {
  unsigned int elems;
  unsigned int length;
  void **data;
};

bool is_uba(uba *U) {
  if (U == NULL) return false;
  else if (U->data == NULL) return false;
  else if (U->elems > U->length) return false;
  return true;
}

uba *uba_new(unsigned int initial_limit) {
  uba *U = xmalloc(sizeof(uba));
  U->elems = 0;
  U->length = initial_limit;
  U->data = xcalloc(U->length, sizeof(void *));

  ENSURES(is_uba(U));
  return U;
}

unsigned int uba_size(uba *U) {
  REQUIRES(is_uba(U));
  return U->elems;
}

unsigned int uba_limit(uba *U) {
  REQUIRES(is_uba(U));
  return U->length;
}

void uba_add(uba *U, void *e) {
  REQUIRES(is_uba(U));

  if (U->elems + 1 == U->length) {
    U->length *= 2;
    U->data = xrealloc(U->data, U->length * sizeof(void *));
  }

  U->data[U->elems] = e;
  U->elems++;

  ENSURES(is_uba(U));
  return;
}

void **uba_data(uba *U) {
  REQUIRES(is_uba(U));
  return U->data;
}

void **uba_free(uba *U) {
  REQUIRES(is_uba(U));

  void **data = uba_data(U);
  free(U);

  ENSURES(data != NULL);
  return data;
}
