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

typedef struct {
	int position;       /* offset in the JSON string */
	int current_token;  /* index of current token */
	int super_token;    /* index of super token: object, array, or attribute */
    int last_function_type;
    int n_numbers;
    //0 - in array
    //1 - in key
    //2 - is key
} jsmn_parser;

void jsmn_init(jsmn_parser *parser);

void refill_parser(jsmn_parser *parser,    
        int parser_position,
        int current_token_index,
        int super_token_index,
        int last_function_type);

int jsmn_parse(const char *js, size_t len);