#include "jsmn.h"

//This is needed for AVX
//but, we might not use AVX - SSE4
//TODO: Need to build in SSE4 and AVX testing support
//with define flags ...
#include "immintrin.h"
#include "nmmintrin.h"


//TODO: Allow string input to function

//TODO: Allow type, d1, and d2 pointer walking
//NOTE: This will require a separate pointer that gets updated on resizing

//TODO: store initial and final allocation sizes for each type
//TODO: Create method for creating scalar and saving into struct


//TODO: Pad end with nulls and maybe a string end???
//TODO: Figure out SSE2/SSE4/AVX
//TODO: Assume TAC is 1 ahead
//TODO: Build in if statements on keys
//TODO: Pad with empty first setting to allow using uint instead of int (padding would allow removal of -1)

//TODO: replace with a goto for more information
#define ERROR_DEPTH mexErrMsgIdAndTxt("jsmn_mex:depth_limit","Max depth exceeded");

#define N_INT_PART  data_info[0]
#define N_FRAC_PART data_info[1]
#define N_EXP_PART  data_info[2]

//DEBUGGING

#define PRINT_CURRENT_POSITION mexPrintf("Current Position: %d\n",CURRENT_INDEX);
#define PRINT_CURRENT_CHAR  mexPrintf("Current Char: %c\n",CURRENT_CHAR);


//Things for opening ======================================================

#define INCREMENT_PARENT_SIZE parent_sizes[current_depth] += 1

#define EXPAND_DATA_CHECK \
    ++current_data_index; \
	if (current_data_index >= data_size_index_max){ \
        data_size_allocated = ceil(1.5*data_size_allocated); \
        data_size_index_max = data_size_allocated-1; \
        \
        types = mxRealloc(types,data_size_allocated); \
        d1 = mxRealloc(d1,data_size_allocated*sizeof(int)); \
        d2 = mxRealloc(d2,data_size_allocated*sizeof(int)); \
    } \

#define EXPAND_KEY_CHECK \
    ++current_key_index; \
    if (current_key_index >= key_size_index_max) { \
        key_size_allocated = ceil(1.5*key_size_allocated); \
        key_size_index_max = key_size_allocated - 1; \
        key_p = mxRealloc(key_p,key_size_allocated * sizeof(unsigned char *)); \
    } \
            
#define EXPAND_STRING_CHECK \
    ++current_string_index; \
    if (current_string_index >= string_size_index_max) { \
        string_size_allocated = ceil(1.5*string_size_allocated); \
        string_size_index_max = string_size_allocated - 1; \
        string_p = mxRealloc(string_p,string_size_allocated * sizeof(unsigned char *)); \
    } \
            
#define EXPAND_NUMERIC_CHECK \
    ++current_numeric_index; \
    if (current_numeric_index >= numeric_size_index_max) { \
        numeric_size_allocated = ceil(1.5*numeric_size_allocated); \
        numeric_size_index_max = numeric_size_allocated - 1; \
        numeric_p = mxRealloc(numeric_p,numeric_size_allocated * sizeof(unsigned char *)); \
    } \
            
#define SET_TYPE(x) types[current_data_index] = x;

//TODO: The size isn't needed for keys
#define INITIALIZE_PARENT_INFO(x) \
        ++current_depth; \
        if (current_depth > 200){\
            goto S_ERROR_DEPTH_EXCEEDED; \
        }\
        parent_types[current_depth] = x; \
        parent_indices[current_depth] = current_data_index; \
        parent_sizes[current_depth] = 0; \
            
//=========================================================================
 
//=================      Processing    ====================================
//=========================================================================
#define PROCESS_NUMBER \
    EXPAND_NUMERIC_CHECK; \
    EXPAND_DATA_CHECK; \
    \
    SET_TYPE(TYPE_NUMBER); \
    \
    numeric_p[current_numeric_index] = CURRENT_POINTER; \
    d1[current_data_index] = current_numeric_index; \
    \
    string_to_double_no_math(CURRENT_POINTER, &CURRENT_POINTER); \
                
#define PROCESS_NULL \
    EXPAND_NUMERIC_CHECK; \
    EXPAND_DATA_CHECK; \
    \
    SET_TYPE(TYPE_NULL); \
    \
    numeric_p[current_numeric_index] = CURRENT_POINTER; \
    d1[current_data_index] = current_numeric_index; \
    \
    /*TODO: Add null check ... */ \
	ADVANCE_POINTER_BY_X(3) \
            
#define PROCESS_STRING \
    EXPAND_STRING_CHECK; \
    EXPAND_DATA_CHECK; \
    SET_TYPE(TYPE_STRING); \
    \
    string_p[current_string_index] = CURRENT_POINTER; \
    d1[current_data_index] = current_string_index; \
    \
    seek_string_end_v2(CURRENT_POINTER,&CURRENT_POINTER); \
            
            
//================    Things for closing             ======================
//=========================================================================     
//+1 to next element
//+1 for Matlab 1 based indexing
#define STORE_TAC  d2[current_parent_index] = current_data_index + 2;
#define STORE_SIZE d1[current_parent_index] = parent_sizes[current_depth];
            
#define MOVE_UP_PARENT_INDEX --current_depth;

#define IS_NULL_PARENT_INDEX current_depth == 0     
            
#define PARENT_TYPE parent_types[current_depth]            
//=========================================================================
  
//================      Navigation       ==================================
//=========================================================================      
#define CURRENT_CHAR *p
#define CURRENT_POINTER p
#define CURRENT_INDEX p - js
#define ADVANCE_POINTER_AND_GET_CHAR_VALUE *(++p)
#define DECREMENT_POINTER --p
#define ADVANCE_POINTER_BY_X(x) p += x;
#define REF_OF_CURRENT_POINTER &p;
    
//tested that is_whitespce was faster than isspace()    

//I'm not thrilled with the __m128i
//Note with this mode, most of these options are defaults (i.e. 0 values)
//_SIDD_UBYTE_OPS - unsigned 8-bit characters
//For each character c in a, determine whether any character in b is equal to c. <= from MSDN, I think the flip
//logic would be better
//_SIDD_NEGATIVE_POLARITY - negation of resulting bitmask, i.e. find first non-match
//_SIDD_BIT_MASK - mask itself is returned, each bit not expanded to bytes - I don't think this is needed
const int ws_search_mode = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_NEGATIVE_POLARITY | _SIDD_BIT_MASK;
//const __m128i whitespace_characters = (__m128i)(_mm_set_ss(0x090A0D20));
//const __m128i whitespace_characters = _mm_set1_epi32(0x090A0D20);
__m128i whitespace_characters;
__m128i chars_to_search_for_ws;
int ws_search_result;

#define ADVANCE_TO_NON_WHITESPACE_CHAR while(is_whitespace[ADVANCE_POINTER_AND_GET_CHAR_VALUE]){}        
   
            
            
// #define ADVANCE_TO_NON_WHITESPACE_CHAR  \
//     /* This has a lot of failures on newlines :/ */
//     /* we might do an OR with \n */
//     if (*(++p) == ' '){ \
//         ++p; \
//     } \
//     if (*p > ' '){ \
//         /* all done, all whitespace values are less than this */ \
//     }else{
//         chars_to_search_for_ws = _mm_loadu_si128((__m128i*)p);
//         ws_search_result = _mm_cmpistri(whitespace_characters, 4, chars_to_search_for_ws, 16, ws_search_mode);
// 
//         int wtf2 = _mm_cmpistri (whitespace_characters, 16, simd_b , 4, ws_search_mode);
//         mexPrintf("Result is: %d"\n,wtf2);
//     }

            
#define DO_KEY_JUMP   goto *key_jump[CURRENT_CHAR]
     
#define DO_ARRAY_JUMP goto *array_jump[CURRENT_CHAR]
                
#define PROCESS_END_OF_ARRAY_VALUE \
	ADVANCE_TO_NON_WHITESPACE_CHAR; \
	switch (CURRENT_CHAR) { \
        case ',': \
            ADVANCE_TO_NON_WHITESPACE_CHAR; \
            DO_ARRAY_JUMP; \
        case ']': \
            goto S_CLOSE_ARRAY; \
        default: \
            goto S_ERROR_END_OF_VALUE_IN_ARRAY; \
	} \
             
#define PROCESS_END_OF_KEY_VALUE \
    ADVANCE_TO_NON_WHITESPACE_CHAR; \
	switch (CURRENT_CHAR) { \
        case ',': \
            current_parent_index = parent_indices[current_depth]; \
            STORE_TAC; \
            MOVE_UP_PARENT_INDEX; \
            ADVANCE_TO_NON_WHITESPACE_CHAR; \
            if (CURRENT_CHAR == '"') { \
                goto S_PARSE_KEY; \
            } \
            else { \
                mexPrintf("Position %d\n",CURRENT_INDEX); \
                mexErrMsgIdAndTxt("jsmn_mex:no_key", "Key expected"); \
            } \
        case '}': \
            goto S_CLOSE_KEY_AND_OBJECT; \
        default: \
            goto S_ERROR_END_OF_VALUE_IN_KEY; \
	} \

//=========================================================================
            
const bool is_whitespace[256] = { false,false,false,false,false,false,false,false,false,true,true,false,false,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };

void setStructField(mxArray *s, void *pr, const char *fieldname, mxClassID classid, mwSize N)
{
    
    //This function is used to set a field in the output struct
    
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
void string_to_double_no_math(unsigned char *p, unsigned char **char_offset) {

    //strcspn - should know what to look for ...
    //Would need to confirm that we end at the correct location
    
    //An alternative approach would be to look for the closing string
    //e.g.: ',' '}' ']' and then check all of the math when running
    //in parallel
    
    //We're point at a negative or a digit
    
    //I'm not necessarily thrilled with this approach
    //TODO: test the alternative code with loops
    //and checks for overflow and subtraction for populating the data
    
    
    if (*p == '-'){
        ++p;
    }

    //http://stackoverflow.com/questions/15372885/if-statements-with-comparison-sse-in-c?rq=1
    
    while (isdigit(*p)) {++p;}

    if (*p == '.'){
        ++p;
        while (isdigit(*p)) {++p;}
    }

    if (*p == 'E' || *p == 'e') {
        ++p;
        if (*p == '-' || *p == '+'){
            ++p;
        }

        while (isdigit(*p)) {++p;}
    }
    
    *char_offset = p;    
}

void seek_string_end_v2(unsigned char *p, unsigned char **char_offset){

STRING_SEEK:    
    //p+1 to advance past initial '"'
    p = strchr(p+1,'"');
    
    //Back up to verify 
    --p;
    
    //I've padded the string with a quote that is preceeded by a null
    //so a null indicates that there really was no "
    if (*p == '\0'){
        mexErrMsgIdAndTxt("jsmn_mex:missing_quotes", "Quote not found");
    }
    
    if (*p == '\\'){
        --p;
        if (*p == '\\'){
            mexErrMsgIdAndTxt("jsmn_mex:unhandled_case", "Code not yet written");
        }
        ++p;
        goto STRING_SEEK;
    }else{
        *char_offset = p+1;
    }
    
}

//=========================================================================
//              Parse JSON   -    Parse JSON    -    Parse JSON
//=========================================================================
void jsmn_parse(unsigned char *js, size_t string_byte_length, mxArray *plhs[]) {
    
    whitespace_characters = _mm_set1_epi32(0x090A0D20);
    
    const void *array_jump[256] = {
        [0 ... 33]  = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [34]        = &&S_PARSE_STRING_IN_ARRAY,            // "
        [35 ... 44] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [45]        = &&S_PARSE_NUMBER_IN_ARRAY,            // -
        [46 ... 47] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [48 ... 57] = &&S_PARSE_NUMBER_IN_ARRAY,            // #
        [58 ... 90] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [91]        = &&S_OPEN_ARRAY_IN_ARRAY,              // [
        [92 ... 101]  = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [102]         = &&S_PARSE_FALSE_IN_ARRAY,           // false
        [103 ... 109] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [110]         = &&S_PARSE_NULL_IN_ARRAY,            // null
        [111 ... 115] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [116]         = &&S_PARSE_TRUE_IN_ARRAY,            // true
        [117 ... 122] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [123]         = &&S_OPEN_OBJECT_IN_ARRAY,           // {
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
    
    unsigned char *p = js;    

    
    const int MAX_DEPTH = 200;
    int parent_types[201];
    //Note, this needs to be indices instead of pointers because
    //we might resize and the indices would become invalid
    int parent_indices[201];
    int parent_sizes[201];
    int current_parent_index;
    int current_depth = 0;

    int data_size_allocated = ceil(string_byte_length/4);
    int data_size_index_max = data_size_allocated - 1;
    int current_data_index = 0;
    
    uint8_t *types = mxMalloc(data_size_allocated);
    //do we need to make these uint64?
    //technically not, the start pointer is an index
    //and not an address
    //
    //d1 - n_values and start pointer index
    //d2 - tac
    int *d1 = mxMalloc(data_size_allocated * sizeof(int));
    int *d2 = mxMalloc(data_size_allocated * sizeof(int));

    int key_size_allocated = ceil(string_byte_length/20);
    int key_size_index_max = key_size_allocated-1;
    int current_key_index = 0;
    unsigned char **key_p = mxMalloc(key_size_allocated * sizeof(unsigned char *));        
    
    int string_size_allocated = ceil(string_byte_length/20);
    int string_size_index_max = string_size_allocated-1;
    int current_string_index = 0;
    unsigned char **string_p = mxMalloc(string_size_allocated * sizeof(unsigned char *));
    
    int numeric_size_allocated = ceil(string_byte_length/4);
    int numeric_size_index_max = numeric_size_allocated - 1;
    int current_numeric_index = 0;
    unsigned char **numeric_p = mxMalloc(numeric_size_allocated * sizeof(unsigned char *));
    
                               // 123 4 5 6789
    unsigned char *test_string = "   \t\n\ris a test of the emergency";
    
    chars_to_search_for_ws = _mm_loadu_si128((__m128i*)(test_string+2));
    //ws_search_result = _mm_cmpistri(whitespace_characters, chars_to_search_for_ws, ws_search_mode);
    ws_search_result = _mm_cmpestri(whitespace_characters, 4, chars_to_search_for_ws, 16, ws_search_mode);
    mexPrintf("Result is: %d\n",ws_search_result);
    
    DECREMENT_POINTER;
	ADVANCE_TO_NON_WHITESPACE_CHAR;

	switch (CURRENT_CHAR) {
        case '{':
            goto S_OPEN_OBJECT_IN_KEY;
        case '[':
            goto S_OPEN_ARRAY_IN_KEY;
        default:
            mexErrMsgIdAndTxt("jsmn_mex:invalid_start", "Starting token needs to be an opening object or array");
	}

//=============================================================
S_OPEN_OBJECT_IN_ARRAY:
    
    INCREMENT_PARENT_SIZE;

	//Fall Through --------------------
S_OPEN_OBJECT_IN_KEY:
    
    //1) Expand data if necessary
    //2) setup parent info
    
    EXPAND_DATA_CHECK;
    
    SET_TYPE(TYPE_OBJECT);
    
    INITIALIZE_PARENT_INFO(TYPE_OBJECT);  
    
    //Navigation -----------------
	ADVANCE_TO_NON_WHITESPACE_CHAR;

    switch (CURRENT_CHAR) {
        case '"':
            goto S_PARSE_KEY;
        case '}':
            goto S_CLOSE_OBJECT;
        default:
            goto S_ERROR_OPEN_OBJECT;
    }

//=============================================================

S_CLOSE_KEY_AND_OBJECT:    
    //Update tac and parent
        
    current_parent_index = parent_indices[current_depth];
            
    STORE_TAC;
    
    //Fall Through ------
S_CLOSE_KEY_AND_OBJECT_SIMPLE:
    
    //Only update parent
    MOVE_UP_PARENT_INDEX;

	//Fall Through ------
S_CLOSE_OBJECT:

    current_parent_index = parent_indices[current_depth];
    STORE_TAC;
    STORE_SIZE;
    MOVE_UP_PARENT_INDEX;
    
    if (IS_NULL_PARENT_INDEX) {
		goto S_PARSE_END_OF_FILE;
	}
    
    if (PARENT_TYPE == TYPE_KEY) {
    	PROCESS_END_OF_KEY_VALUE;
    } else {
        PROCESS_END_OF_ARRAY_VALUE;
    }
    
//=============================================================
S_OPEN_ARRAY_IN_ARRAY:
	INCREMENT_PARENT_SIZE;

	//Fall Through -------------------------------
S_OPEN_ARRAY_IN_KEY:

    EXPAND_DATA_CHECK;
    
    SET_TYPE(TYPE_ARRAY);
    
    INITIALIZE_PARENT_INFO(TYPE_ARRAY); 
    
	ADVANCE_TO_NON_WHITESPACE_CHAR;
    DO_ARRAY_JUMP;
            
	//=============================================================
S_CLOSE_ARRAY:

    current_parent_index = parent_indices[current_depth];
    STORE_TAC;
    STORE_SIZE;
    MOVE_UP_PARENT_INDEX;
    
	if (IS_NULL_PARENT_INDEX) {
		goto S_PARSE_END_OF_FILE;
	}
    
    if (PARENT_TYPE == TYPE_KEY) {
        PROCESS_END_OF_KEY_VALUE;
    } else {
        PROCESS_END_OF_ARRAY_VALUE;
    }

//=============================================================
S_PARSE_KEY:
	INCREMENT_PARENT_SIZE;
    
    EXPAND_KEY_CHECK;
    
    EXPAND_DATA_CHECK;
    
    INITIALIZE_PARENT_INFO(TYPE_KEY);
    
    SET_TYPE(TYPE_KEY);

    key_p[current_string_index] = CURRENT_POINTER;
    d1[current_data_index] = current_string_index;
        
    seek_string_end_v2(CURRENT_POINTER,&CURRENT_POINTER);
    
    //We're getting an extra bit of memory for storing the end :/
    EXPAND_DATA_CHECK;
    
    if (ADVANCE_POINTER_AND_GET_CHAR_VALUE == ':'){
        ADVANCE_TO_NON_WHITESPACE_CHAR;
        DO_KEY_JUMP;    
    }else{
        DECREMENT_POINTER;
        ADVANCE_TO_NON_WHITESPACE_CHAR;

        if (CURRENT_CHAR == ':') {
            ADVANCE_TO_NON_WHITESPACE_CHAR;
            DO_KEY_JUMP;
        }
        else {
            goto S_ERROR_MISSING_COLON_AFTER_KEY;
        }
    }

//=============================================================
S_PARSE_STRING_IN_ARRAY:
	INCREMENT_PARENT_SIZE;
    
    PROCESS_STRING

	PROCESS_END_OF_ARRAY_VALUE;

//=============================================================
S_PARSE_STRING_IN_KEY:

    PROCESS_STRING

	PROCESS_END_OF_KEY_VALUE


//=============================================================
S_PARSE_NUMBER_IN_KEY:
    
    PROCESS_NUMBER
    
    DECREMENT_POINTER;

	PROCESS_END_OF_KEY_VALUE;

//=============================================================
S_PARSE_NUMBER_IN_ARRAY:
    
	INCREMENT_PARENT_SIZE;
    
    PROCESS_NUMBER
               
    if (CURRENT_CHAR == ','){
        ADVANCE_TO_NON_WHITESPACE_CHAR;
        DO_ARRAY_JUMP;
    }else{
        DECREMENT_POINTER;
        PROCESS_END_OF_ARRAY_VALUE;
    }

//=============================================================
S_PARSE_NULL_IN_KEY:

    PROCESS_NULL;
    
	PROCESS_END_OF_KEY_VALUE;

//=============================================================
S_PARSE_NULL_IN_ARRAY:

	INCREMENT_PARENT_SIZE;
    
    PROCESS_NULL
    
	PROCESS_END_OF_ARRAY_VALUE;

//=============================================================
S_PARSE_TRUE_IN_KEY:
    
    EXPAND_DATA_CHECK;
    SET_TYPE(TYPE_TRUE);
	ADVANCE_POINTER_BY_X(3);
    
	PROCESS_END_OF_KEY_VALUE


S_PARSE_TRUE_IN_ARRAY:
    
	INCREMENT_PARENT_SIZE;
    
    EXPAND_DATA_CHECK;
    SET_TYPE(TYPE_TRUE);
	ADVANCE_POINTER_BY_X(3);
    
    PROCESS_END_OF_ARRAY_VALUE;

    
S_PARSE_FALSE_IN_KEY:
    
    EXPAND_DATA_CHECK;
    SET_TYPE(TYPE_FALSE);
	ADVANCE_POINTER_BY_X(4);
    
    PROCESS_END_OF_KEY_VALUE;

S_PARSE_FALSE_IN_ARRAY:
    
	INCREMENT_PARENT_SIZE;
    
    EXPAND_DATA_CHECK;
    SET_TYPE(TYPE_FALSE);
	ADVANCE_POINTER_BY_X(4);
    
	PROCESS_END_OF_ARRAY_VALUE;

	//=============================================================
S_PARSE_END_OF_FILE:
	ADVANCE_TO_NON_WHITESPACE_CHAR

		if (!(CURRENT_CHAR == '\0')) {
			mexErrMsgIdAndTxt("jsmn_mex:invalid_end", "non-whitespace characters found after end of root token close");
		}

	goto finish_main;


//===============       ERRORS   ==========================================
//=========================================================================
//TODO: This is going to be redone 
    
S_ERROR_DEPTH_EXCEEDED:
    mexErrMsgIdAndTxt("jsmn_mex:depth_exceeded", "Max depth was exceeded");
    
S_ERROR_OPEN_OBJECT:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "S_ERROR_OPEN_OBJECT");

S_ERROR_MISSING_COLON_AFTER_KEY:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "S_ERROR_MISSING_COLON_AFTER_KEY");

//TODO: Describe when this error is called    
S_ERROR_END_OF_VALUE_IN_KEY:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "Token of key must be followed by a comma or a closing object ""}"" character");

//This error comes when we have a comma in an array that is not followed
// by a valid value => i.e. #, ", [, {, etc.
S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY:
    mexPrintf("Current character: %c\n",CURRENT_CHAR);
    mexPrintf("Current position in string: %d\n",CURRENT_INDEX);
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "Invalid token found after a comma in an array");
	//mexErrMsgIdAndTxt("jsmn_mex:no_primitive","Primitive value was not found after the comma");
   
//TODO: Open array points here now too
S_ERROR_TOKEN_AFTER_KEY:
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "S_ERROR_TOKEN_AFTER_KEY");
	//mexErrMsgIdAndTxt("jsmn_mex:no_primitive","Primitive value was not found after the comma");    
    

S_ERROR_END_OF_VALUE_IN_ARRAY:  
	mexPrintf("Current position: %d\n", CURRENT_INDEX);
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "Token in array must be followed by a comma or a closing array ""]"" character ");    

finish_main:
    
    types = mxRealloc(types,(current_data_index + 1));
    setStructField(plhs[0],types,"types",mxUINT8_CLASS,current_data_index + 1);
    
    d1 = mxRealloc(d1,((current_data_index + 1)*sizeof(int)));
    setStructField(plhs[0],d1,"d1",mxINT32_CLASS,current_data_index + 1);
    
    d2 = mxRealloc(d2,((current_data_index + 1)*sizeof(int)));
    setStructField(plhs[0],d2,"d2",mxINT32_CLASS,current_data_index + 1);

    //TODO: This is only correct on 64 bit systems ...
    key_p = mxRealloc(key_p,(current_key_index + 1)*sizeof(unsigned char *));
    setStructField(plhs[0],key_p,"key_p",mxUINT64_CLASS,current_key_index + 1);
    
    string_p = mxRealloc(string_p,(current_string_index + 1)*sizeof(unsigned char *));
    setStructField(plhs[0],string_p,"string_p",mxUINT64_CLASS,current_string_index + 1);
    
    numeric_p = mxRealloc(numeric_p,(current_numeric_index + 1)*sizeof(unsigned char *));
    setStructField(plhs[0],numeric_p,"numeric_p",mxUINT64_CLASS,current_numeric_index + 1);

	return;
}



