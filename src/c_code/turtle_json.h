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



#define STORE_INDEX(x) \
    /* + 1 for Matlab indexing */ \
    d1[current_data_index] = x + 1;

#define RETRIEVE_DATA_INDEX(x) \
    /* -1 since we are undoing the Matlab indexing */ \
    d1[x]-1

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

// #define TIC(x) \
//     clock_t x; \
//     x = clock();
//             
//The toc_and_log() macro saves the timing to a field with the name 
//specified by y
//#y => y as a string, not as a value "stringification"
// #define TOC_AND_LOG(x,y) \
//     double *y = mxMalloc(sizeof(double)); \
//     *y = (double)(clock() - x)/(double)(CLOCKS_PER_SEC); \
//     setStructField(plhs[0],y,#y,mxDOUBLE_CLASS,1);
    
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
    
#define TOC_AND_LOG(x,y) \
    gettimeofday(&x##_1,NULL); \
    double *y = mxMalloc(sizeof(double)); \
    *y = (double)(x##_1.tv_sec - x##_0.tv_sec) + (double)(x##_1.tv_usec - x##_0.tv_usec)/1e6; \
    setStructField(plhs[0],y,#y,mxDOUBLE_CLASS,1);      
    
    
    
//void addParseBuffer(unsigned char **p_buffer, size_t array_length)
    
//void processInputBytes(const mxArray *prhs[], unsigned char **p_buffer, size_t *string_byte_length)    
    
void setIntScalar(mxArray *s, const char *fieldname, int value);    
    
void setStructField(mxArray *s, void *pr, const char *fieldname, mxClassID classid, mwSize N);

void *get_field(mxArray *plhs[],const char *fieldname);

mwSize get_field_length(mxArray *plhs[],const char *fieldname);

void populate_object_flags(unsigned char *js,mxArray *plhs[]);

void populate_array_flags(unsigned char *js,mxArray *plhs[]);

void parse_char_data(unsigned char *js,mxArray *plhs[], bool is_key);

//void parse_keys(unsigned char *js,mxArray *plhs[]);    
    
//void parse_strings(unsigned char *js,mxArray *plhs[]);
    
void parse_numbers(unsigned char *js, mxArray *plhs[]);

void parse_json(unsigned char *js, size_t len, mxArray *plhs[], Options *options);