/*
 *  Wrapper for jsmn:
 *      http://zserge.com/jsmn.html
 *
 *  Compile via:
 *      mex jsmn_mex.c jsmn.c -O
 *      mex OPTIMFLAGS="/Ox" jsmn_mex.c jsmn.c
 */

//TODO: These may not all be needed ...
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"
#include "mex.h"
#include "jsmn.h"

//Error code borrowed from:
//  http://www.mathworks.com/matlabcentral/fileexchange/55972-json-encode---decode
#define ERROR_MINRHS 1 // NYI
#define ERROR_MAXRHS 2 // NYI
#define ERROR_INVALID_ARGUMENT 3
#define ERROR_MALLOC 4

// enum jsmnerr {
// 	/* Not enough tokens were provided */
// 	JSMN_ERROR_NOMEM = -1,
// 	/* Invalid character inside JSON string */
// 	JSMN_ERROR_INVAL = -2,
// 	/* The string is not a full JSON packet, more bytes expected */
// 	JSMN_ERROR_PART = -3
// };
//
// char *json_str;

void error(const unsigned int e) {
    switch (e) {
        case JSMN_ERROR_INVAL:
            mexErrMsgIdAndTxt("jsmn_mex:invalid_chars","Invalid character inside a JSON string.");
        case JSMN_ERROR_INVALID_TOKEN_CHAR:
            mexErrMsgIdAndTxt("jsmn_mex:invalid_chars","Invalid token character inside the JSON input string.");
        case JSMN_ERROR_PART:
            mexErrMsgIdAndTxt("jsmn_mex:incomplete_input","Input string is not a full JSON packet, more bytes expected.");
        case ERROR_MINRHS:
            mexErrMsgIdAndTxt("MATLAB:minrhs","Not enough input arguments.");
        case ERROR_MAXRHS:
            mexErrMsgIdAndTxt("MATLAB:maxrhs","Too many input arguments.");
        case ERROR_INVALID_ARGUMENT:
            mexErrMsgIdAndTxt("jsmn_mex:InvalidArgument","Requires string input.");
        case ERROR_MALLOC:
            mexErrMsgIdAndTxt("jsmn_mex:malloc","Insufficient free heap space.");
    }
}


//  mex jsmn_mex.c jsmn.c

void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[] )
{
    //  Usage:
    //  ------
    //  str = fileread(file_path);
    //  token_info = jsmn_mex(str)
    //
    //  Outputs:
    //  --------
    //  token_info
    //      - see wrapping Matlab function
    
    int n_tokens_allocated;
    int n_new_tokens_to_allocate;
    int n_tokens_used;
    int parse_result;
    char *json_string;
    jsmn_parser p;
    jsmntok_t *t;
    double *values;
    size_t string_byte_length;
    
    //Input Checking
    //---------------------------------------------------------------------
    //TODO: Check the # of inputs
    
    if (mxIsUint8(prhs[0])){
        json_string = (char *)mxGetData(prhs[0]);
        string_byte_length = mxGetNumberOfElements(prhs[0])-1;
        if (string_byte_length <= 1){
            mexErrMsgIdAndTxt("jsmn_mex:null_string","Unhandled null string case");
        }
        //TODO: Check that the last character is a 0
        //0,1,2,3
        //1,2,3,4
        if (json_string[string_byte_length] != 0){
            mexErrMsgIdAndTxt("jsmn_mex:bad_uint8_input","Currently uint8 should be padded with a zero");
        }
    }else if (mxIsClass(prhs[0],"char")){
        json_string = mxArrayToString(prhs[0]);
        string_byte_length = mxGetNumberOfElements(prhs[0]);
        if (string_byte_length == 0){
            mexErrMsgIdAndTxt("jsmn_mex:null_string","Unhandled null string case");
        }
        
        mexPrintf("Last Character: %c\n",json_string[string_byte_length]);
        
    }else {
        mexErrMsgIdAndTxt("jsmn_mex:invalid_input","Input should be a string or null terminated uint8");
    }
    
    
    //Allocation of the # of tokens
    //-------------------------------------------------
    
    //This is a rough guess
    //We might eventually want to change this if you have LOTS of strings
    //
    //TODO: Make this a user input
    n_tokens_allocated = (string_byte_length/6);
    if (n_tokens_allocated < 100){
        n_tokens_allocated = 100;
    }
    //mexPrintf("N tokens allocated: %d\n",n_tokens_allocated);
    t = mxMalloc(n_tokens_allocated*sizeof(jsmntok_t));
    values = mxMalloc(n_tokens_allocated*8);
    if (t == NULL) error(ERROR_MALLOC);
    
    jsmn_init(&p);
    
    while (1) {
        parse_result = jsmn_parse(&p,json_string,string_byte_length,t,n_tokens_allocated,values);
        n_tokens_used = parse_result;
        
        if (parse_result >= 0){
            //Then we are all set
            break;
        }else if (parse_result != JSMN_ERROR_NOMEM){
            error(parse_result);
        } //else - not enough memory, reallocate below

        //Reallocation
        //---------------------------------------
        //I'm not thrilled with this approach but I think it will work
        n_new_tokens_to_allocate = (int)(1.2 * ((double)string_byte_length/p.pos) * n_tokens_allocated);
        if (n_new_tokens_to_allocate < n_tokens_allocated){
            mexPrintf("Tokens no increase: %d \n",n_new_tokens_to_allocate);
            n_new_tokens_to_allocate = 2*n_tokens_allocated;
            if (n_new_tokens_to_allocate > string_byte_length){
                n_new_tokens_to_allocate = string_byte_length;
            }
        }
        //mexPrintf("Reallocating to %d tokens\n",n_new_tokens_to_allocate);
        
        t = mxRealloc(t,n_new_tokens_to_allocate*sizeof(jsmntok_t));
        values = mxRealloc(values,n_new_tokens_to_allocate*8);
        
        if (t == NULL) error(ERROR_MALLOC);
        n_tokens_allocated = n_new_tokens_to_allocate;
    }
    //mexPrintf("Finished %d\n",n_tokens_allocated);
    //mexPrintf("N tokens used %d\n",n_tokens_used);
    
    if (mxIsClass(prhs[0],"char"))
        mxFree(json_string);
    
    
    plhs[0] = mxCreateNumericArray(0, 0, mxINT32_CLASS, mxREAL);
    mxSetData(plhs[0], t);
    mxSetM(plhs[0], sizeof(jsmntok_t)/4);
    mxSetN(plhs[0], n_tokens_used);
    
    plhs[1] = mxCreateNumericArray(0, 0, mxDOUBLE_CLASS, mxREAL);
    mxSetData(plhs[1], values);
    mxSetM(plhs[1], 1);
    mxSetN(plhs[1], n_tokens_used);
    
}