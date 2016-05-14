#include "jsmn.h"
#include "stdio.h"
#include <stdlib.h>
#include <ctype.h>
#include "mex.h"
#include <math.h>
#include "stdint.h"  //uint_8
#include <time.h>
#include <omp.h>

//This is needed for AVX?
#include "immintrin.h"

//TODO: Can we substantially remove parent indexing
//- type
//- n_things
//- tac


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

//Things for opening ======================================================
#define EXPAND_DATA_CHECK(x) \
	if (current_data_index + x >= data_size_index_max){ \
        data_size_allocated = ceil(1.5*data_size_allocated); \
        data = mxRealloc(data,data_size_allocated*sizeof(int)); \
        data_size_index_max = data_size_allocated-1; \
    } \

#define SET_TYPE(x) data[++current_data_index] = x;

#define STORE_AND_UPDATE_PARENT \
    data[++current_data_index] = parent_index; \
    parent_index = current_data_index;  
    
#define INIT_N_THINGS data[++current_data_index] = n_things;
//=========================================================================
    
    
//Things for closing ======================================================
#define FINALIZE_N_THINGS 1 \
//     temp_n_things = n_things - data[parent_index+2]; \
//     n_things = data[parent_index+2]; \
//     data[parent_index+2] = temp_n_things; \
            
//+1 to next element
//+1 for Matlab 1 based indexing
#define STORE_TAC 1
    //data[parent_index+1] = current_data_index+2;
            
#define MOVE_UP_PARENT_INDEX parent_index = data[parent_index];	
            
#define NULL_PARENT_INDEX parent_index == 0     
            
#define PARENT_TYPE data[parent_index-1]            
//=========================================================================
            
            

//===================== Navigation ========================================
//=========================================================================      
#define DO_KEY_JUMP goto *key_jump[js[parser_position]];
     
#define DO_ARRAY_JUMP goto *array_jump[js[parser_position]];
            
// #define DO_KEY_JUMP \
//     switch(js[parser_position]){\
//         case '"':\
//             goto S_PARSE_STRING_IN_KEY;\
//         case '-':\
//         case '0':\
//         case '1':\
//         case '2':\
//         case '3':\
//         case '4':\
//         case '5':\
//         case '6':\
//         case '7':\
//         case '8':\
//         case '9':\
//             goto S_PARSE_NUMBER_IN_KEY;\
//         case '{':\
//             goto S_OPEN_OBJECT_IN_KEY;\
//         case '[':\
//             goto S_OPEN_ARRAY_IN_KEY;\
//         case 't':\
//             goto S_PARSE_TRUE_IN_KEY;\
//         case 'f':\
//             goto S_PARSE_FALSE_IN_KEY;\
//         case 'n':\
//             goto S_PARSE_NULL_IN_KEY;\
//         default:\
//             mexErrMsgIdAndTxt("jsmn_mex:asdfasdfsadf","Invalid token following colon after key declaration");\
//     } \
//     
        
// #define DO_ARRAY_JUMP \
//     switch(js[parser_position]){\
//         case '"':\
//             goto S_PARSE_STRING_IN_ARRAY;\
//         case '-':\
//         case '0':\
//         case '1':\
//         case '2':\
//         case '3':\
//         case '4':\
//         case '5':\
//         case '6':\
//         case '7':\
//         case '8':\
//         case '9':\
//             goto S_PARSE_NUMBER_IN_ARRAY;\
//         case '{':\
//             goto S_OPEN_OBJECT_IN_ARRAY;\
//         case '[':\
//             goto S_OPEN_ARRAY_IN_ARRAY;\
//         case 't':\
//             goto S_PARSE_TRUE_IN_ARRAY;\
//         case 'f':\
//             goto S_PARSE_FALSE_IN_ARRAY;\
//         case 'n':\
//             goto S_PARSE_NULL_IN_ARRAY;\
//         default:\
//             mexErrMsgIdAndTxt("jsmn_mex:asdfasdfsadf","Invalid token following comma in an array");\
//     }\
            

          
#define SKIP_WHITESPACE while (is_whitespace[js[++parser_position]]){}
            
#define PROCESS_END_OF_ARRAY_VALUE \
	SKIP_WHITESPACE; \
	switch (js[parser_position]) { \
        case ',': \
            SKIP_WHITESPACE; \
            DO_ARRAY_JUMP; \
        case ']': \
            goto S_CLOSE_ARRAY; \
        default: \
            goto S_ERROR_END_OF_VALUE_IN_ARRAY; \
	} \
             
#define PROCESS_END_OF_KEY_VALUE \
	SKIP_WHITESPACE; \
	switch (js[parser_position]) { \
        case ',': \
            STORE_TAC; \
            MOVE_UP_PARENT_INDEX; \
            SKIP_WHITESPACE; \
            if (js[parser_position] == '"') { \
                goto S_PARSE_KEY; \
            } \
            else { \
                mexPrintf("Position %d\n",parser_position); \
                mexErrMsgIdAndTxt("jsmn_mex:no_key", "Key expected"); \
            } \
        case '}': \
            goto S_CLOSE_KEY_AND_OBJECT; \
        default: \
            goto S_ERROR_END_OF_VALUE_IN_KEY; \
	} \

//=========================================================================

            


/* This is our thread function.  It is like main(), but for a thread */
void do_calculation(int *wtf)
{
    int j = 1;
    #pragma omp parallel num_threads(4)
    {

        int tid = omp_get_thread_num();
        wtf[tid] = tid;

        
//         while(i < 10 )
//         {   
//             #pragma omp critical
//             {
//                 mexPrintf("threadFunc says: %d,%d\n",tid,j);
//                 ++i;
//             }
//         }

	
    }
}            
            
            
            
            
            
            
            
            
            
            
            
const bool is_whitespace[256] = { false,false,false,false,false,false,false,false,false,true,true,false,false,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };

//Values for Integer portion of number
//------------------------------------
//Note that I'm avoiding subtracting 0 which makes these arrays rather large ...
//It may be faster to subtract and do the lookup ...
//We might even be better having a single array which we offset into
//e.g. js[parser_position-9] + js[parser_position-9+10] + js[parser_position-9+20] + js[parser_position-9+30] 
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
//I ran into problems with something being defined with leading zeros
//
//e.g. 0.0000000000001234567891234567
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

    double value;
    
    bool negate;
//     uint8_t n_int  = (uint8_t)num_info;
//     uint8_t n_frac = num_info >> 8;
//     uint8_t n_exp  = num_info >> 16;
    
	uint8_t *info2 = (uint8_t*)&num_info;
    
    if (*p == '-'){
        ++p;
        negate = true;    
    }else{
        negate = false;
    }

    value = 0;
    //I'm curious if you could take advantage of something like SSE
    //if each case was written out explicitly rather than via fall through
//     switch (n_int) {
    switch (info2[0]){
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
    
    if(info2[1]){
        ++p;
        switch (info2[1]) {
            case 20:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               value += p1e_7[*p++];
               value += p1e_8[*p++];
               value += p1e_9[*p++];
               value += p1e_10[*p++];
               value += p1e_11[*p++];
               value += p1e_12[*p++];
               value += p1e_13[*p++];
               value += p1e_14[*p++];
               value += p1e_15[*p++];
               value += p1e_16[*p++];
               value += p1e_17[*p++];
               value += p1e_18[*p++];
               value += p1e_19[*p++];
               value += p1e_20[*p++];
               break;
            case 19:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               value += p1e_7[*p++];
               value += p1e_8[*p++];
               value += p1e_9[*p++];
               value += p1e_10[*p++];
               value += p1e_11[*p++];
               value += p1e_12[*p++];
               value += p1e_13[*p++];
               value += p1e_14[*p++];
               value += p1e_15[*p++];
               value += p1e_16[*p++];
               value += p1e_17[*p++];
               value += p1e_18[*p++];
               value += p1e_19[*p++];
               break;               
            case 18:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               value += p1e_7[*p++];
               value += p1e_8[*p++];
               value += p1e_9[*p++];
               value += p1e_10[*p++];
               value += p1e_11[*p++];
               value += p1e_12[*p++];
               value += p1e_13[*p++];
               value += p1e_14[*p++];
               value += p1e_15[*p++];
               value += p1e_16[*p++];
               value += p1e_17[*p++];
               value += p1e_18[*p++];
               break;                  
            case 17:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               value += p1e_7[*p++];
               value += p1e_8[*p++];
               value += p1e_9[*p++];
               value += p1e_10[*p++];
               value += p1e_11[*p++];
               value += p1e_12[*p++];
               value += p1e_13[*p++];
               value += p1e_14[*p++];
               value += p1e_15[*p++];
               value += p1e_16[*p++];
               value += p1e_17[*p++];
               break;                
            case 16:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               value += p1e_7[*p++];
               value += p1e_8[*p++];
               value += p1e_9[*p++];
               value += p1e_10[*p++];
               value += p1e_11[*p++];
               value += p1e_12[*p++];
               value += p1e_13[*p++];
               value += p1e_14[*p++];
               value += p1e_15[*p++];
               value += p1e_16[*p++];
               break;
            case 15:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               value += p1e_7[*p++];
               value += p1e_8[*p++];
               value += p1e_9[*p++];
               value += p1e_10[*p++];
               value += p1e_11[*p++];
               value += p1e_12[*p++];
               value += p1e_13[*p++];
               value += p1e_14[*p++];
               value += p1e_15[*p++];
               break;
            case 14:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               value += p1e_7[*p++];
               value += p1e_8[*p++];
               value += p1e_9[*p++];
               value += p1e_10[*p++];
               value += p1e_11[*p++];
               value += p1e_12[*p++];
               value += p1e_13[*p++];
               value += p1e_14[*p++];
               break;
            case 13:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               value += p1e_7[*p++];
               value += p1e_8[*p++];
               value += p1e_9[*p++];
               value += p1e_10[*p++];
               value += p1e_11[*p++];
               value += p1e_12[*p++];
               value += p1e_13[*p++];
               break;
            case 12:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               value += p1e_7[*p++];
               value += p1e_8[*p++];
               value += p1e_9[*p++];
               value += p1e_10[*p++];
               value += p1e_11[*p++];
               value += p1e_12[*p++];
               break;
            case 11:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               value += p1e_7[*p++];
               value += p1e_8[*p++];
               value += p1e_9[*p++];
               value += p1e_10[*p++];
               value += p1e_11[*p++];
               break;
            case 10:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               value += p1e_7[*p++];
               value += p1e_8[*p++];
               value += p1e_9[*p++];
               value += p1e_10[*p++];
               break;
            case 9:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               value += p1e_7[*p++];
               value += p1e_8[*p++];
               value += p1e_9[*p++];
               break;
            case 8:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               value += p1e_7[*p++];
               value += p1e_8[*p++];
               break;
            case 7:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               value += p1e_7[*p++];
               break;
            case 6:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               value += p1e_6[*p++];
               break;
            case 5:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               value += p1e_5[*p++];
               break;
            case 4:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               value += p1e_4[*p++];
               break;
            case 3:
               value += p1e_1[*p++];
               value += p1e_2[*p++]; 
               value += p1e_3[*p++];
               break;
            case 2:
               value += p1e_1[*p++];
               value += p1e_2[*p++];
               break;
            case 1:
               value += p1e_1[*p++];

        }  
    }
    
    //End of if '.'    
    if (negate) {
        value = -value;
    }

    if(info2[2]){
        ++p;
          switch (*p){
            case '-':
                ++p;
                negate = true;
                break;
            case '+':
                ++p;
				// fall through -------
            default:
                negate = false;
        }  
        
		double exponent_value = 0;
        switch(info2[2]){
            case 4:
               exponent_value += p1e3[*p++];
            case 3:
               exponent_value += p1e2[*p++];
            case 2:
               exponent_value += p1e1[*p++]; //1e1 == 10, 2 #s plus the off by 1
            case 1:
               exponent_value += p1e0[*p++]; //1e0 == 1, an unfortunate mismatch of exponent and scalar
        } 
        
        if (negate){
          exponent_value = -exponent_value;  
        }
        value *= pow(10.0, exponent_value);
          
    }
    
    
    return value;
    
}

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
int string_to_double_no_math(unsigned char *p, unsigned char **char_offset) {

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

    if (*p == 'E' || *p == 'e') {
		++p;
		switch (*p) {
		case '-':
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
    
    return *(int *)data_info;
    
}


void seek_string_end(unsigned char *js, int *input_parser_position) {
    
	//  seek_string_end(js,&parser_position)

	//TODO: This could be completely wrong if we have unicode
	//although I think unicode has to be escaped????
	int parser_position = *input_parser_position;
	unsigned char c;

//I had wanted to just do a search for " but then
//I might miss the end of string
    
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

// // // // // #ifdef  __AVX__
// // // // //     mexPrintf("AVX is defined\n");
// // // // // #endif
// // // // // #ifdef  __AVX2__
// // // // //     mexPrintf("AVX2 is defined\n");
// // // // // #endif    
// // // // //     
// // // // //       /* Initialize the two argument vectors */
// // // // //   __m256 evens = _mm256_set_ps(2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0);
// // // // //   __m256 odds = _mm256_set_ps(1.0, 3.0, 5.0, 7.0, 9.0, 11.0, 13.0, 15.0);
// // // // // 
// // // // //   /* Compute the difference between the two vectors */
// // // // //   __m256 result = _mm256_sub_ps(evens, odds);
// // // // //     
// // // // //         //mexPrintf("AVX2 is defined\n");
    
    
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
    

    int temp_n_things = 0;
    int n_things = 0;
    
    int n_numeric = 0;
    int n_keys = 0;
    int n_strings = 0;

	int parser_position = -1;
	unsigned char *pEndNumber;
    int current_data_index = -1;

    int parent_index = 0;

    //This current allocation means we hit 4x the size of the file
    //in initial memory allocation
    int data_size_allocated = string_byte_length;
    int data_size_index_max = data_size_allocated - 1;
	int *data = mxMalloc(data_size_allocated * sizeof(int));
    
    const double MX_NAN = mxGetNaN();
    
    int *wtf = mxMalloc(16);
    
    do_calculation(wtf);
    
    mexPrintf("wtf[1] = %d\n",wtf[0]);
    mexPrintf("wtf[1] = %d\n",wtf[1]);
    mexPrintf("wtf[1] = %d\n",wtf[2]);
    mexPrintf("wtf[1] = %d\n",wtf[3]);
    
    mxFree(wtf);
    
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
    ++n_things;

	//Fall Through --------------------
S_OPEN_OBJECT_IN_KEY:
    
    EXPAND_DATA_CHECK(N_DATA_OBJECT);
    
    SET_TYPE(TYPE_OBJECT);
    STORE_AND_UPDATE_PARENT;
    ++current_data_index; //TAC
    INIT_N_THINGS;
    
    //Navigation -----------------
	SKIP_WHITESPACE

    switch (js[parser_position]) {
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
    STORE_TAC;
    
    //Fall Through ------
S_CLOSE_KEY_AND_OBJECT_SIMPLE:
    
    //Only update parent
    MOVE_UP_PARENT_INDEX;

	//Fall Through ------
S_CLOSE_OBJECT:

    FINALIZE_N_THINGS;
    STORE_TAC;
    MOVE_UP_PARENT_INDEX;
    
    if (NULL_PARENT_INDEX) {
		goto S_PARSE_END_OF_FILE;
	}
    
    if (PARENT_TYPE == TYPE_KEY) {
    	PROCESS_END_OF_KEY_VALUE
    } else {
        PROCESS_END_OF_ARRAY_VALUE
    }
    
//=============================================================
S_OPEN_ARRAY_IN_ARRAY:
	++n_things;

	//Fall Through -------------------------------
S_OPEN_ARRAY_IN_KEY:

    EXPAND_DATA_CHECK(N_DATA_ARRAY);
    
    SET_TYPE(TYPE_ARRAY);
    STORE_AND_UPDATE_PARENT;
    ++current_data_index; //TAC
    INIT_N_THINGS
    
	SKIP_WHITESPACE
    DO_ARRAY_JUMP
            
	//=============================================================
S_CLOSE_ARRAY:

    FINALIZE_N_THINGS;
    STORE_TAC;
    MOVE_UP_PARENT_INDEX;
    
	if (NULL_PARENT_INDEX) {
		goto S_PARSE_END_OF_FILE;
	}
    
    if (PARENT_TYPE == TYPE_KEY) {
        PROCESS_END_OF_KEY_VALUE
    } else {
        PROCESS_END_OF_ARRAY_VALUE
    }

//=============================================================
S_PARSE_KEY:
	++n_things;
    ++n_keys;
    
    EXPAND_DATA_CHECK(N_DATA_KEY);

    SET_TYPE(TYPE_KEY);
    STORE_AND_UPDATE_PARENT;
    
    //TAC handling -----------------------
    //tac - 0, start 1 end 2 
    //- assume 3 indices, true for string, #, null
    //- then + 1 for pointing to after
    ++current_data_index; 
    data[current_data_index] = current_data_index + 6;

    //start
    data[++current_data_index] = parser_position;

	seek_string_end(js, &parser_position);

    //end
	data[++current_data_index] = parser_position;

    //Navigation ------------------
	//TODO: Assume : and space otherwise use code below
    // if js[++parser_position] = ':' and 
    
    SKIP_WHITESPACE;

	if (js[parser_position] == ':') {
        SKIP_WHITESPACE
        DO_KEY_JUMP
        //goto *key_jump[js[parser_position]];
	}
	else {
		goto S_ERROR_MISSING_COLON_AFTER_KEY;
	}



	//=============================================================
S_PARSE_STRING_IN_ARRAY:
	n_things++;

    n_strings++;
    
    EXPAND_DATA_CHECK(N_DATA_STRING);
    SET_TYPE(TYPE_STRING);

	data[++current_data_index] = parser_position;
	seek_string_end(js, &parser_position);
    data[++current_data_index] = parser_position;

	PROCESS_END_OF_ARRAY_VALUE;



	//=============================================================
S_PARSE_STRING_IN_KEY:

    n_strings++;
    
    EXPAND_DATA_CHECK(N_DATA_STRING)    
	SET_TYPE(TYPE_STRING);

	data[++current_data_index] = parser_position;
	seek_string_end(js, &parser_position);
    data[++current_data_index] = parser_position;

	PROCESS_END_OF_KEY_VALUE



	//=============================================================
S_PARSE_NUMBER_IN_KEY:

    ++n_numeric;
    
    EXPAND_DATA_CHECK(N_DATA_NUMERIC)
    SET_TYPE(TYPE_NUMBER);

    data[++current_data_index] = parser_position;
	data[++current_data_index] = string_to_double_no_math(js + parser_position, &pEndNumber);
	parser_position = (int)(pEndNumber - js);
	parser_position--;

	PROCESS_END_OF_KEY_VALUE;

//=============================================================
S_PARSE_NUMBER_IN_ARRAY:
	++n_things;
    ++n_numeric;
    
    EXPAND_DATA_CHECK(N_DATA_NUMERIC);
	
    SET_TYPE(TYPE_NUMBER);
    data[++current_data_index] = parser_position;
	data[++current_data_index] = string_to_double_no_math(js + parser_position, &pEndNumber);
	parser_position = (int)(pEndNumber - js);
    
    if (js[parser_position] == ','){
        SKIP_WHITESPACE;
        DO_ARRAY_JUMP;
        
//         if (js[++parser_position] == ' '){
//             if (isdigit(js[++parser_position]) || js[parser_position] == '-'){
//                 goto S_PARSE_NUMBER_IN_ARRAY;
//             }else{
//                 parser_position--;
//                 SKIP_WHITESPACE;
//                 DO_ARRAY_JUMP;
//             }
//         }else{
//             parser_position--;
//             SKIP_WHITESPACE;
//             DO_ARRAY_JUMP;
//         }
    }else{
        parser_position--;
        PROCESS_END_OF_ARRAY_VALUE;
    }

//=============================================================
S_PARSE_NULL_IN_KEY:

    ++n_numeric;
    
    EXPAND_DATA_CHECK(N_DATA_NULL);
	SET_TYPE(TYPE_NULL);
    ++current_data_index;
    ++current_data_index;
    
    //TODO: Add null check ...
	parser_position += 3;
    
	PROCESS_END_OF_KEY_VALUE;



//=============================================================
S_PARSE_NULL_IN_ARRAY:

	n_things++;
    
    ++n_numeric;
    
    EXPAND_DATA_CHECK(N_DATA_NULL);
	SET_TYPE(TYPE_NULL);
    ++current_data_index;
    ++current_data_index;

	parser_position += 3;
    
	PROCESS_END_OF_ARRAY_VALUE;


//=============================================================
S_PARSE_TRUE_IN_KEY:
    
    EXPAND_DATA_CHECK(N_DATA_LOGICAL);
    SET_TYPE(TYPE_TRUE);
	parser_position += 3;
	PROCESS_END_OF_KEY_VALUE


S_PARSE_TRUE_IN_ARRAY:
    
	n_things++;
    EXPAND_DATA_CHECK(N_DATA_LOGICAL);
	SET_TYPE(TYPE_TRUE);
	parser_position += 3;
    PROCESS_END_OF_ARRAY_VALUE;

    
S_PARSE_FALSE_IN_KEY:
    
    EXPAND_DATA_CHECK(N_DATA_LOGICAL)
	SET_TYPE(TYPE_FALSE);
	parser_position += 4;
    PROCESS_END_OF_KEY_VALUE;

S_PARSE_FALSE_IN_ARRAY:
    
	n_things++;
    EXPAND_DATA_CHECK(N_DATA_LOGICAL);
	SET_TYPE(TYPE_FALSE);
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

//This error comes when we have a comma in an array that is not followed
// by a valid value => i.e. #, ", [, {, etc.
S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY:
    mexPrintf("Current character: %c\n",js[parser_position]);
    mexPrintf("Current position in string: %d\n",parser_position);
	mexErrMsgIdAndTxt("jsmn_mex:invalid_token", "Invalid token found after a comma in an array");
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
    
    //Hopefully this doesn't ever need to run ...
    //We're adding a single type value at the end to indicate we are done
    //rather than checking at each type value
    EXPAND_DATA_CHECK(1);
    data[++current_data_index] = TYPE_DATA_END; 
    
    //Set the data into storage
    data = mxRealloc(data,(current_data_index + 1)*sizeof(int));
	setStructField(plhs[0],data,"data",mxINT32_CLASS,current_data_index + 1);
    
    //Holding onto values
    int *p_data_size_allocated = mxMalloc(sizeof(int));
    *p_data_size_allocated = data_size_allocated;
    int *p_n_numeric = mxMalloc(sizeof(int));
    *p_n_numeric = n_numeric;
    int *p_n_keys = mxMalloc(sizeof(int));
    *p_n_keys = n_keys;
    int *p_n_strings = mxMalloc(sizeof(int));
    *p_n_strings= n_strings;

    setStructField(plhs[0],p_data_size_allocated,"data_size_allocated",mxINT32_CLASS,1);
    setStructField(plhs[0],p_n_numeric,"n_numeric",mxINT32_CLASS,1);
    setStructField(plhs[0],p_n_keys,"n_keys",mxINT32_CLASS,1);
    setStructField(plhs[0],p_n_strings,"n_strings",mxINT32_CLASS,1);
    
    int cur_number = -1;
    int cur_string = -1;
    int cur_key = -1;
    
    //Initialization of all numeric data
    double *numeric_data = mxMalloc(n_numeric*sizeof(double));

    unsigned char *char_start;
    int num_info;
    
    clock_t start_post_process, end_post_process;
    
    
    start_post_process = clock();
    current_data_index = -1;
    //while (current_data_index <= data_size_index_max){
    while (1){
      
// #define N_DATA_OBJECT   4
// #define N_DATA_ARRAY    4
// #define N_DATA_KEY      5
// #define N_DATA_STRING   3
// #define N_DATA_NUMERIC  3
// #define N_DATA_LOGICAL  1
// #define N_DATA_NULL     3
        
        switch(data[++current_data_index]){
            case TYPE_DATA_END:
                goto ALL_DONE;
            case TYPE_OBJECT:
                current_data_index += 3;
                break;
            case TYPE_ARRAY:
                current_data_index += 3;
                break;
            case TYPE_KEY:
                current_data_index += 4;
                break;    
            case TYPE_STRING:
                current_data_index += 2;
                break;
            case TYPE_NUMBER:
                //storage format:
                //1) type   2) start of number 3) info about number
                //We change #2 to pointing to the storage location of
                //the number in the array
//                 char_start = js + data[++current_data_index];
//                 data[current_data_index] = (++cur_number) + 1; //+1 for Matlab :/
//                 num_info = data[++current_data_index];
//                 numeric_data[cur_number] = string_to_double(char_start,num_info);
                
                current_data_index += 2;
                break;
            case TYPE_NULL:
//                 data[++current_data_index] = ++cur_number;
//                 numeric_data[cur_number] = MX_NAN;
//                 ++current_data_index;
                
                current_data_index += 2;
                break;    
            case TYPE_TRUE:
                break;
            case TYPE_FALSE:
                break;
        }   
    }
    
ALL_DONE:
    
    end_post_process = clock();
    double *elapsed_pp_time = mxMalloc(sizeof(double));
    *elapsed_pp_time = (double)(end_post_process - start_post_process)/CLOCKS_PER_SEC;
    
    setStructField(plhs[0],elapsed_pp_time,"elapsed_pp_time",mxDOUBLE_CLASS,1);
    
    setStructField(plhs[0],numeric_data,"numeric_data",mxDOUBLE_CLASS,n_numeric);        

    
    
	return;
}



