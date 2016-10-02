#include "stdio.h" //fopen_s?
#include <stdlib.h>
#include <ctype.h>
#include "mex.h"        //Matlab mex
#include <math.h>
#include "stdint.h"     //uint_8
#include <string.h>     //strchr()
#include <time.h>       //clock()
#include <omp.h>        //openmp

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


typedef struct {
   bool has_raw_string;
   bool has_raw_bytes;
   int n_tokens;
   int chars_per_token;
} Options;

/*
 *
 *  Example Usage:
 *  TIC(start_parse)
 *  //run parsing code
 *  TOC(start_parse,parsing_time)
 *
 *
 */

//These two MACROS are meant to be used like TIC and TOC in Matlab

#define TIC(x)\
    clock_t x;\
    x = clock();
            
//The toc_and_log() macro saves the timing to a field with the name 
//specified by y
//#y => y as a string, not as a value "stringification"
#define TOC_AND_LOG(x,y) \
    double *y = mxMalloc(sizeof(double)); \
    *y = (double)(clock() - x)/CLOCKS_PER_SEC; \
    setStructField(plhs[0],y,#y,mxDOUBLE_CLASS,1);
    
//void addParseBuffer(unsigned char **p_buffer, size_t array_length)
    
//void processInputBytes(const mxArray *prhs[], unsigned char **p_buffer, size_t *string_byte_length)    
    
void setStructField(mxArray *s, void *pr, const char *fieldname, mxClassID classid, mwSize N);

void parse_keys(unsigned char *js,mxArray *plhs[]);    
    
void parse_strings(unsigned char *js,mxArray *plhs[]);
    
void parse_numbers(unsigned char *js, mxArray *plhs[]);

void parse_json(unsigned char *js, size_t len, mxArray *plhs[]);