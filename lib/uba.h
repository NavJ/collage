#ifndef _UBA__
#define _UBA__

#include <stdbool.h>
#include <stdlib.h>

#include "xalloc.h"
#include "contracts.h"

typedef struct uba uba;

uba *uba_new(unsigned int initial_limit);
unsigned int uba_size(uba *U);
unsigned int uba_limit(uba *U);
void uba_add(uba *U, void *e);
void **uba_data(uba *U);
void **uba_free(uba *U);

#endif

