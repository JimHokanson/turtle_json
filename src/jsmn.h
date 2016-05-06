#include <stdint.h> //uint8
#include <stddef.h>
#include "mex.h"

#define TYPE_OBJECT 1
#define TYPE_ARRAY 2
#define TYPE_STRING 3
#define TYPE_NUMBER 4
#define TYPE_LOGICAL 5
#define TYPE_KEY 6
#define TYPE_NULL 7

#define N_DATA_OBJECT 2
#define N_DATA_ARRAY 2
#define N_DATA_KEY 3
#define N_DATA_STRING 2
#define N_DATA_NUMERIC 2
#define N_DATA_LOGICAL 1
#define N_DATA_NULL 1

#define JSMN_ERROR_NOMEM -2


int jsmn_parse(unsigned char *js, size_t len, mxArray *plhs[]);