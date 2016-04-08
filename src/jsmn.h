#ifndef __JSMN_H_
#define __JSMN_H_

//NYI
//#define JSMN_ONE_BASED_INDICES 1

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */
//typedef enum {
#define JSMN_UNDEFINED 0
#define JSMN_OBJECT 1
#define JSMN_ARRAY 2
#define JSMN_STRING 3
#define JSMN_NUMBER 4
#define JSMN_LOGICAL 5
//} jsmntype_t;

enum jsmnerr {
	/* Not enough tokens were provided */
	JSMN_ERROR_NOMEM = -1,
	/* Invalid character inside a JSON string */
	JSMN_ERROR_INVAL = -2,
	/* The string is not a full JSON packet, more bytes expected */
	JSMN_ERROR_PART = -3,
    JSMN_ERROR_INVALID_TOKEN_CHAR = -4
};

/**
 * JSON token description.
 * @param		type	type (object, array, string etc.)
 * @param		start	start position in JSON data string
 * @param		end		end position in JSON data string
 */
// typedef struct {
// 	//jsmntype_t type;
// 	//int start;
// 	//int end;
// 	//int size;
// 	//int parent;
//     //int token_after_close; //
// } jsmntok_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct {
	unsigned int pos; /* offset in the JSON string */
	int toknext; /* next token to allocate */
	int toksuper; /* superior token node, object, array, or attribute */
    int is_key;
} jsmn_parser;

/**
 * Create JSON parser over an array of tokens
 */
void jsmn_init(jsmn_parser *parser);

void refill_parser(jsmn_parser *parser,    
        unsigned int parser_position,
        int next_token_index,
        int super_token_index,
        int is_key);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens, each describing
 * a single JSON object.
 */
int jsmn_parse(jsmn_parser *parser, const char *js, size_t len, 
        unsigned int num_tokens, double *values , uint8_t *types, 
        int *starts, int *ends, int *sizes, int *parents, int *tokens_after_close);

#ifdef __cplusplus
}
#endif

#endif /* __JSMN_H_ */
