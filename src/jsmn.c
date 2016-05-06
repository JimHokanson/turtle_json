#include "jsmn.h"
#include "stdio.h"
#include <stdlib.h>
#include <ctype.h>
#include "mex.h"
#include <math.h>
#include "stdint.h"  //uint_8

//TODO: store goto rather than type in parent

//TODO: replace with a goto            
#define ERROR_DEPTH mexErrMsgIdAndTxt("jsmn_mex:depth_limit","Max depth exceeded");

#define N_INT_PART  data_info[0]
#define N_FRAC_PART data_info[1]
#define N_EXP_PART  data_info[2]

#define EXPAND_DATA_CHECK(x) \
	if (current_type_index+x >= data_size_index_max){ \
        data_size_allocated = ceil(1.5*data_size_allocated); \
        types = mxRealloc(types,type_size_allocated*sizeof(int)); \
        data_size_index_max = data_size_allocated-1; \
    } \

#define SET_TYPE(x) \
	if (current_type_index >= type_size_index_max){ \
        type_size_allocated = ceil(1.5*type_size_allocated); \
        types = mxRealloc(types,type_size_allocated); \
        type_size_index_max = type_size_allocated-1; \
    } \
	types[++current_type_index] = x; \
      
#define SET_TAC data[parent_indices[current_depth]] = current_data_index+1;
#define SET_N_VALUES data[parent_indices[current_depth]+1] = parent_sizes[current_depth];             
            
#define SETUP_PARENT_INFO(x) \
	if (current_depth == MAX_DEPTH) { \
		ERROR_DEPTH \
	} \
	else { \
		++current_depth; \
		parent_types[current_depth] = x; \
		parent_indices[current_depth] = (++current_data_index); \
		parent_sizes[current_depth] = 0; \
	} \

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
            
            
#define PROCESS_END_OF_KEY_VALUE \
	SKIP_WHITESPACE; \
	switch (js[parser_position]) { \
        case ',': \
            SKIP_WHITESPACE; \
            --current_depth; \
            if (js[parser_position] == '"') { \
                goto S_PARSE_KEY; \
            } \
            else { \
                mexErrMsgIdAndTxt("jsmn_mex:no_key", "Key expected"); \
            } \
        case '}': \
            goto S_CLOSE_KEY_AND_OBJECT; \
        default: \
            goto S_ERROR_END_OF_VALUE_IN_KEY; \
	} \



//const bool is_start_of_number[256] = { false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true,false,false,false,true,true,true,true,true,true,true,true,true,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };
const bool is_whitespace[256] = { false,false,false,false,false,false,false,false,false,true,true,false,false,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };
//const bool is_number_array[256] = { false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true,true,true,true,true,true,true,true,true,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };

//49:58
//const bool is_number_array[256] = {[0 ... 47]=false,[48 ... 57]=true,[58 ... 255]=false};


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

//-------------------------------------------------------------------------
//--------------------  End of Number Parsing  ----------------------------
//-------------------------------------------------------------------------
//TODO: Allow looking for closer - verify math later
inline int string_to_double_no_math(unsigned char *p, unsigned char **char_offset) {

    //We're point at a negative or a digit
    
    
    uint8_t data_info[4] = {0,0,0,0};
    
//     uint8_t d[4];
//     uint8_t *data_info = d;
    
    
    //N_INT_PART
    //N_FRAC_PART
    //N_EXP_PART
    //SIGN_INFO  
    //  1 for negate integer
    //  2 for negate exponent
    
//     uint8_t n_int;
//     uint8_t n_frac;
//     uint8_t n_exp;
//     uint8_t sign_info = 0;
//     unsigned char *start;
    
    if (*p == '-'){
        //SIGN_INFO = 1;
        ++p;
    }
    
    if (isdigit(*p)){
        if(isdigit(*(++p))){
            if(isdigit(*(++p))){
                if(isdigit(*(++p))){
                    if(isdigit(*(++p))){
                        if(isdigit(*(++p))){
                            if(isdigit(*(++p))){
                                if(isdigit(*(++p))){
                                    if(isdigit(*(++p))){
                                        if(isdigit(*(++p))){
                                            if(isdigit(*(++p))){
                                                if(isdigit(*(++p))){
                                                    if(isdigit(*(++p))){
                                                        if(isdigit(*(++p))){
                                                            if(isdigit(*(++p))){
                                                                if(isdigit(*(++p))){
                                                                    if(isdigit(*(++p))){
                                                                        mexErrMsgIdAndTxt("jsmn_mex:too_many_numbers_in_integer_part", "too_many_numbers_in_integer_part");
                                                                    }else{
                                                                        N_INT_PART = 16;
                                                                    }
                                                                }else{
                                                                    N_INT_PART = 15;
                                                                }
                                                            }else{
                                                                N_INT_PART = 14;
                                                            }
                                                        }else{
                                                            N_INT_PART = 13;
                                                        }
                                                    }else{
                                                        N_INT_PART = 12;
                                                    }
                                                }else{
                                                    N_INT_PART = 11;
                                                }
                                            }else{
                                                N_INT_PART = 10;
                                            }
                                        }else{
                                            N_INT_PART = 9;
                                        }
                                    }else{
                                        N_INT_PART = 8;
                                    }
                                }else{
                                    N_INT_PART = 7;
                                }
                            }else{
                                N_INT_PART = 6;
                            }
                        }else{
                            N_INT_PART = 5;
                        }
                    }else{
                        N_INT_PART = 4;
                    }
                }else{
                    N_INT_PART = 3;
                }
            }else{
                N_INT_PART = 2;
            }
        }else{
            N_INT_PART = 1;
        }
    }else{
        mexErrMsgIdAndTxt("jsmn_mex:missing_digit_following_minus_sign", "No digit found after minus sign");
    }
    
    if (*p == '.') {
        if (isdigit(*(++p))){
            if(isdigit(*(++p))){
                if(isdigit(*(++p))){
                    if(isdigit(*(++p))){
                        if(isdigit(*(++p))){
                            if(isdigit(*(++p))){
                                if(isdigit(*(++p))){
                                    if(isdigit(*(++p))){
                                        if(isdigit(*(++p))){
                                            if(isdigit(*(++p))){
                                                if(isdigit(*(++p))){
                                                    if(isdigit(*(++p))){
                                                        if(isdigit(*(++p))){
                                                            if(isdigit(*(++p))){
                                                                if(isdigit(*(++p))){
                                                                    if(isdigit(*(++p))){
                                                                        if(isdigit(*(++p))){
                                                                            if(isdigit(*(++p))){
                                                                                if(isdigit(*(++p))){
                                                                                    if(isdigit(*(++p))){
                                                                                        if(isdigit(*(++p))){
                                                                                            mexErrMsgIdAndTxt("jsmn_mex:too_many_numbers_in_fractional_part", "too_many_numbers_in_fractional_part");
                                                                                        }else{
                                                                                            N_FRAC_PART = 20;
                                                                                        }
                                                                                    }else{
                                                                                        N_FRAC_PART = 19;
                                                                                    }
                                                                                }else{
                                                                                    N_FRAC_PART = 18;
                                                                                }
                                                                            }else{
                                                                                N_FRAC_PART = 17;
                                                                            }
                                                                        }else{
                                                                            N_FRAC_PART = 16;
                                                                        }
                                                                    }else{
                                                                        N_FRAC_PART = 15;
                                                                    }
                                                                }else{
                                                                    N_FRAC_PART = 14;
                                                                }
                                                            }else{
                                                                N_FRAC_PART = 13;
                                                            }
                                                        }else{
                                                            N_FRAC_PART = 12;
                                                        }
                                                    }else{
                                                        N_FRAC_PART = 11;
                                                    }
                                                }else{
                                                    N_FRAC_PART = 10;
                                                }
                                            }else{
                                                N_FRAC_PART = 9;
                                            }
                                        }else{
                                            N_FRAC_PART = 8;
                                        }
                                    }else{
                                        N_FRAC_PART = 7;
                                    }
                                }else{
                                    N_FRAC_PART = 6;
                                }
                            }else{
                                N_FRAC_PART = 5;
                            }
                        }else{
                            N_FRAC_PART = 4;
                        }
                    }else{
                        N_FRAC_PART = 3;
                    }
                }else{
                    N_FRAC_PART = 2;
                }
            }else{
                N_FRAC_PART = 1;
            }
        }else{
            mexErrMsgIdAndTxt("jsmn_mex:missing_digit_following_period", "No digit found after period");
        }
    }
    
// //     else{
// //         ++data_info;
// //     }
    
    if (*p == 'E' || *p == 'e') {
		++p;
		switch (*p) {
		case '-':
            //SIGN_INFO += 2;
			++p;
			break;
		case '+':
			++p;
		}

        if (isdigit(*p)){
            if (isdigit(*(++p))){
                if (isdigit(*(++p))){
                    if (isdigit(*(++p))){
                        if (isdigit(*(++p))){
                        	mexErrMsgIdAndTxt("jsmn_mex:too_many_numbers_in_exponent_part", "too_many_numbers_in_exponent");
                        }else{
                            N_EXP_PART = 4;
                        }
                    }else{
                        N_EXP_PART = 3;
                    }
                }else{
                    N_EXP_PART = 2;
                }
            }else{
                N_EXP_PART = 1;
            }
        }else{
            mexErrMsgIdAndTxt("jsmn_mex:missing_digit_following_exponent", "No digit found after exponent");
        }
	}
    
    *char_offset = p;
    
//     return *(int *)&data_info[0];
    
    return *(int *)data_info;
    
//     while (isdigit(*(++p))) {}
// 
// 	if (*p == '.') {
// 		++p;
//         while (isdigit(*(++p))) {}
// 	}
// 
// 	if (*p == 'E' || *p == 'e') {
// 		++p;
// 		switch (*p) {
// 		case '-':
// 			++p;
// 			break;
// 		case '+':
// 			++p;
// 		}
// 
// 		while (isdigit(*(++p))) {}
// 	}
// // // 
// // // 	
// // // *char_offset = p;
// // // 	return 0;
}


void seek_string_end(unsigned char *js, int *input_parser_position) {

	//  seek_string_end(js,&parser_position)

	//TODO: This could be completely wrong if we have unicode
	//although I think unicode has to be escaped????
	int parser_position = *input_parser_position;
	unsigned char c;

//I had wanted to do just a search for " but then
//I might miss the end of string
//START_STRING_SEEK:
    
// // // 	while ((c = js[++parser_position])) {
// // // 		if (c == '\"') {
// // // 			//The idea here is that we don't check this all the time
// // // 			//If we hit \" we would need to keep going back until
// // // 			//we determine if the '\' is real or not
// // // 			if (js[parser_position - 1] == '\\') {
// // //                 if (js[parser_position - 2] == '\\') {
// // //                     //This indicates that we have '\\"'
// // //                     //Since we don't know what's before we could
// // //                     //have a double escape
// // //                     //The way to fix this to walk backwards and togggle
// // //                     //whether or not the quote '"' is escaped or not
// // //                     //  cur_parser_position = parser_position
// // //                     //  is_escaped = false;
// // //                     //  while(js[--parser_position] == '\\'){
// // //                     //     is_escaped = !is_escaped 
// // //                     //  }
// // //                     //  if (is_escaped) => continue
// // //                     //  else => done
// // //                     mexErrMsgIdAndTxt("jsmn_mex:unhandled_case", "Code not yet written");
// // //                 }
// // //                 //else, keep going looking for another "
// // // 			}else{
// // //                 *input_parser_position = parser_position;
// // //                 return;
// // //             }
// // // 		}
// // // 	}

STRING_SEEK:
    while ((c = js[++parser_position]) && c != '"'){}
    
    if (c == '"') { //otherwise we're at the end of the string ...
        //The idea here is that we don't check this all the time
        //If we hit \" we would need to keep going back until
        //we determine if the '\' is real or not
        if (js[parser_position - 1] == '\\') {
            if (js[parser_position - 2] == '\\') {
                //This indicates that we have '\\"'
                //Since we don't know what's before we could
                //have a double escape
                //The way to fix this to walk backwards and togggle
                //whether or not the quote '"' is escaped or not
                //  cur_parser_position = parser_position
                //  is_escaped = false;
                //  while(js[--parser_position] == '\\'){
                //     is_escaped = !is_escaped 
                //  }
                //  if (is_escaped) => continue
                //  else => done
                mexErrMsgIdAndTxt("jsmn_mex:unhandled_case", "Code not yet written");
            }
            goto STRING_SEEK;
            //else, keep going looking for another "
        }else{
            *input_parser_position = parser_position;
            return;
        }
    }

	mexErrMsgIdAndTxt("jsmn_mex:unterminated_string", "Unable to find a terminating string quote");
}






//=========================================================================
//              Parse JSON   -    Parse JSON    -    Parse JSON
//=========================================================================
int jsmn_parse(unsigned char *js, size_t string_byte_length, mxArray *plhs[]) {

	/*
	*  Inputs
	*  ------
	*  parser :
	*    Initialized parser from jsmn_init()
	*  js :
	*    The JSON string to parse
	*/

    //const bool is_number_array[256] = {[0 ... 47]=false,[48 ... 57]=true,[58 ... 255]=false};
    
    const void *array_jump[256] = {
        [0 ... 33]  = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [34]        = &&S_PARSE_STRING_IN_ARRAY, // "
        [35 ... 44] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [45]        = &&S_PARSE_NUMBER_IN_ARRAY,
        [46 ... 47] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [48 ... 57] = &&S_PARSE_NUMBER_IN_ARRAY,
        [58 ... 90] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [91]        = &&S_OPEN_ARRAY_IN_ARRAY,
        [92 ... 101]  = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [102]         = &&S_PARSE_FALSE_IN_ARRAY,
        [103 ... 109] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [110]         = &&S_PARSE_NULL_IN_ARRAY,    // null
        [111 ... 115] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [116]         = &&S_PARSE_TRUE_IN_ARRAY,    // true
        [117 ... 122] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [123]         = &&S_OPEN_OBJECT_IN_ARRAY,   // {
        [124 ... 255] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY};
        
    const void *key_jump[256] = {
        [0 ... 33]  = &&S_ERROR_TOKEN_AFTER_KEY,
        [34]        = &&S_PARSE_STRING_IN_KEY,      // "
        [35 ... 44] = &&S_ERROR_TOKEN_AFTER_KEY,
        [45]        = &&S_PARSE_NUMBER_IN_KEY,      // -
        [46 ... 47] = &&S_ERROR_TOKEN_AFTER_KEY,    
        [48 ... 57] = &&S_PARSE_NUMBER_IN_KEY,      // 0-9
        [58 ... 90] = &&S_ERROR_TOKEN_AFTER_KEY,
        [91]        = &&S_OPEN_ARRAY_IN_KEY,        // [
        [92 ... 101]  = &&S_ERROR_TOKEN_AFTER_KEY,
        [102]         = &&S_PARSE_FALSE_IN_KEY,   //false
        [103 ... 109] = &&S_ERROR_TOKEN_AFTER_KEY,
        [110]         = &&S_PARSE_NULL_IN_KEY,    // null
        [111 ... 115] = &&S_ERROR_TOKEN_AFTER_KEY,
        [116]         = &&S_PARSE_TRUE_IN_KEY,    // true
        [117 ... 122] = &&S_ERROR_TOKEN_AFTER_KEY,
        [123]         = &&S_OPEN_OBJECT_IN_KEY,   // {
        [124 ... 255] = &&S_ERROR_TOKEN_AFTER_KEY};        
    
    
	//const void *array_jump[256] = { &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_PARSE_STRING_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_PARSE_NUMBER_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_OPEN_ARRAY_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_PARSE_FALSE_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_PARSE_NULL_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_PARSE_TRUE_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_OPEN_OBJECT_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,&&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY };

	//enum STATES next_state;
	const int MAX_DEPTH = 200;

	//TODO: This needs to be fixed
	int parent_types[201];
	int parent_indices[201];
	int parent_sizes[201];
	int current_depth = 0;

	int parser_position = -1;
	unsigned char *pEndNumber;
	int current_type_index = -1;
    int current_data_index = -1;

    //We might change these ...
	int type_size_allocated = ceil(string_byte_length / 6);
    int data_size_allocated = string_byte_length;
    
    int type_size_index_max = type_size_allocated - 1;
    int data_size_index_max = data_size_allocated - 1;
    
	uint8_t *types = mxMalloc(type_size_allocated);
	int *data = mxMalloc(data_size_allocated * sizeof(int));
    
    const double MX_NAN = mxGetNaN();
    
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

    SET_TYPE(TYPE_OBJECT);

    SETUP_PARENT_INFO(TYPE_OBJECT);
    ++current_data_index; //Space for setting size

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
    SET_TAC;
	--current_depth; //Move up to the object
    
	//Fall Through ------  closing the object
S_CLOSE_EMPTY_OBJECT:

    SET_TAC;
    SET_N_VALUES;
    --current_depth; //Move up to parent of the object
    
    if (current_depth == 0) {
		goto S_PARSE_END_OF_FILE;
	}
    
    if (parent_types[current_depth] == TYPE_KEY) {
        PROCESS_END_OF_KEY_VALUE
    }
    
    PROCESS_END_OF_ARRAY_VALUE


	//=============================================================
S_OPEN_ARRAY_IN_ARRAY:
	parent_sizes[current_depth] += 1;

	//Fall Through -------------------------------
S_OPEN_ARRAY_IN_KEY:

    SET_TYPE(TYPE_ARRAY);

    SETUP_PARENT_INFO(TYPE_ARRAY);
    ++current_data_index; //Space for setting size

	SKIP_WHITESPACE
    goto *array_jump[js[parser_position]];



	//=============================================================
S_CLOSE_ARRAY:

    SET_TAC;
    SET_N_VALUES;
    --current_depth;
    
	if (current_depth == 0) {
		goto S_PARSE_END_OF_FILE;
	}
    
    if (parent_types[current_depth] == TYPE_KEY) {
        PROCESS_END_OF_KEY_VALUE
    }
    
    PROCESS_END_OF_ARRAY_VALUE



//=============================================================
S_PARSE_KEY:
	parent_sizes[current_depth] += 1;
    
    SET_TYPE(TYPE_KEY);
    
    SETUP_PARENT_INFO(TYPE_KEY);
    
//     if (current_depth == 5 && current_data_index < 2000){
//         mexPrintf("parent size %d\n",parent_sizes[current_depth-1]);
//         mexPrintf("parent size %d\n",parent_sizes[current_depth]);
//     }
    

    //start
    data[++current_data_index] = parser_position;

	seek_string_end(js, &parser_position);

    //end
	data[++current_data_index] = parser_position;


	SKIP_WHITESPACE;

	if (js[parser_position] == ':') {

		//Advance to the next token
        SKIP_WHITESPACE
        goto *key_jump[js[parser_position]];

	}
	else {
		goto S_ERROR_MISSING_COLON_AFTER_KEY;
	}



	//=============================================================
S_PARSE_STRING_IN_ARRAY:
	parent_sizes[current_depth] += 1;

    SET_TYPE(TYPE_STRING);

    EXPAND_DATA_CHECK(N_DATA_STRING)
	data[++current_data_index] = parser_position;
	seek_string_end(js, &parser_position);
    data[++current_data_index] = parser_position;

	PROCESS_END_OF_ARRAY_VALUE;



	//=============================================================
S_PARSE_STRING_IN_KEY:

	SET_TYPE(TYPE_STRING);

    EXPAND_DATA_CHECK(N_DATA_STRING)
	data[++current_data_index] = parser_position;
	seek_string_end(js, &parser_position);
    data[++current_data_index] = parser_position;

	PROCESS_END_OF_KEY_VALUE



	//=============================================================
S_PARSE_NUMBER_IN_KEY:

    SET_TYPE(TYPE_NUMBER);

    EXPAND_DATA_CHECK(N_DATA_NUMERIC)
    
    data[++current_data_index] = parser_position;
	data[++current_data_index] = string_to_double_no_math(js + parser_position, &pEndNumber);
	parser_position = (int)(pEndNumber - js);
	parser_position--;

	PROCESS_END_OF_KEY_VALUE



	//=============================================================
S_PARSE_NUMBER_IN_ARRAY:
	parent_sizes[current_depth] += 1;
    
	SET_TYPE(TYPE_NUMBER);

	EXPAND_DATA_CHECK(N_DATA_NUMERIC)
    data[++current_data_index] = parser_position;
	data[++current_data_index] = string_to_double_no_math(js + parser_position, &pEndNumber);
	parser_position = (int)(pEndNumber - js);
    

    
    //TODO:
// // //     if(js[parser_position] == ','){
// // //         
// // //         else{

	//TODO: Let's get rid of this by incorporating it below
	parser_position--;

	//TODO: I think we would get some speed gains by correctly guessing
	//the structure
	// -> typically ", " <- comma then space then number

    //Navigation --------------------------------
	PROCESS_END_OF_ARRAY_VALUE;

//=============================================================
S_PARSE_NULL_IN_KEY:

	SET_TYPE(TYPE_NULL);

	EXPAND_DATA_CHECK(N_DATA_NULL)

    data[++current_data_index] = MX_NAN;
    
	parser_position += 3;
    
	PROCESS_END_OF_KEY_VALUE



//=============================================================
S_PARSE_NULL_IN_ARRAY:

	parent_sizes[current_depth] += 1;
    
	SET_TYPE(TYPE_NULL);

	EXPAND_DATA_CHECK(N_DATA_NULL)
    
    data[++current_data_index] = MX_NAN;

	parser_position += 3;
    
	PROCESS_END_OF_ARRAY_VALUE;


//=============================================================
S_PARSE_TRUE_IN_KEY:
    
    SET_TYPE(TYPE_LOGICAL);

    EXPAND_DATA_CHECK(N_DATA_LOGICAL)
    
    data[++current_data_index] = 1;
    
	parser_position += 3;
	PROCESS_END_OF_KEY_VALUE


S_PARSE_TRUE_IN_ARRAY:
	parent_sizes[current_depth] += 1;
    
	SET_TYPE(TYPE_LOGICAL);
    
    EXPAND_DATA_CHECK(N_DATA_LOGICAL)
    
    data[++current_data_index] = 1;

	parser_position += 3;
    
    PROCESS_END_OF_ARRAY_VALUE;

    
S_PARSE_FALSE_IN_KEY:
	SET_TYPE(TYPE_LOGICAL);

    EXPAND_DATA_CHECK(N_DATA_LOGICAL)
    
    data[++current_data_index] = 0;
    
	parser_position += 4;

    PROCESS_END_OF_KEY_VALUE

            
S_PARSE_FALSE_IN_ARRAY:
	parent_sizes[current_depth] += 1;
    
	SET_TYPE(TYPE_LOGICAL);
    
    EXPAND_DATA_CHECK(N_DATA_LOGICAL)
    
    data[++current_data_index] = 0;

	parser_position += 4;
    
	PROCESS_END_OF_ARRAY_VALUE;



	//=============================================================
// // // S_PARSE_END_OF_VALUE_IN_KEY:
// // // 
// // // 	SKIP_WHITESPACE;
// // // 
// // // 	switch (js[parser_position]) {
// // //         case ',':
// // //             goto S_PARSE_COMMA_IN_OBJECT;
// // //         case '}':
// // //             goto S_CLOSE_KEY_AND_OBJECT;
// // //         default:
// // //             goto S_ERROR_END_OF_VALUE_IN_KEY;
// // // 	}


	//=============================================================
S_PARSE_END_OF_FILE:
	SKIP_WHITESPACE

		if (!(js[parser_position] == '\0')) {
			mexErrMsgIdAndTxt("jsmn_mex:invalid_end", "non-whitespace characters found after end of root token close");
		}

	goto finish_main;



	//=============================================================
// // // S_PARSE_COMMA_IN_OBJECT:
// // // 
// // // 	SKIP_WHITESPACE
// // // 		--current_depth;
// // // 
// // // 	if (js[parser_position] == '"') {
// // // 		goto S_PARSE_KEY;
// // // 	}
// // // 	else {
// // // 		//TODO: Change this ...
// // // 		mexErrMsgIdAndTxt("jsmn_mex:no_key", "Key expected");
// // // 	}



	//=============================================================
S_ERROR_OPEN_OBJECT:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "S_ERROR_OPEN_OBJECT");

S_ERROR_MISSING_COLON_AFTER_KEY:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "S_ERROR_MISSING_COLON_AFTER_KEY");

//TODO: Describe when this error is called    
S_ERROR_END_OF_VALUE_IN_KEY:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "Token of key must be followed by a comma or a closing object ""}"" character");

//TODO: Open array points here now too
S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY");
	//mexErrMsgIdAndTxt("jsmn_mex:no_primitive","Primitive value was not found after the comma");
   
//TODO: Open array points here now too
S_ERROR_TOKEN_AFTER_KEY:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "S_ERROR_TOKEN_AFTER_KEY");
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
    
    
    //TODO: make realloc calls here ...
    setStructField(plhs[0],types,"types",mxUINT8_CLASS,current_type_index + 1);
	setStructField(plhs[0],data,"data",mxINT32_CLASS,current_data_index + 1);
    
// // //     setStructField(plhs[0],values,"values",mxDOUBLE_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],types,"types",mxUINT8_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],sizes,"sizes",mxINT32_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],parents,"parents",mxINT32_CLASS,n_tokens_used);    
    
    
    
    
    
	return current_type_index + 1;

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



