#include <stdint.h> //uint8
#include <stddef.h>
#include "mex.h"

#define TYPE_OBJECT 1
#define TYPE_ARRAY 2
#define TYPE_STRING 3
#define TYPE_NUMBER 4
#define TYPE_LOGICAL 5
#define TYPE_KEY 6

#define JSMN_ERROR_NOMEM -2


int jsmn_parse(unsigned char *js, size_t len);