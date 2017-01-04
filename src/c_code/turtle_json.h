#include "stdio.h" //fopen_s?
#include <stdlib.h>
#include <ctype.h>
#include "mex.h"        //Matlab mex
#include <math.h>
#include "stdint.h"     //uint_8
#include <string.h>     //strchr()
#include <time.h>       //clock()
#include <sys/time.h>   //for gettimeofday
#include <omp.h>        //openmp
#include "turtle_json_memory.h"

//AVX
//#include "immintrin.h"

//TODO: Need to build in checks for support 
//SSE4.2
#include "nmmintrin.h"

#define TYPE_OBJECT 1
#define TYPE_ARRAY  2
#define TYPE_KEY    3
#define TYPE_STRING 4
#define TYPE_NUMBER 5
#define TYPE_NULL   6
#define TYPE_TRUE   7
#define TYPE_FALSE  8

#define ARRAY_OTHER_TYPE   0
#define ARRAY_NUMERIC_TYPE 1
#define ARRAY_STRING_TYPE  2
#define ARRAY_LOGICAL_TYPE 3
#define ARRAY_OBJECT_SAME_TYPE  4
#define ARRAY_OBJECT_DIFF_TYPE 5
#define ARRAY_ND_NUMERIC 6
#define ARRAY_ND_STRING 7
#define ARRAY_ND_LOGICAL 8

typedef struct {
   bool has_raw_string;
   bool has_raw_bytes;
   bool parse_strings; //Not yet implemented
   int n_tokens;
   int n_keys;
   int n_strings;
   int n_numbers;
   int chars_per_token;
} Options;

//http://stackoverflow.com/questions/18847833/is-it-possible-return-cell-array-that-contains-one-instance-in-several-cells
struct mxArray_Tag_Partial {
    void *name_or_CrossLinkReverse;
    mxClassID ClassID;
    int VariableType;
    mxArray *CrossLink;
    size_t ndim;
    unsigned int RefCount; /* Number of sub-elements identical to this one */
};

extern mxArray *mxCreateSharedDataCopy(const mxArray *pr);

#define STORE_DATA_INDEX(x) d1[current_data_index] = x;

#define RETRIEVE_DATA_INDEX(x) d1[x]

//TODO: I'd like to rename cur_key__key_index to cur_key_data_index
#define NEXT_KEY__KEY_INDEX(cur_key__key_index) \
        RETRIEVE_DATA_INDEX(next_sibling_index_key[cur_key__key_index])
/*
 *
 *  Example Usage:
 *  TIC(start_parse)
 *  //run parsing code
 *  TOC(start_parse,parsing_time)
 *
 *
 */

//http://stackoverflow.com/questions/10673732/openmp-time-and-clock-calculates-two-different-results
//These two MACROS are meant to be used like TIC and TOC in Matlab
//These were added when I got an error declaring TIC(x) immediately
//after a label
#define DEFINE_TIC(x) \
    struct timeval x ## _0; \
    struct timeval x ## _1;
    
#define START_TIC(x) \
    gettimeofday(&x##_0,NULL);
    
//TODO: Make this call start and define    
#define TIC(x) \
    struct timeval x ## _0; \
    struct timeval x ## _1; \
    gettimeofday(&x##_0,NULL);
    
#define TOC(x,y) \
    gettimeofday(&x##_1,NULL); \
    double *y = mxMalloc(sizeof(double)); \
    *y = (double)(x##_1.tv_sec - x##_0.tv_sec) + (double)(x##_1.tv_usec - x##_0.tv_usec)/1e6; \
    setStructField(timing_info,y,#y,mxDOUBLE_CLASS,1);      
    
#define ADD_STRUCT_FIELD(name,pointer) \
    mxAddField(plhs[0],#name); \
    mxSetField(plhs[0],0,#name,pointer);    
    
//Number Parsing
//-------------------------------------------------------------------------
void string_to_double(double *value_p, char *p, int i, int *error_p, int *error_value);

void string_to_double_v2(double *value_p, char *p, int i, int *error_p, int *error_value);

//Main parsing
//-------------------------------------------------------------------------
void parse_json(unsigned char *js, size_t len, mxArray *plhs[], Options *options, mxArray *timing_info);

//Helpers
//-------------------------------------------------------------------------
mxArray *mxCreateReference(const mxArray *mx);

void setIntScalar(mxArray *s, const char *fieldname, int value);    
    
void setStructField(mxArray *s, void *pr, const char *fieldname, mxClassID classid, mwSize N);

void *get_field(mxArray *plhs[],const char *fieldname);

mwSize get_field_length(mxArray *plhs[],const char *fieldname);

uint8_t * get_u8_field(mxArray *p,const char *fieldname);

int *get_int_field(mxArray *p,const char *fieldname);

mwSize get_field_length2(mxArray *p,const char *fieldname);


//Post-processing related
//-------------------------------------------------------------------------
uint16_t parse_utf8_char(unsigned char **pp, unsigned char *p, int *parse_status);

void populateProcessingOrder(int *process_order, uint8_t *types, int n_entries, uint8_t type_to_match, int *n_values_at_depth, int n_depths, uint8_t *value_depths);

void post_process(unsigned char *js,mxArray *plhs[], mxArray *timing_info);

void populate_object_flags(unsigned char *js,mxArray *plhs[]);

void populate_array_flags(unsigned char *js,mxArray *plhs[]);

void parse_char_data(unsigned char *js,mxArray *plhs[], mxArray *timing_info);

void parse_key_chars(unsigned char *js,mxArray *plhs[]);
    
void parse_numbers(unsigned char *js, mxArray *plhs[]);

