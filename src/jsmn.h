#include "stdio.h" //fopen_s?
#include <stdlib.h>
#include <ctype.h>
#include "mex.h"
#include <math.h>
#include "stdint.h"  //uint_8
#include <string.h> //strchr
#include <time.h>   //required for clock
#include <omp.h>
//This is needed for AVX
//but, we might not use AVX - SSE4
//TODO: Need to build in SSE4 and AVX testing support
//with define flags ...

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

#define JSMN_ERROR_NOMEM -2

/*
 *
 *  Example Usage:
 *  TIC(start_parse)
 *  //run parsing code
 *  TOC(start_parse,parsing_time)
 *
 *
 */

//TODO: It would be nice to get higher resolution
#define TIC(x)\
    clock_t x;\
    x = clock();\
            
#define TOC_AND_LOG(x,y) \
    double *y = mxMalloc(sizeof(double)); \
    *y = (double)(clock() - x)/CLOCKS_PER_SEC; \
    setStructField(plhs[0],y,#y,mxDOUBLE_CLASS,1); \
    





void setStructField(mxArray *s, void *pr, const char *fieldname, mxClassID classid, mwSize N);

void parse_keys(unsigned char *js,mxArray *plhs[]);    
    
void parse_strings(unsigned char *js,mxArray *plhs[]);
    
void parse_numbers(unsigned char *js, mxArray *plhs[]);

void jsmn_parse(unsigned char *js, size_t len, mxArray *plhs[]);