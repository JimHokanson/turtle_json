#include "jsmn.h"
#include "stdio.h"
#include <stdlib.h>
#include <ctype.h>
#include "mex.h"
#include <math.h>
#include "stdint.h"  //uint_8


//TODO: replace with a goto for more information
#define ERROR_DEPTH mexErrMsgIdAndTxt("jsmn_mex:depth_limit","Max depth exceeded");

#define N_INT_PART  data_info[0]
#define N_FRAC_PART data_info[1]
#define N_EXP_PART  data_info[2]

//TODO: Document
#define EXPAND_DATA_CHECK(x) \
	if (current_type_index+x >= data_size_index_max){ \
        data_size_allocated = ceil(1.5*data_size_allocated); \
        types = mxRealloc(types,type_size_allocated*sizeof(int)); \
        data_size_index_max = data_size_allocated-1; \
    } \

//TODO: Document
#define SET_TYPE(x) \
	if (current_type_index >= type_size_index_max){ \
        type_size_allocated = ceil(1.5*type_size_allocated); \
        types = mxRealloc(types,type_size_allocated); \
        type_size_index_max = type_size_allocated-1; \
    } \
	types[++current_type_index] = x; \
      
//Sets the token after close information for the object, array, or key
//that is being closed
#define SET_TAC data[parent_indices[current_depth]] = current_data_index+1;
//Sets the # of values that 
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

//same as above but with no size
#define SETUP_PARENT_INFO_FOR_KEY \
	if (current_depth == MAX_DEPTH) { \
		ERROR_DEPTH \
	} \
	else { \
		++current_depth; \
		parent_types[current_depth] = TYPE_KEY; \
		parent_indices[current_depth] = (++current_data_index); \
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
            SET_TAC; \
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



const bool is_whitespace[256] = { false,false,false,false,false,false,false,false,false,true,true,false,false,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };

//Values for Integer portion of number
//------------------------------------
//Note that I'm avoiding subtracting 0 which makes these arrays rather large ...
const double p1e0[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9};
const double p1e1[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,20,30,40,50,60,70,80,90};
const double p1e2[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,200,300,400,500,600,700,800,900};
const double p1e3[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000,2000,3000,4000,5000,6000,7000,8000,9000};
const double p1e4[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10000,20000,30000,40000,50000,60000,70000,80000,90000};
const double p1e5[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100000,200000,300000,400000,500000,600000,700000,800000,900000};
const double p1e6[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000000,2000000,3000000,4000000,5000000,6000000,7000000,8000000,9000000};
const double p1e7[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10000000,20000000,30000000,40000000,50000000,60000000,70000000,80000000,90000000};
const double p1e8[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100000000,200000000,300000000,400000000,500000000,600000000,700000000,800000000,900000000};
const double p1e9[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000000000,2000000000,3000000000,4000000000,5000000000,6000000000,7000000000,8000000000,9000000000};
const double p1e10[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10000000000,20000000000,30000000000,40000000000,50000000000,60000000000,70000000000,80000000000,90000000000};
const double p1e11[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100000000000,200000000000,300000000000,400000000000,500000000000,600000000000,700000000000,800000000000,900000000000};
const double p1e12[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000000000000,2000000000000,3000000000000,4000000000000,5000000000000,6000000000000,7000000000000,8000000000000,9000000000000};
const double p1e13[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10000000000000,20000000000000,30000000000000,40000000000000,50000000000000,60000000000000,70000000000000,80000000000000,90000000000000};
const double p1e14[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100000000000000,200000000000000,300000000000000,400000000000000,500000000000000,600000000000000,700000000000000,800000000000000,900000000000000};
const double p1e15[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000000000000000,2000000000000000,3000000000000000,4000000000000000,5000000000000000,6000000000000000,7000000000000000,8000000000000000,9000000000000000};

//Values for fractional component
//-------------------------------
//Note, it is unclear how accurate we can really be with this ...
//and accordingly, how far out we should really populate ...
//
//I've gone out to the maximum of double (I think)
//
//i.e. 2.0123456789012345 ?
const double p1e_1[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
const double p1e_2[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09};
const double p1e_3[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.001,0.002,0.003,0.004,0.005,0.006,0.007,0.008,0.009};
const double p1e_4[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0001,0.0002,0.0003,0.0004,0.0005,0.0006,0.0007,0.0008,0.0009};
const double p1e_5[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00001,0.00002,0.00003,0.00004,0.00005,0.00006,0.00007,0.00008,0.00009};
const double p1e_6[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000001,0.000002,0.000003,0.000004,0.000005,0.000006,0.000007,0.000008,0.000009};
const double p1e_7[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000001,0.0000002,0.0000003,0.0000004,0.0000005,0.0000006,0.0000007,0.0000008,0.0000009};
const double p1e_8[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000001,0.00000002,0.00000003,0.00000004,0.00000005,0.00000006,0.00000007,0.00000008,0.00000009};
const double p1e_9[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000001,0.000000002,0.000000003,0.000000004,0.000000005,0.000000006,0.000000007,0.000000008,0.000000009};
const double p1e_10[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000001,0.0000000002,0.0000000003,0.0000000004,0.0000000005,0.0000000006,0.0000000007,0.0000000008,0.0000000009};
const double p1e_11[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000001,0.00000000002,0.00000000003,0.00000000004,0.00000000005,0.00000000006,0.00000000007,0.00000000008,0.00000000009};
const double p1e_12[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000000001,0.000000000002,0.000000000003,0.000000000004,0.000000000005,0.000000000006,0.000000000007,0.000000000008,0.000000000009};
const double p1e_13[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000000001,0.0000000000002,0.0000000000003,0.0000000000004,0.0000000000005,0.0000000000006,0.0000000000007,0.0000000000008,0.0000000000009};
const double p1e_14[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000000001,0.00000000000002,0.00000000000003,0.00000000000004,0.00000000000005,0.00000000000006,0.00000000000007,0.00000000000008,0.00000000000009};
const double p1e_15[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000000000001,0.000000000000002,0.000000000000003,0.000000000000004,0.000000000000005,0.000000000000006,0.000000000000007,0.000000000000008,0.000000000000009};
const double p1e_16[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000000000001,0.0000000000000002,0.0000000000000003,0.0000000000000004,0.0000000000000005,0.0000000000000006,0.0000000000000007,0.0000000000000008,0.0000000000000009};
const double p1e_17[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000000000001,0.00000000000000002,0.00000000000000003,0.00000000000000004,0.00000000000000005,0.00000000000000006,0.00000000000000007,0.00000000000000008,0.00000000000000009};
const double p1e_18[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000000000000001,0.000000000000000002,0.000000000000000003,0.000000000000000004,0.000000000000000005,0.000000000000000006,0.000000000000000007,0.000000000000000008,0.000000000000000009};
const double p1e_19[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000000000000001,0.0000000000000000002,0.0000000000000000003,0.0000000000000000004,0.0000000000000000005,0.0000000000000000006,0.0000000000000000007,0.0000000000000000008,0.0000000000000000009};
const double p1e_20[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000000000000001,0.00000000000000000002,0.00000000000000000003,0.00000000000000000004,0.00000000000000000005,0.00000000000000000006,0.00000000000000000007,0.00000000000000000008,0.00000000000000000009};

//*p++                  js[parser_position++]    
//*p   ==========       js[parser_position]
//++p  ==========       ++parser_position

inline double string_to_double(unsigned char *p, int num_info) {

    double value = 0;
    bool negate;
    uint8_t n_int  = (uint8_t)num_info;
    uint8_t n_frac = num_info >> 8;
    uint8_t n_exp  = num_info >> 16;
    
//     uint8_t *info2 = (uint8_t*)&num_info;
    
    if (*p == '-'){
        ++p;
        negate = true;    
    }else{
        negate = false;
    }

    //I'm curious if you could take advantage of something like SSE
    //if each case was written out explicitly rather than via fall through
    switch (n_int) {
        case 16:
           value += p1e15[*p++];  
        case 15:
           value += p1e14[*p++];  
        case 14:
           value += p1e13[*p++];   
        case 13:
           value += p1e12[*p++];   
        case 12:
           value += p1e11[*p++];   
        case 11:
           value += p1e10[*p++];       
        case 10:
           value += p1e9[*p++];  
        case 9:
           value += p1e8[*p++];  
        case 8:
           value += p1e7[*p++];  
        case 7:
           value += p1e6[*p++]; 
        case 6:
           value += p1e5[*p++];
        case 5:
           value += p1e4[*p++];
        case 4:
           value += p1e3[*p++];
        case 3:
           value += p1e2[*p++];
        case 2:
           value += p1e1[*p++]; //1e1 == 10, 2 #s plus the off by 1
        case 1:
           value += p1e0[*p++]; //1e0 == 1, an unfortunate mismatch of exponent and scalar
    }  
    
    //This would be written neater as a while loop that updates a pointer
    //to the various arrays but I don't think it would be as fast
    //
    //Unlike the integer part, we know how to interpret each
    //numeric value as we are parsing it
    //
    //0.1234 =
    //0.1 +
    //0.02 +
    //0.003 +
    //0.0004
    
    //TODO: Implement fraction next
    
//     if (*p == '.') {
//         ++p;   
//         //TODO: Is no digit ok?
//         if(is_number_array[*p]){
//             value += p1e_1[*p++];
//             if(is_number_array[*p]){
//                 value += p1e_2[*p++];
//                 if(is_number_array[*p]){
//                     value += p1e_3[*p++];
//                     if(is_number_array[*p]){
//                         value += p1e_4[*p++];
//                         if(is_number_array[*p]){
//                             value += p1e_5[*p++];
//                             if(is_number_array[*p]){
//                                 value += p1e_6[*p++];
//                                 if(is_number_array[*p]){
//                                     value += p1e_7[*p++];
//                                     if(is_number_array[*p]){
//                                         value += p1e_8[*p++];
//                                         if(is_number_array[*p]){
//                                             value += p1e_9[*p++];
//                                             if(is_number_array[*p]){
//                                                 value += p1e_10[*p++];
//                                                 if(is_number_array[*p]){
//                                                     value += p1e_11[*p++];
//                                                     if(is_number_array[*p]){
//                                                         value += p1e_12[*p++];
//                                                         if(is_number_array[*p]){
//                                                             value += p1e_13[*p++];
//                                                             if(is_number_array[*p]){
//                                                                 value += p1e_14[*p++];
//                                                                 if(is_number_array[*p]){
//                                                                     value += p1e_15[*p++];
//                                                                     if(is_number_array[*p]){
//                                                                         value += p1e_16[*p++];
//                                                                         if(is_number_array[*p]){
//                                                                             value += p1e_17[*p++];
//                                                                             if(is_number_array[*p]){
//                                                                                 value += p1e_18[*p++];
//                                                                                 if(is_number_array[*p]){
//                                                                                     value += p1e_19[*p++];
//                                                                                     if(is_number_array[*p]){
//                                                                                         value += p1e_20[*p++];
//                                                                                         if(is_number_array[*p]){
//                                                                                             mexErrMsgIdAndTxt("jsmn_mex:too_many_decimals","The fractional component of the number had too many digits");
// 
//                                                                                         }
//                                                                                     }
//                                                                                 }
//                                                                             }
//                                                                         }
//                                                                     }
//                                                                 }
//                                                             }
//                                                         }
//                                                     }
//                                                 }
//                                             }
//                                         }
//                                     }
//                                 }
//                             }
//                         }
//                     }
//                 }
//             }
//         }
//     }
    //End of if '.'    
    if (negate) {
        value = -value;
    }

    
    
    
    return value;
    
    /*
     *  I found atof to be the main bottleneck in this code. It was 
     *  suggested to me by some internet searching that a custom algorithm
     *  would be close enough in 99.99 of the cases. I found a couple
     *  basic ones online that used loops and multiplation. This was my
     *  attempt to make something as fast possible. I welcome a faster
     *  approach!
     */
    
// // // // // //     double value = 0;
// // // // // //     double exponent_value;
// // // // // //     
// // // // // //     int64_t n_numeric_chars_plus_1;
// // // // // //     char *number_start;
// // // // // //     
// // // // // //     if (*p == '-'){
// // // // // //         ++p;
// // // // // //         negate = true;    
// // // // // //     }else{
// // // // // //         negate = false;
// // // // // //     }
// // // // // // 
// // // // // //     number_start = p;
// // // // // //     while (is_number_array[*p++]) {  
// // // // // //     }
// // // // // //     
// // // // // //     n_numeric_chars_plus_1 = p-number_start; //# of numeric characters (off by 1 due to ++)
// // // // // //     
// // // // // //     //reset p so that we can increment our way along the number
// // // // // //     p = number_start;
// // // // // //     //Now that we know how many #s we have (off by 1 due to ++)
// // // // // //     //we can start at the beginning and add each to its correct place
// // // // // //     //e.g:
// // // // // //     //  4086 =
// // // // // //     //  4000 +
// // // // // //     //   0   +
// // // // // //     //    80 +
// // // // // //     //     6
// // // // // //     
// // // // // //     
// // // // // //     if (*p == 'E' || *p == 'e') {
// // // // // //         ++p;
// // // // // //         switch (*p){
// // // // // //             case '-':
// // // // // //                 ++p;
// // // // // //                 negate = true;
// // // // // //                 break;
// // // // // //             case '+':
// // // // // //                 ++p;
// // // // // //             default:
// // // // // //                 negate = false;
// // // // // //         }
// // // // // //         
// // // // // //         number_start = p;
// // // // // //         while (is_number_array[*p++]) {  
// // // // // //         }
// // // // // // 
// // // // // //         exponent_value = 0;
// // // // // //         n_numeric_chars_plus_1 = p-number_start;
// // // // // //         //reset p so that we can increment our way down
// // // // // //         p = number_start;
// // // // // //         switch (n_numeric_chars_plus_1) {
// // // // // //             case 17:
// // // // // //                exponent_value += p1e15[*p++];  
// // // // // //             case 16:
// // // // // //                exponent_value += p1e14[*p++];  
// // // // // //             case 15:
// // // // // //                exponent_value += p1e13[*p++];   
// // // // // //             case 14:
// // // // // //                exponent_value += p1e12[*p++];   
// // // // // //             case 13:
// // // // // //                exponent_value += p1e11[*p++];   
// // // // // //             case 12:
// // // // // //                exponent_value += p1e10[*p++];       
// // // // // //             case 11:
// // // // // //                exponent_value += p1e9[*p++];  
// // // // // //             case 10:
// // // // // //                exponent_value += p1e8[*p++];  
// // // // // //             case 9:
// // // // // //                exponent_value += p1e7[*p++];  
// // // // // //             case 8:
// // // // // //                exponent_value += p1e6[*p++]; 
// // // // // //             case 7:
// // // // // //                exponent_value += p1e5[*p++];
// // // // // //             case 6:
// // // // // //                exponent_value += p1e4[*p++];
// // // // // //             case 5:
// // // // // //                exponent_value += p1e3[*p++];
// // // // // //             case 4:
// // // // // //                exponent_value += p1e2[*p++];
// // // // // //             case 3:
// // // // // //                exponent_value += p1e1[*p++];
// // // // // //             case 2:
// // // // // //                exponent_value += p1e0[*p++];
// // // // // //                break;
// // // // // //             case 1:
// // // // // //                 mexErrMsgIdAndTxt("jsmn_mex:empty_exponent","An exponent was given with no numeric value");
// // // // // //             default:
// // // // // //                 //TODO: Give error location in string
// // // // // //                 mexErrMsgIdAndTxt("jsmn_mex:large_exponent","There were more than 15 digits in a numeric exponent");
// // // // // //         }
// // // // // //         if (negate){
// // // // // //           exponent_value = -exponent_value;  
// // // // // //         }
// // // // // //         value *= pow(10.0, exponent_value);
// // // // // //     }
// // // // // //     
// // // // // //     *char_offset = p;
// // // // // //     
// // // // // //     return value;
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

//-------------------------------------------------------------------------
//--------------------  End of Number Parsing  ----------------------------
//-------------------------------------------------------------------------
inline int string_to_double_no_math(unsigned char *p, unsigned char **char_offset) {

    //An alternative approach would be to look for the closing string
    //e.g.: ',' '}' ']' and then check all of the math when running
    //in parallel
    
    //We're point at a negative or a digit
    
    //I'm not necessarily thrilled with this approach
    //TODO: test the alternative code with loops
    //and checks for overflow and subtraction for populating the data
    
    
    uint8_t data_info[4] = {0,0,0,0};
        
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
void jsmn_parse(unsigned char *js, size_t string_byte_length, mxArray *plhs[]) {

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
    
	const int MAX_DEPTH = 200;

    int n_numeric = 0;
    int n_keys = 0;
    int n_strings = 0;
    
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
    
    ++n_keys;
    
    SET_TYPE(TYPE_KEY);
    
    SETUP_PARENT_INFO_FOR_KEY;
    
//     if (current_depth == 2 && current_data_index < 2000){
//         mexPrintf("current_data_index %d\n",current_data_index);
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

    ++n_strings;
    
    SET_TYPE(TYPE_STRING);

    EXPAND_DATA_CHECK(N_DATA_STRING)
	data[++current_data_index] = parser_position;
	seek_string_end(js, &parser_position);
    data[++current_data_index] = parser_position;

	PROCESS_END_OF_ARRAY_VALUE;



	//=============================================================
S_PARSE_STRING_IN_KEY:

    ++n_strings;
    
	SET_TYPE(TYPE_STRING);

    EXPAND_DATA_CHECK(N_DATA_STRING)
	data[++current_data_index] = parser_position;
	seek_string_end(js, &parser_position);
    data[++current_data_index] = parser_position;

	PROCESS_END_OF_KEY_VALUE



	//=============================================================
S_PARSE_NUMBER_IN_KEY:

    ++n_numeric;
    
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

    ++n_numeric;
    
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

    ++n_numeric;
    
	SET_TYPE(TYPE_NULL);

	EXPAND_DATA_CHECK(N_DATA_NULL)

    data[++current_data_index] = MX_NAN;
    
	parser_position += 3;
    
	PROCESS_END_OF_KEY_VALUE



//=============================================================
S_PARSE_NULL_IN_ARRAY:

	parent_sizes[current_depth] += 1;
    
    ++n_numeric;
    
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
    
    //We free up memory
    types = mxRealloc(types,(current_type_index + 1));
    setStructField(plhs[0],types,"types",mxUINT8_CLASS,current_type_index + 1);
    
    data = mxRealloc(data,(current_type_index + 1)*sizeof(int));
	setStructField(plhs[0],data,"data",mxINT32_CLASS,current_data_index + 1);
    
    //Holding onto values
    int *p_type_size_allocated = mxMalloc(sizeof(int));
    *p_type_size_allocated = type_size_allocated;
    int *p_data_size_allocated = mxMalloc(sizeof(int));
    *p_data_size_allocated = data_size_allocated;
    int *p_n_numeric = mxMalloc(sizeof(int));
    *p_n_numeric = n_numeric;
    int *p_n_keys = mxMalloc(sizeof(int));
    *p_n_keys = n_keys;
    int *p_n_strings = mxMalloc(sizeof(int));
    *p_n_strings= n_strings;

    setStructField(plhs[0],p_type_size_allocated,"type_size_allocated",mxINT32_CLASS,1);
    setStructField(plhs[0],p_data_size_allocated,"data_size_allocated",mxINT32_CLASS,1);
    setStructField(plhs[0],p_n_numeric,"n_numeric",mxINT32_CLASS,1);
    setStructField(plhs[0],p_n_keys,"n_keys",mxINT32_CLASS,1);
    setStructField(plhs[0],p_n_strings,"n_strings",mxINT32_CLASS,1);
    
    int cur_string = -1;
    int cur_key = -1;
    int cur_data_index = -1;
    
    double *numeric_data = mxMalloc(n_numeric*sizeof(double));
    double *moving_numeric_data = numeric_data;

    unsigned char *char_start;
    int num_info;
    
    //Eventually I'd like this part to be multi-threaded
    for(int iKey = 0; iKey <= current_type_index; iKey++){
        switch(types[iKey]){
            case TYPE_OBJECT:
                cur_data_index += 2;
                break;
            case TYPE_ARRAY:
                cur_data_index += 2;
                break;
            case TYPE_STRING:
                cur_data_index += 2;
                break;
            case TYPE_NUMBER:
                char_start = js + data[++cur_data_index];
                num_info = data[++cur_data_index];
                *moving_numeric_data = string_to_double(char_start,num_info);
                ++moving_numeric_data;
                break;
            case TYPE_LOGICAL:
                cur_data_index += 1;
                break;
            case TYPE_KEY:
                cur_data_index += 3;
                break;
            case TYPE_NULL:
                cur_data_index += 1;
                break;
        }
             
    }
    
    setStructField(plhs[0],numeric_data,"numeric_data",mxDOUBLE_CLASS,n_numeric);        

    
    //=====================================================================
    
    
    
    
    
// // //     setStructField(plhs[0],values,"values",mxDOUBLE_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],types,"types",mxUINT8_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],sizes,"sizes",mxINT32_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],parents,"parents",mxINT32_CLASS,n_tokens_used);    
    
    
    
    
    
	return;

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



