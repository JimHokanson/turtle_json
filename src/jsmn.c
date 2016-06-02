#include "jsmn.h"

//KEY PROCESSING
//---------------
//In order to improve processing we key TAC and parent information
//processing in the key's value. For simple values such as numbers or
//strings we have relatively little to do. For complex values (objects 
//and arrays) we log the key as a parent node which will have subsequent 
//children.
//
//number, string, null, true, false - store TAC, 
//array - store parent info
//
//When closing, we need to call different comma processors or closers.


// parent type - use pointers

//TODO: Allow string input to function

//TODO: store initial and final allocation sizes for each type
//TODO: Create method for creating scalar and saving into struct - for above TODO


//TODO: Build in if statements on keys

//TODO: replace with a goto for more information


//DEBUGGING
#define ERROR_DEPTH goto S_ERROR_DEPTH;

#define PRINT_CURRENT_POSITION mexPrintf("Current Position: %d\n",CURRENT_INDEX);
#define PRINT_CURRENT_CHAR  mexPrintf("Current Char: %c\n",CURRENT_CHAR);

//=========================================================================
//              Data Allocation and index advancement
//=========================================================================

#define INCREMENT_DATA_INDEX \
    ++current_data_index; \
	if (current_data_index >= data_size_index_max){ \
        data_size_allocated = ceil(1.5*data_size_allocated); \
        data_size_index_max = data_size_allocated-1; \
        \
        types = mxRealloc(types,data_size_allocated); \
        d1 = mxRealloc(d1,data_size_allocated*sizeof(int)); \
        d2 = mxRealloc(d2,data_size_allocated*sizeof(int)); \
    }

#define ALLOCATE_KEY_DATA \
    unsigned char **key_p = mxMalloc(key_size_allocated * sizeof(unsigned char *)); \
    int *key_end_indices = mxMalloc(key_size_allocated * sizeof(int)); \
    int *key_start_indices = mxMalloc(key_size_allocated * sizeof(int));
#define INCREMENT_KEY_INDEX \
    ++current_key_index; \
    if (current_key_index >= key_size_index_max) { \
        key_size_allocated = ceil(1.5*key_size_allocated); \
        key_size_index_max = key_size_allocated - 1; \
        key_p = mxRealloc(key_p,key_size_allocated * sizeof(unsigned char *)); \
        key_end_indices = mxRealloc(key_end_indices,key_size_allocated * sizeof(int)); \
        key_start_indices = mxRealloc(key_start_indices,key_size_allocated * sizeof(int)); \
    }
#define TRUNCATE_KEY_DATA \
    key_p = mxRealloc(key_p,(current_key_index + 1)*sizeof(unsigned char *)); \
    key_end_indices = mxRealloc(key_end_indices,(current_key_index + 1) * sizeof(int)); \
    key_start_indices = mxRealloc(key_start_indices,(current_key_index + 1) * sizeof(int));
    
    
#define ALLOCATE_STRING_DATA \
    unsigned char **string_p = mxMalloc(string_size_allocated * sizeof(unsigned char *)); \
    int *string_end_indices = mxMalloc(string_size_allocated * sizeof(int)); \
    int *string_start_indices = mxMalloc(string_size_allocated * sizeof(int));
#define INCREMENT_STRING_INDEX \
    ++current_string_index; \
    if (current_string_index >= string_size_index_max) { \
        string_size_allocated = ceil(1.5*string_size_allocated); \
        string_size_index_max = string_size_allocated - 1; \
        string_p = mxRealloc(string_p,string_size_allocated * sizeof(unsigned char *)); \
        string_end_indices = mxRealloc(string_end_indices,string_size_allocated * sizeof(int)); \
        string_start_indices = mxRealloc(string_start_indices,string_size_allocated * sizeof(int)); \
    }
#define TRUNCATE_STRING_DATA \
    string_p = mxRealloc(string_p,(current_string_index + 1)*sizeof(unsigned char *)); \
	string_end_indices = mxRealloc(string_end_indices,(current_string_index + 1) * sizeof(int)); \
 	string_start_indices = mxRealloc(string_start_indices,(current_string_index + 1) * sizeof(int));
    

#define ALLOCATE_NUMERIC_DATA unsigned char **numeric_p = mxMalloc(numeric_size_allocated * sizeof(unsigned char *));  
//uint64_t *numeric_p = mxMalloc(numeric_size_allocated * sizeof(uint64_t));    
#define EXPAND_NUMERIC_CHECK \
    ++current_numeric_index; \
    if (current_numeric_index >= numeric_size_index_max) { \
        numeric_size_allocated = ceil(1.5*numeric_size_allocated); \
        numeric_size_index_max = numeric_size_allocated - 1; \
        numeric_p = mxRealloc(numeric_p,numeric_size_allocated * sizeof(unsigned char *)); \
    }
#define TRUNCATE_NUMERIC_DATA numeric_p = mxRealloc(numeric_p,(current_numeric_index + 1)*sizeof(unsigned char *));

//Things for opening ======================================================
#define INCREMENT_PARENT_SIZE parent_sizes[current_depth] += 1

#define SET_TYPE(x) types[current_data_index] = x;            
            
//TODO: The size isn't needed for keys
#define INITIALIZE_PARENT_INFO(x) \
        ++current_depth; \
        if (current_depth > 200){\
            goto S_ERROR_DEPTH_EXCEEDED; \
        }\
        parent_types[current_depth] = x; \
        parent_indices[current_depth] = current_data_index; \
        parent_sizes[current_depth] = 0;
            
//=========================================================================
//=================      Processing    ====================================
//=========================================================================
// The main code is essentially a state machine. States within the machine
// have the following rough layout:
//
//      1) key or array specific initialization code
//      2) common processing
//      3) navigation to the next state
//        
//  Code below is for #2, the processing that is commen to the type
//  regardless of whether or not it is in a key or array
        
#define PROCESS_OPENING_OBJECT \
    INCREMENT_DATA_INDEX; \
    SET_TYPE(TYPE_OBJECT); \
    INITIALIZE_PARENT_INFO(TYPE_OBJECT);
    
#define PROCESS_OPENING_ARRAY \
    INCREMENT_DATA_INDEX; \
    SET_TYPE(TYPE_ARRAY); \
    INITIALIZE_PARENT_INFO(TYPE_ARRAY);    

#define PROCESS_STRING \
    INCREMENT_STRING_INDEX; \
    INCREMENT_DATA_INDEX; \
    SET_TYPE(TYPE_STRING); \
    temp_p = CURRENT_POINTER; \
    string_p[current_string_index] = CURRENT_POINTER + 1; \
    string_start_indices[current_string_index] = n_string_chars; \
    d1[current_data_index] = current_string_index + 1; \
    seek_string_end(CURRENT_POINTER,&CURRENT_POINTER); \
    n_string_chars += CURRENT_POINTER - string_p[current_string_index]; \
    string_end_indices[current_string_index] = n_string_chars;
            
    //d2[current_data_index] = CURRENT_POINTER - temp_p;        
            
    //d2[current_data_index] = CURRENT_POINTER - string_p[current_string_index];

#define PROCESS_KEY \
    INCREMENT_KEY_INDEX; \
    INCREMENT_DATA_INDEX; \
    /* This step is now done in some key values */ \
    /* //INITIALIZE_PARENT_INFO(TYPE_KEY); */ \
    SET_TYPE(TYPE_KEY); \
    /* We want to skip the opening quotes */ \
    key_p[current_key_index] = CURRENT_POINTER + 1; \
    key_start_indices[current_key_index]   = n_key_chars; \
    d1[current_data_index]   = current_key_index + 1; \
    seek_string_end(CURRENT_POINTER,&CURRENT_POINTER); \
    /* We won't count the closing quote, but we would normally add 1 to be inclusive on a count, so they cancel out */ \
    n_key_chars += CURRENT_POINTER - key_p[current_key_index]; \
    key_end_indices[current_key_index] = n_key_chars;
    
// Option 1            
//     if ((CURRENT_POINTER - key_p[current_key_index]) > max_key_size){ \
//         max_key_size = CURRENT_POINTER - key_p[current_key_index]; \
//     }
           
// Option 2    
//     n_chars_key = CURRENT_POINTER - key_p[current_key_index]; \
//     max_key_size = (n_chars_key > max_key_size) ? n_chars_key : max_key_size;
    
// Option 3    
//      n_chars_key = CURRENT_POINTER - key_p[current_key_index]; \    
//     if (n_chars_key > max_key_size){ \
//         max_key_size = n_chars_key; \
//     }
            
#define PROCESS_NUMBER \
    EXPAND_NUMERIC_CHECK; \
    INCREMENT_DATA_INDEX; \
    SET_TYPE(TYPE_NUMBER); \
    numeric_p[current_numeric_index] = CURRENT_POINTER; \
    d1[current_data_index] = current_numeric_index; \
    string_to_double_no_math(CURRENT_POINTER, &CURRENT_POINTER);    
    
#define PROCESS_NULL \
    EXPAND_NUMERIC_CHECK; \
    INCREMENT_DATA_INDEX; \
    SET_TYPE(TYPE_NULL); \
    numeric_p[current_numeric_index] = 0; \
    d1[current_data_index] = current_numeric_index; \
    /*TODO: Add null check ... */ \
	ADVANCE_POINTER_BY_X(3)    
           
#define PROCESS_TRUE \
    INCREMENT_DATA_INDEX; \
    SET_TYPE(TYPE_TRUE); \
	ADVANCE_POINTER_BY_X(3);
            
#define PROCESS_FALSE \
    INCREMENT_DATA_INDEX; \
    SET_TYPE(TYPE_FALSE); \
	ADVANCE_POINTER_BY_X(4);
                
//Things for closing  =====================================================
//+1 to next element
//+1 for Matlab 1 based indexing
#define STORE_TAC_OF_OBJECT_OR_ARRAY d2[current_parent_index] = current_data_index + 2;
    
//This is called before the simple value, so we need to advance to the simple
//value and then do the next value (i.e the token after close)
//Note that we're working with the current_data_index since we haven't
//advanced it yet and don't need to rely on a parent index (which hasn't even
//been set since the value is simple)
#define STORE_TAC_KEY_SIMPLE d2[current_data_index] = current_data_index + 3;  
    
//#define STORE_TAC_KEY_COMPLEX d2[current_parent_index+1] = current_data_index + 2;    

#define STORE_TAC_KEY_COMPLEX d2[current_parent_index] = current_data_index + 2;    
            
    
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

// const bool is_whitespace[256] = { false,false,false,false,false,false,false,false,false,true,true,false,false,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };
// #define ADVANCE_TO_NON_WHITESPACE_CHAR while(is_whitespace[ADVANCE_POINTER_AND_GET_CHAR_VALUE]){}        
    
//_mm_cmpestri turns out to be slower :/

#define INIT_LOCAL_WS_CHARS \
    const __m128i whitespace_characters = _mm_set1_epi32(0x090A0D20);

#define ADVANCE_TO_NON_WHITESPACE_CHAR  \
    /* This might fail alot on newlines :/ */ \
    /* we might do an OR with \n => == ' ' OR = '\n' */ \
    if (*(++p) == ' '){ \
        ++p; \
    } \
    if (*p <= ' '){ \
        chars_to_search_for_ws = _mm_loadu_si128((__m128i*)p); \
        ws_search_result = _mm_cmpistri(whitespace_characters, chars_to_search_for_ws, simd_search_mode); \
        p += ws_search_result; \
        if (ws_search_result == 16) { \
            while (ws_search_result == 16){ \
                chars_to_search_for_ws = _mm_loadu_si128((__m128i*)p); \
                ws_search_result = _mm_cmpistri(whitespace_characters, chars_to_search_for_ws, simd_search_mode); \
                p += ws_search_result; \
            } \
        } \
    } \

            
#define DO_KEY_JUMP   goto *key_jump[CURRENT_CHAR]
     
#define DO_ARRAY_JUMP goto *array_jump[CURRENT_CHAR]
                
#define NAVIGATE_AFTER_OPENING_OBJECT \
	ADVANCE_TO_NON_WHITESPACE_CHAR; \
    switch (CURRENT_CHAR) { \
        case '"': \
            goto S_PARSE_KEY; \
        case '}': \
            goto S_CLOSE_OBJECT; \
        default: \
            goto S_ERROR_OPEN_OBJECT; \
    }            
                        
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
	}
    
//This is for values following a key that are simple such as:
//number, string, null, fales, true    
    
#define PROCESS_END_OF_KEY_VALUE_SIMPLE \
    ADVANCE_TO_NON_WHITESPACE_CHAR; \
	switch (CURRENT_CHAR) { \
        case ',': \
            ADVANCE_TO_NON_WHITESPACE_CHAR; \
            if (CURRENT_CHAR == '"') { \
                goto S_PARSE_KEY; \
            } \
            else { \
                goto S_ERROR_BAD_TOKEN_FOLLOWING_OBJECT_VALUE_COMMA; \
            } \
        case '}': \
            goto S_CLOSE_OBJECT; \
        default: \
            goto S_ERROR_END_OF_VALUE_IN_KEY; \
	}          
            
#define PROCESS_END_OF_KEY_VALUE_COMPLEX \
    ADVANCE_TO_NON_WHITESPACE_CHAR; \
	switch (CURRENT_CHAR) { \
        case ',': \
            current_parent_index = parent_indices[current_depth]; \
            STORE_TAC_KEY_COMPLEX; \
            MOVE_UP_PARENT_INDEX; \
            ADVANCE_TO_NON_WHITESPACE_CHAR; \
            if (CURRENT_CHAR == '"') { \
                goto S_PARSE_KEY; \
            } \
            else { \
                goto S_ERROR_BAD_TOKEN_FOLLOWING_OBJECT_VALUE_COMMA; \
            } \
        case '}': \
            goto S_CLOSE_KEY_COMPLEX_AND_OBJECT; \
        default: \
            goto S_ERROR_END_OF_VALUE_IN_KEY; \
	}

#define NAVIGATE_AFTER_OPENING_ARRAY_OR_AFTER_COMMA_IN_ARRAY \
    ADVANCE_TO_NON_WHITESPACE_CHAR; \
    DO_ARRAY_JUMP;
    
#define NAVIGATE_AFTER_CLOSING_COMPLEX \
	if (IS_NULL_PARENT_INDEX) { \
		goto S_PARSE_END_OF_FILE; \
	} else if (PARENT_TYPE == TYPE_KEY) { \
        PROCESS_END_OF_KEY_VALUE_COMPLEX; \
    } else { \
        PROCESS_END_OF_ARRAY_VALUE; \
    }    
    
//=========================================================================
const int simd_search_mode = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_NEGATIVE_POLARITY | _SIDD_BIT_MASK;
__m128i chars_to_search_for_ws;
int ws_search_result;            

void setStructField(mxArray *s, void *pr, const char *fieldname, mxClassID classid, mwSize N)
{
    
    //This function is used to set a field in the output struct
        
    mxArray *pm;
    
    pm = mxCreateNumericArray(0, 0, classid, mxREAL);
    mxSetData(pm, pr);
    mxSetM(pm, 1);
    mxSetN(pm, N);
    mxAddField(s,fieldname);
    mxSetField(s,0,fieldname,pm);
}

void setIntScalar(mxArray *s, const char *fieldname, int value){

    //This function allows us to hold onto integer scalars
    //We need to make an allocation to grab a value off the stack
    
    mxArray *pm;
    
    int *temp_value = mxMalloc(sizeof(double));
    
    *temp_value = value;
    
    pm = mxCreateNumericArray(0, 0, mxINT32_CLASS, mxREAL);
    mxSetData(pm, temp_value);
    mxSetM(pm, 1);
    mxSetN(pm, 1);
    mxAddField(s,fieldname);
    mxSetField(s,0,fieldname,pm);    
    
}

//-------------------------------------------------------------------------
void string_to_double_no_math(unsigned char *p, unsigned char **char_offset) {

    //In this approach we look for math like characters. We parser for
    //validity at a later point in time.
    
    const __m128i digit_characters = _mm_set_epi8('0','1','2','3','4','5','6','7','8','9','.','-','+','e','E','0');
    
    __m128i chars_to_search_for_digits;
    
    int digit_search_result;
    
    chars_to_search_for_digits = _mm_loadu_si128((__m128i*)p);
    digit_search_result = _mm_cmpistri(digit_characters, chars_to_search_for_digits, simd_search_mode);
    p += digit_search_result;
    if (digit_search_result == 16){
        chars_to_search_for_digits = _mm_loadu_si128((__m128i*)p);
        digit_search_result = _mm_cmpistri(digit_characters, chars_to_search_for_digits, simd_search_mode);
        p += digit_search_result;
        //At this point we've traversed 32 characters
        //This code is easily rewriteable if in reality we need more
        if (digit_search_result == 16){
        	mexErrMsgIdAndTxt("jsmn_mex:too_long_math", "too much info in digit parsing");
        }
    }
    
    *char_offset = p;    
}

//-------------------------------------------------------------------------
void seek_string_end(unsigned char *p, unsigned char **char_offset){

STRING_SEEK:    
    //p+1 to advance past initial '"'
    p = strchr(p+1,'"');
    
    //Back up to verify 
    
    if (*(--p) == '\\'){
        mexErrMsgIdAndTxt("jsmn_mex:unhandled_case", "Code not yet written");
    }else{
        *char_offset = p+1;
    } 
}

//=========================================================================
//              Parse JSON   -    Parse JSON    -    Parse JSON
//=========================================================================
void jsmn_parse(unsigned char *js, size_t string_byte_length, mxArray *plhs[]) {
    
    INIT_LOCAL_WS_CHARS;
    
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
    unsigned char *temp_p;
    
    //---------------------------------------------------------------------
    const int MAX_DEPTH = 200;
    int parent_types[201];
    //Note, this needs to be indices instead of pointers because
    //we might resize (resize types, d1, d2) and the pointers would become 
    //invalid
    int parent_indices[201];
    int parent_sizes[201];
    int current_parent_index;
    int current_depth = 0;
    //---------------------------------------------------------------------
    int data_size_allocated = ceil(string_byte_length/4);
    int data_size_index_max = data_size_allocated - 1;
    int current_data_index = -1;
    
    uint8_t *types = mxMalloc(data_size_allocated);
    uint8_t *types_move = types;
            
    //do we need to make these uint64?
    //technically not, the start pointer is an index
    //and not an address
    //
    //d1 - n_values and start pointer index
    //d2 - tac
    int *d1 = mxMalloc(data_size_allocated * sizeof(int));
    int *d2 = mxMalloc(data_size_allocated * sizeof(int));
    //---------------------------------------------------------------------
    
    int n_key_chars = 0;
    int n_key_allocations  = 1;
    int key_size_allocated = ceil(string_byte_length/20);
    int key_size_index_max = key_size_allocated-1;
    int current_key_index = -1;
    ALLOCATE_KEY_DATA 
    
    int n_string_chars = 0;
    int n_string_allocations = 1;
    int string_size_allocated = ceil(string_byte_length/20);
    int string_size_index_max = string_size_allocated-1;
    int current_string_index = -1;
    ALLOCATE_STRING_DATA
   
    int n_numeric_allocations = 1;
    int numeric_size_allocated = ceil(string_byte_length/4);
    int numeric_size_index_max = numeric_size_allocated - 1;
    int current_numeric_index = -1;
    ALLOCATE_NUMERIC_DATA
    
    
//Start of the parsing ====================================================
    DECREMENT_POINTER;
	ADVANCE_TO_NON_WHITESPACE_CHAR;

	switch (CURRENT_CHAR) {
        case '{':
        	PROCESS_OPENING_OBJECT;
            NAVIGATE_AFTER_OPENING_OBJECT;
        case '[':
            PROCESS_OPENING_ARRAY;
            NAVIGATE_AFTER_OPENING_ARRAY_OR_AFTER_COMMA_IN_ARRAY;
        default:
            mexErrMsgIdAndTxt("jsmn_mex:invalid_start", "Starting token needs to be an opening object or array");
	}

//    [ {            ======================================================
S_OPEN_OBJECT_IN_ARRAY:
    
    
    INCREMENT_PARENT_SIZE;
    PROCESS_OPENING_OBJECT;
    NAVIGATE_AFTER_OPENING_OBJECT;
    

//   "key": {        ====================================================== 
S_OPEN_OBJECT_IN_KEY:
    
    INITIALIZE_PARENT_INFO(TYPE_KEY);
    PROCESS_OPENING_OBJECT;
    NAVIGATE_AFTER_OPENING_OBJECT;
  
//=============================================================
S_CLOSE_KEY_COMPLEX_AND_OBJECT:    
    //Update tac and parent
        
    current_parent_index = parent_indices[current_depth];    
    STORE_TAC_KEY_COMPLEX;
    MOVE_UP_PARENT_INDEX;

    //Fall Through ------
S_CLOSE_OBJECT:
    
    current_parent_index = parent_indices[current_depth];
    STORE_TAC_OF_OBJECT_OR_ARRAY;
    STORE_SIZE;
    MOVE_UP_PARENT_INDEX;
    
    NAVIGATE_AFTER_CLOSING_COMPLEX
    
//=============================================================
S_OPEN_ARRAY_IN_ARRAY:
	
    //mexPrintf("Opening array in array: %d\n",CURRENT_INDEX);
    
    INCREMENT_PARENT_SIZE;
    PROCESS_OPENING_ARRAY;   
    NAVIGATE_AFTER_OPENING_ARRAY_OR_AFTER_COMMA_IN_ARRAY;
    
//=============================================================
S_OPEN_ARRAY_IN_KEY:
        
    //mexPrintf("Opening array in key: %d\n",CURRENT_INDEX);
    
    INITIALIZE_PARENT_INFO(TYPE_KEY);
    PROCESS_OPENING_ARRAY;
	NAVIGATE_AFTER_OPENING_ARRAY_OR_AFTER_COMMA_IN_ARRAY;
            
//=============================================================
S_CLOSE_ARRAY:
    
    current_parent_index = parent_indices[current_depth];
    STORE_TAC_OF_OBJECT_OR_ARRAY;
    STORE_SIZE;
    MOVE_UP_PARENT_INDEX;
    
    NAVIGATE_AFTER_CLOSING_COMPLEX;

//=============================================================
S_PARSE_KEY:
    
    //mexPrintf("Parsing a key: %d\n",CURRENT_INDEX);
    
	INCREMENT_PARENT_SIZE;
    
    PROCESS_KEY;
    
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

    STORE_TAC_KEY_SIMPLE;
    PROCESS_STRING;
	PROCESS_END_OF_KEY_VALUE_SIMPLE


//=============================================================
S_PARSE_NUMBER_IN_KEY:
    
//     if (CURRENT_INDEX < 1000){
//         mexPrintf("Parse # in key 1: %d\n",CURRENT_INDEX);
//         mexPrintf("Current parent: %d\n",CURRENT_INDEX);
//     }
    
    STORE_TAC_KEY_SIMPLE;
    
//     if (CURRENT_INDEX < 1000){
//         mexPrintf("Parse # in key 2: %d\n",CURRENT_INDEX);
//     }
    
    PROCESS_NUMBER;
    
    DECREMENT_POINTER;
	PROCESS_END_OF_KEY_VALUE_SIMPLE;

//=============================================================
S_PARSE_NUMBER_IN_ARRAY:
    
	INCREMENT_PARENT_SIZE;
    PROCESS_NUMBER;
   
    //This normally happens, trying to optimize progression of #s in array
    if (CURRENT_CHAR == ','){
        ADVANCE_TO_NON_WHITESPACE_CHAR;
        DO_ARRAY_JUMP;
    }else{
        DECREMENT_POINTER;
        PROCESS_END_OF_ARRAY_VALUE;
    }

//=============================================================
S_PARSE_NULL_IN_KEY:
    
    STORE_TAC_KEY_SIMPLE;

    PROCESS_NULL;
    
	PROCESS_END_OF_KEY_VALUE_SIMPLE;

//=============================================================
S_PARSE_NULL_IN_ARRAY:

	INCREMENT_PARENT_SIZE;
    
    PROCESS_NULL
    
	PROCESS_END_OF_ARRAY_VALUE;

//=============================================================
S_PARSE_TRUE_IN_KEY:
    
    STORE_TAC_KEY_SIMPLE;
    
    PROCESS_TRUE
    
	PROCESS_END_OF_KEY_VALUE_SIMPLE


S_PARSE_TRUE_IN_ARRAY:
    
	INCREMENT_PARENT_SIZE;
    
    PROCESS_TRUE;
    
    PROCESS_END_OF_ARRAY_VALUE;

    
S_PARSE_FALSE_IN_KEY:
    
    STORE_TAC_KEY_SIMPLE;
    
    PROCESS_FALSE
    
    PROCESS_END_OF_KEY_VALUE_SIMPLE;

S_PARSE_FALSE_IN_ARRAY:
    
	INCREMENT_PARENT_SIZE;
    
    PROCESS_FALSE
    
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
  
S_ERROR_BAD_TOKEN_FOLLOWING_OBJECT_VALUE_COMMA:
    // {"key": value, #ERROR
    //  e.g.
    // {"key": value, 1
    //
	mexPrintf("Position %d\n",CURRENT_INDEX); \
	mexErrMsgIdAndTxt("jsmn_mex:no_key", "Key or closing of object expected"); \
    
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

S_ERROR_DEPTH:
    mexErrMsgIdAndTxt("jsmn_mex:depth_limit","Max depth exceeded");
    
finish_main:
    
    types = mxRealloc(types,(current_data_index + 1));
    setStructField(plhs[0],types,"types",mxUINT8_CLASS,current_data_index + 1);
    
    d1 = mxRealloc(d1,((current_data_index + 1)*sizeof(int)));
    setStructField(plhs[0],d1,"d1",mxINT32_CLASS,current_data_index + 1);
    
    d2 = mxRealloc(d2,((current_data_index + 1)*sizeof(int)));
    setStructField(plhs[0],d2,"d2",mxINT32_CLASS,current_data_index + 1);
     
    setIntScalar(plhs[0],"n_key_allocations",n_key_allocations);
    setIntScalar(plhs[0],"n_key_chars",n_key_chars);
    setIntScalar(plhs[0],"n_string_chars",n_string_chars);
    setIntScalar(plhs[0],"n_string_allocations",n_string_allocations);
    setIntScalar(plhs[0],"n_numeric_allocations",n_numeric_allocations);
    
    
    //TODO: This is only correct on 64 bit systems ...
    TRUNCATE_KEY_DATA
    setStructField(plhs[0],key_p,"key_p",mxUINT64_CLASS,current_key_index + 1);
    setStructField(plhs[0],key_end_indices,"key_end_indices",mxINT32_CLASS,current_key_index + 1);
    setStructField(plhs[0],key_start_indices,"key_start_indices",mxINT32_CLASS,current_key_index + 1);
    
    TRUNCATE_STRING_DATA
    setStructField(plhs[0],string_p,"string_p",mxUINT64_CLASS,current_string_index + 1);
    setStructField(plhs[0],string_end_indices,"string_end_indices",mxINT32_CLASS,current_string_index + 1);
    setStructField(plhs[0],string_start_indices,"string_start_indices",mxINT32_CLASS,current_string_index + 1);
    
    TRUNCATE_NUMERIC_DATA
    //Note, it seems the class type may only be needed for viewing in Matlab
    //Internally it is just bytes (assuming sizeof is the same)
    setStructField(plhs[0],numeric_p,"numeric_p",mxDOUBLE_CLASS,current_numeric_index + 1);


	return;
    
}