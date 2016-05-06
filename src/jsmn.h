#include <stdint.h> //uint8
#include <stddef.h>
#include "mex.h"

#define TYPE_DATA_END 0
#define TYPE_OBJECT 1
#define TYPE_ARRAY  2
#define TYPE_KEY    3
#define TYPE_STRING 4
#define TYPE_NUMBER 5
#define TYPE_NULL   6
#define TYPE_TRUE   7
#define TYPE_FALSE  8

#define N_DATA_OBJECT   4
#define N_DATA_ARRAY    4
#define N_DATA_KEY      5
#define N_DATA_STRING   3
#define N_DATA_NUMERIC  3
#define N_DATA_LOGICAL  1
#define N_DATA_NULL     3

#define JSMN_ERROR_NOMEM -2


void jsmn_parse(unsigned char *js, size_t len, mxArray *plhs[]);