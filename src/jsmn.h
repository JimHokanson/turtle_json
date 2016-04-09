#include <stdint.h> //uint8
#include <stddef.h>

#define JSMN_UNDEFINED 0
#define JSMN_OBJECT 1
#define JSMN_ARRAY 2
#define JSMN_STRING 3
#define JSMN_NUMBER 4
#define JSMN_LOGICAL 5
#define JSMN_KEY 6

#define JSMN_ERROR_NOMEM -2

typedef struct {
	int position;       /* offset in the JSON string */
	int current_token;  /* index of current token */
	int super_token;    /* index of super token: object, array, or attribute */
    int last_function_type;
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

int jsmn_parse(jsmn_parser *parser, const char *js, size_t len, 
        unsigned int num_tokens, double *values , uint8_t *types, 
        int *starts, int *ends, int *sizes, int *parents, int *tokens_after_close);