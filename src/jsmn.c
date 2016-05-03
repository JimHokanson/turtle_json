#include "jsmn.h"
#include "stdio.h"
#include <stdlib.h>
#include "mex.h"
#include <math.h>
#include "stdint.h"  //uint_8

#define SKIP_WHITESPACE while (is_whitespace[js[++parser_position]]){}
#define PROCESS_END_OF_ARRAY_VALUE \
	SKIP_WHITESPACE; \
	switch (js[parser_position]) { \
        case ',': \
            SKIP_WHITESPACE; \    
            goto *array_jump[js[parser_position]]; \
        case ']': \
            goto S_CLOSE_ARRAY; \
        default: \
            goto S_ERROR_END_OF_VALUE_IN_ARRAY; \
	} \


#define ERROR_DEPTH mexErrMsgIdAndTxt("jsmn_mex:depth_limit","Max depth exceeded");

const bool is_start_of_number[256] = { false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true,false,false,false,true,true,true,true,true,true,true,true,true,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };
const bool is_whitespace[256] = { false,false,false,false,false,false,false,false,false,true,true,false,false,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };
//const bool is_number_array[256] = { false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true,true,true,true,true,true,true,true,true,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };

//49:58
const bool is_number_array[256] = {[0 ... 47]=false,[48 ... 57]=true,[58 ... 255]=false};

//-------------------------------------------------------------------------
//--------------------  End of Number Parsing  ----------------------------
//-------------------------------------------------------------------------
//TODO: Allow looking for closer - verify math later
double string_to_double_no_math(char *p, char **char_offset) {

	while (is_number_array[*(++p)]) {}

	if (*p == '.') {
		++p;
		while (is_number_array[*(++p)]) {}
	}

	if (*p == 'E' || *p == 'e') {
		++p;
		switch (*p) {
		case '-':
			++p;
			break;
		case '+':
			++p;
		}

		while (is_number_array[*(++p)]) {}
	}

	*char_offset = p;

	return 0;
}


void seek_string_end(const char *js, int *input_parser_position) {

	//  seek_string_end(js,&parser_position)

	//TODO: This could be completely wrong if we have unicode
	//although I think unicode has to be escaped????
	int parser_position = *input_parser_position;
	char c;

	while ((c = js[++parser_position])) {
		if (c == '\"') {
			//The idea here is that we don't check this all the time
			//If we hit \" we would need to keep going back until
			//we determine if the '\' is real or not
			if (js[parser_position - 1] == '\\') {
				mexErrMsgIdAndTxt("jsmn_mex:unhandled_case", "Code not yet written");
			}
			*input_parser_position = parser_position;
			return;
		}
	}

	mexErrMsgIdAndTxt("jsmn_mex:unterminated_string", "Unable to find a terminating string quote");
}




//Parser initialization
//----------------------------------
void jsmn_init(jsmn_parser *parser) {
	parser->position = -1;
	parser->current_token = -1;
	parser->super_token = -1;
	parser->last_function_type = -1;
	parser->n_numbers = 0;
}

//We work with the parser variables directly
//Here we
void refill_parser(jsmn_parser *parser,
	int parser_position,
	int current_token_index,
	int super_token_index,
	int last_function_type) {


	//  refill_parser(parser,parser_position,next_token_index,super_token_index,false)

	parser->position = parser_position;
	parser->current_token = current_token_index;
	parser->super_token = super_token_index;
	parser->last_function_type = last_function_type;
}

//=========================================================================
//              Parse JSON   -    Parse JSON    -    Parse JSON
//=========================================================================
int jsmn_parse(const char *js, size_t string_byte_length) {

	/*
	*  Inputs
	*  ------
	*  parser :
	*    Initialized parser from jsmn_init()
	*  js :
	*    The JSON string to parse
	*/

	const void *array_jump[256] = { &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_PARSE_STRING_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_OPEN_ARRAY_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_PARSE_FALSE_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_PARSE_NULL_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_PARSE_TRUE_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_OPEN_OBJECT_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY };


	int objects_or_arrays_i = -1;
	int keys_i = -1;
	int strings_i = -1;
	int numeric_i = -1;

	int objects_or_arrays_max_i = 1000;
	int keys_max_i = 1000;
	int strings_max_i = 1000;
	int numeric_max_i = 1000;

	//enum STATES next_state;
	const int MAX_DEPTH = 200;

	//TODO: This needs to be fixed
	int parent_types[201];
	int parent_indices[201];
	int parent_sizes[201];
	int current_depth = 0;

	int parser_position = -1;
	char *pEndNumber;
	int current_token_index = -1;


	int n_tokens_to_allocate = ceil(string_byte_length / 2);
	uint8_t *main_types = mxMalloc(n_tokens_to_allocate);
	int *main_indices = mxMalloc(n_tokens_to_allocate * sizeof(int));

	SKIP_WHITESPACE;

	switch (js[parser_position]) {
        case '{':
            goto S_OPEN_OBJECT_IN_KEY;
        case '[':
            goto S_OPEN_ARRAY_IN_KEY;
        default:
            mexErrMsgIdAndTxt("jsmn_mex:invalid_start", "Starting token needs to be an opening object or array");
	}

//=============================================================
S_OPEN_OBJECT_IN_ARRAY:
	parent_sizes[current_depth] += 1;

	//Fall Through --------------------
    
S_OPEN_OBJECT_IN_KEY:

	++current_token_index;
	main_types[current_token_index] = TYPE_OBJECT;
	main_indices[current_token_index] = ++objects_or_arrays_i;

	if (objects_or_arrays_i > objects_or_arrays_max_i) {
		//TODO: Resize
	}

	if (current_depth == MAX_DEPTH) {
		ERROR_DEPTH
	}
	else {
		++current_depth;
		parent_types[current_depth] = TYPE_OBJECT;
		parent_indices[current_depth] = objects_or_arrays_i;
		parent_sizes[current_depth] = 0;
	}

	SKIP_WHITESPACE

    switch (js[parser_position]) {
        case '"':
            goto S_PARSE_KEY;
        case '}':
            goto S_CLOSE_EMPTY_OBJECT;
        default:
            goto S_ERROR_OPEN_OBJECT;
    }

//=============================================================
S_CLOSE_KEY_AND_OBJECT:
	//TODO: set token close
	--current_depth;

	//Fall Through ------  closing the object
S_CLOSE_EMPTY_OBJECT:

	//TODO: Insert logic for object
	//token close
	//set count

	if (current_depth == 1) {
		goto S_PARSE_END_OF_FILE;
	}
	else {
		--current_depth;
		if (parent_types[current_depth] == TYPE_KEY) {
			goto S_PARSE_END_OF_VALUE_IN_KEY;
		}
		else {
			goto S_PARSE_END_OF_VALUE_IN_ARRAY;
		}
	}



	//=============================================================
S_OPEN_ARRAY_IN_ARRAY:
	parent_sizes[current_depth] += 1;

	//Fall Through -------------------------------
S_OPEN_ARRAY_IN_KEY:
	++current_token_index;

	main_types[current_token_index] = TYPE_ARRAY;
	main_indices[current_token_index] = ++objects_or_arrays_i;

	if (objects_or_arrays_i > objects_or_arrays_max_i) {
		//TODO: Resize
	}

	if (current_depth == MAX_DEPTH) {
		ERROR_DEPTH
	}
	else {
		++current_depth;
		parent_types[current_depth] = TYPE_ARRAY;
		parent_indices[current_depth] = objects_or_arrays_i;
		parent_sizes[current_depth] = 0;
	}

	//TODO: This could be a call to S_PARSE_END_OF_VALUE_IN_ARRAY
	SKIP_WHITESPACE
    goto *array_jump[js[parser_position]];



	//=============================================================
S_CLOSE_ARRAY:

	//TODO: Insert logic for object
	//token close
	//set count

	if (current_depth == 1) {
		goto S_PARSE_END_OF_FILE;
	}
	else {
		--current_depth;
		if (parent_types[current_depth] == TYPE_KEY) {
			goto S_PARSE_END_OF_VALUE_IN_KEY;
		}
		else {
			goto S_PARSE_END_OF_VALUE_IN_ARRAY;
		}
	}
	//                 mexPrintf("WTF\n");
	//                 mexPrintf("Type: %d\n",main_types[current_depth]);
	//                 mexPrintf("State: %d\n",next_state);

	//=============================================================
S_PARSE_KEY:
	parent_sizes[current_depth] += 1;
	++current_token_index;

	main_types[current_token_index] = TYPE_KEY;
	main_indices[current_token_index] = ++keys_i;


	//TODO: Change below ....
	if (keys_i > keys_max_i) {
		//TODO: Resize
	}

	if (current_depth == MAX_DEPTH) {
		ERROR_DEPTH
	}
	else {
		++current_depth;
		parent_types[current_depth] = TYPE_KEY;
		parent_indices[current_depth] = keys_i;
	}


	//TODO: Set start

	seek_string_end(js, &parser_position);

	//TODO: Set end


	SKIP_WHITESPACE;

	if (js[parser_position] == ':') {

		//Advance to the next token
		SKIP_WHITESPACE

			//TODO: Include - value as well
			if (is_number_array[js[parser_position]]) {
				goto S_PARSE_NUMBER_IN_KEY;
			}
			else {
				switch (js[parser_position]) {
				case '"':
					goto S_PARSE_STRING_IN_KEY;
					break;
				case '-':
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					goto S_PARSE_NUMBER_IN_KEY;
					break;
				case '{':
					goto S_OPEN_OBJECT_IN_KEY;
					break;
				case '[':
					goto S_OPEN_ARRAY_IN_KEY;
					break;
				case 't':
					goto S_PARSE_TRUE_IN_KEY;
					break;
				case 'f':
					goto S_PARSE_FALSE_IN_KEY;
					break;
				case 'n':
					goto S_PARSE_NULL_IN_KEY;
					break;
				default:
					goto S_ERROR_OPEN_KEY_2;
				}
			}
	}
	else {
		goto S_ERROR_OPEN_KEY_1;
	}



	//=============================================================
S_PARSE_STRING_IN_ARRAY:
	parent_sizes[current_depth] += 1;
	++current_token_index;

	main_types[current_token_index] = TYPE_STRING;
	main_indices[current_token_index] = ++strings_i;

	if (strings_i > strings_max_i) {
		//TODO: Resize
	}

	//TODO: Set start
	//
	seek_string_end(js, &parser_position);

	//TODO: Set end

	PROCESS_END_OF_ARRAY_VALUE;



	//=============================================================
S_PARSE_STRING_IN_KEY:
	++current_token_index;

	main_types[current_token_index] = TYPE_STRING;
	main_indices[current_token_index] = ++strings_i;

	if (strings_i > strings_max_i) {
		//TODO: Resize
	}

	//TODO: Set start
	//
	seek_string_end(js, &parser_position);

	//TODO: Set end

	goto S_PARSE_END_OF_VALUE_IN_KEY;



	//=============================================================
S_PARSE_NUMBER_IN_KEY:

	++current_token_index;

	main_types[current_token_index] = TYPE_NUMBER;
	main_indices[current_token_index] = ++numeric_i;

	if (numeric_i > numeric_max_i) {
		//TODO: Resize

	}

	//TODO: log start

	string_to_double_no_math(js + parser_position, &pEndNumber);
	parser_position = (int)(pEndNumber - js);
	parser_position--;

	goto S_PARSE_END_OF_VALUE_IN_KEY;



	//=============================================================
S_PARSE_NUMBER_IN_ARRAY:
	parent_sizes[current_depth] += 1;
	++current_token_index;

	main_types[current_token_index] = TYPE_NUMBER;
	main_indices[current_token_index] = ++numeric_i;

	if (numeric_i > numeric_max_i) {
		//TODO: Resize

	}

	//TODO: log start

	string_to_double_no_math(js + parser_position, &pEndNumber);
	parser_position = (int)(pEndNumber - js);

	//TODO: Let's get rid of this by incorporating it below
	parser_position--;

	//TODO: I think we would get some speed gains by correctly guessing
	//the structure
	// -> typically ", " <- comma then space then number

    //Navigation --------------------------------
	PROCESS_END_OF_ARRAY_VALUE;

	//=============================================================
S_PARSE_NULL_IN_KEY:

	++current_token_index;

	main_types[current_token_index] = TYPE_NUMBER;
	main_indices[current_token_index] = ++numeric_i;

	if (numeric_i > numeric_max_i) {
		//TODO: Resize

	}

	//TODO: log start

	parser_position += 3;
	goto S_PARSE_END_OF_VALUE_IN_KEY;



	//=============================================================
S_PARSE_NULL_IN_ARRAY:

	parent_sizes[current_depth] += 1;
	++current_token_index;

	main_types[current_token_index] = TYPE_NUMBER;
	main_indices[current_token_index] = ++numeric_i;

	if (numeric_i > numeric_max_i) {
		//TODO: Resize

	}

	//TODO: log start

	parser_position += 3;
    
	PROCESS_END_OF_ARRAY_VALUE;



	//=============================================================
S_PARSE_TRUE_IN_KEY:
	++current_token_index;

	main_types[current_token_index] = TYPE_LOGICAL;
	main_indices[current_token_index] = 1;

	parser_position += 3;
	goto S_PARSE_END_OF_VALUE_IN_KEY;


S_PARSE_TRUE_IN_ARRAY:
	parent_sizes[current_depth] += 1;
	++current_token_index;

	main_types[current_token_index] = TYPE_LOGICAL;
	main_indices[current_token_index] = 1;

	parser_position += 3;
    
    PROCESS_END_OF_ARRAY_VALUE;
    
	//goto S_PARSE_END_OF_VALUE_IN_ARRAY;

S_PARSE_FALSE_IN_KEY:
	++current_token_index;

	main_types[current_token_index] = TYPE_LOGICAL;
	main_indices[current_token_index] = 0;

	parser_position += 4;
    
    goto S_PARSE_END_OF_VALUE_IN_KEY;

S_PARSE_FALSE_IN_ARRAY:
	parent_sizes[current_depth] += 1;
	++current_token_index;

	main_types[current_token_index] = TYPE_LOGICAL;
	main_indices[current_token_index] = 0;

	parser_position += 4;
    
	PROCESS_END_OF_ARRAY_VALUE;


S_PARSE_END_OF_VALUE_IN_ARRAY:
	SKIP_WHITESPACE;

	switch (js[parser_position]) {
	case ',':
		goto S_PARSE_COMMA_IN_ARRAY;
	case ']':
		goto S_CLOSE_ARRAY;
	default:
        goto S_ERROR_END_OF_VALUE_IN_ARRAY;
	}


	//=============================================================
S_PARSE_END_OF_VALUE_IN_KEY:

	SKIP_WHITESPACE;

	switch (js[parser_position]) {
        case ',':
            goto S_PARSE_COMMA_IN_OBJECT;
        case '}':
            goto S_CLOSE_KEY_AND_OBJECT;
        default:
            goto S_ERROR_END_OF_VALUE_IN_KEY;
	}


	//=============================================================
S_PARSE_END_OF_FILE:
	SKIP_WHITESPACE

		if (!(js[parser_position] == '\0')) {
			mexErrMsgIdAndTxt("jsmn_mex:invalid_end", "non-whitespace characters found after end of root token close");
		}

	goto finish_main;



	//=============================================================
S_PARSE_COMMA_IN_ARRAY:
	SKIP_WHITESPACE
	goto *array_jump[js[parser_position]];




	//=============================================================
S_PARSE_COMMA_IN_OBJECT:

	SKIP_WHITESPACE
		--current_depth;

	if (js[parser_position] == '"') {
		goto S_PARSE_KEY;
	}
	else {
		//TODO: Change this ...
		mexErrMsgIdAndTxt("jsmn_mex:no_key", "Key expected");
	}



	//=============================================================
S_ERROR_OPEN_OBJECT:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "S_ERROR_OPEN_OBJECT");

S_ERROR_OPEN_KEY_1:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "S_ERROR_OPEN_KEY_1");

S_ERROR_OPEN_KEY_2:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "S_ERROR_OPEN_KEY_2");

//TODO: Describe when this error is called    
S_ERROR_END_OF_VALUE_IN_KEY:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "Token of key must be followed by a comma or a closing object ""}"" character");

    
S_ERROR_UNIMPLEMENTED:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "S_ERROR_UNIMPLEMENTED");

//TODO: Open array points here now too
S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY");
	//mexErrMsgIdAndTxt("jsmn_mex:no_primitive","Primitive value was not found after the comma");

S_ERROR_END_OF_VALUE_IN_ARRAY:
		//                         mexPrintf("Current depth: %d\n",current_depth);
		//                         mexPrintf("Current type: %d\n",parent_types[current_depth-4]);
		//                         mexPrintf("Current type: %d\n",parent_types[current_depth-3]);
		//                         mexPrintf("Current type: %d\n",parent_types[current_depth-2]);
		//                         mexPrintf("Current type: %d\n",parent_types[current_depth-1]);
		//                         mexPrintf("Current type: %d\n",parent_types[current_depth]);
		//                         mexPrintf("Current char: %c\n",js[parser_position]);    
	mexPrintf("Current position: %d\n", parser_position);
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "Token in array must be followed by a comma or a closing array ""]"" character ");    

finish_main:
    
    
    
    
    
    
    
	return current_token_index + 1;

	//         int num_tokens,
	//         double *values,
	//         uint8_t *types,
	//         int *sizes,
	//         int *parents,
	//         int *tokens_after_close,
	//         mxArray **mxStrings





	// // //     const double MX_NAN = mxGetNaN();
	// // //
	// // //     //Parser local variables
	// // //     //--------------------------------------
	// // //     int parser_position = parser->position;
	// // //     int current_token_index = parser->current_token;
	// // //     int super_token_index = parser->super_token;
	// // //
	// // //     int num_tokens_minus_1 = num_tokens-1;
	// // //
	// // //     //Frequently accessed super token attributes
	// // //     //------------------------------------------
	// // //     //This is true when inside an attribute
	// // //     bool super_token_is_key;
	// // //     //This was moved to a variable specifically for large arrays
	// // //     int super_token_size;
	// // //
	// // //     char *pEndNumber;
	// // //
	// // // //     //reinitialize super if we've reallocated memory for the parser
	// // // //     if (super_token_index != -1){
	// // // //         super_token_is_key = types[super_token_index] == JSMN_KEY;
	// // // //     }else{
	// // // //         super_token_is_key = false;
	// // // //     }





}

void setStructField(mxArray *s, void *pr, const char *fieldname, mxClassID classid, mwSize N)
{
    
    //setStructField(plhs[0],data_ptr,"name",mxINT32_CLASS,N)
    
    //mwSize ndim, const mwSize *dims, mxClassID classid, mxComplexity ComplexFlag
    //void mxSetM(mxArray *pm, mwSize m);
    //void mxSetData(mxArray *pm, void *pr);
    //extern int mxAddField(mxArray *pm, const char *fieldname);
    
    mxArray *pm;
    
    pm = mxCreateNumericArray(0, 0, classid, mxREAL);
    mxSetData(pm, pr);
    mxSetM(pm, 1);
    mxSetN(pm, N);
    mxAddField(s,fieldname);
    mxSetField(s,0,fieldname,pm);

}

