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

//  mex jsmn_mex.c jsmn.c

void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[] )
{
    //  Usage:
    //  ------
    //  str = fileread(file_path);
    //  token_info = jsmn_mex(str)
    //
    //
    //  Outputs:
    //  --------
    //  token_info
    //      - see wrapping Matlab function
    
    mxArray *output_info;
    mxArray *output_values;
    
    mwSize n_tokens_allocated;
    mwSize n_tokens_to_allocate;
    mwSize n_tokens_used;
    mwSize n_reallocations = 0;
    
    int parse_result;
    char *json_string;
    jsmn_parser p;
    jsmntok_t *t;
    double *values;
    size_t string_byte_length;
    
    //Input Handling
    //---------------------------------------------------------------------
    if (!(nrhs == 1 || nrhs == 2)){
        mexErrMsgIdAndTxt("jsmn_mex:n_inputs","Invalid # of inputs ...");
    }
    
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
            mexErrMsgIdAndTxt("jsmn_mex:bad_uint8_input","Currently uint8 should be padded with a trailing zero");
        }
    }else if (mxIsClass(prhs[0],"char")){
        json_string = mxArrayToString(prhs[0]);
        string_byte_length = mxGetNumberOfElements(prhs[0]);
        if (string_byte_length == 0){
            mexErrMsgIdAndTxt("jsmn_mex:null_string","Unhandled null string case");
        }
        
        //mexPrintf("Last Character: %c\n",json_string[string_byte_length]);
        
    }else {
        mexErrMsgIdAndTxt("jsmn_mex:invalid_input","Input should be a string or null terminated uint8");
    }
    
    if (nrhs == 2){
        if (mxIsDouble(prhs[1])){
            n_tokens_to_allocate = (mwSize )mxGetScalar(prhs[1]);
        }else{
            mexErrMsgIdAndTxt("jsmn_mex:input_type","2nd input type must be a double");    
        }
        
        //This could be an error ...
        if (n_tokens_to_allocate <= 0){
            n_tokens_to_allocate = 100;
        }
    } else {
        n_tokens_to_allocate = (string_byte_length/12);
        
        //This should be changed to not exceed the # of input characters
        if (n_tokens_to_allocate <= 0){
            n_tokens_to_allocate = 100;
        }
    }
    
    //Allocation of the # of tokens
    //----------------------------------------------------
    t = mxMalloc(n_tokens_to_allocate*sizeof(jsmntok_t));
    values = mxMalloc(n_tokens_to_allocate*sizeof(double));
    n_tokens_allocated = n_tokens_to_allocate;
    
    jsmn_init(&p);
    
    while (1) {
        //The main function call
        //------------------------------------------
        parse_result = jsmn_parse(&p,json_string,string_byte_length,t,n_tokens_allocated,values);
        n_tokens_used = parse_result;
        
        if (parse_result >= 0){
            //Then we are all set
            break;
        }else if (parse_result != JSMN_ERROR_NOMEM){
            mexErrMsgIdAndTxt("jsmn_mex:unrecognized_error","Code error, non-recognized error returned to jsmn_mex");  
        } //else - not enough memory, reallocate below

        n_reallocations += 1;
        
        //Reallocation
        //---------------------------------------
        //How much should we increase by?
        //
        //  n_tokens_allocated*(total_string_length/current_position)
        //
        //  This however will break if we have a really long token at the
        //  beginning followed by a lot of dense tokens
        //
        //  TODO: determine a better approach for this ...
        
        
        //I'm not thrilled with this approach but I think it will work
        n_tokens_to_allocate = (mwSize )(1.2 * ((double)string_byte_length/p.pos) * n_tokens_allocated);
        
        if (n_tokens_to_allocate == n_tokens_allocated){
            n_tokens_to_allocate = 2*n_tokens_allocated;
        }
        
        t = mxRealloc(t,n_tokens_to_allocate*sizeof(jsmntok_t));
        values = mxRealloc(values,n_tokens_to_allocate*8);
        n_tokens_allocated = n_tokens_to_allocate;
    }
    
    if (mxIsClass(prhs[0],"char")){
        mxFree(json_string);
    }
    
    //Output population
    //---------------------------------------------------------    
    output_info = mxCreateNumericArray(0, 0, mxINT32_CLASS, mxREAL);
    mxSetData(output_info, t);
    mxSetM(output_info, sizeof(jsmntok_t)/4);
    mxSetN(output_info, n_tokens_used);
    
    output_values = mxCreateNumericArray(0, 0, mxDOUBLE_CLASS, mxREAL);
    mxSetData(output_values, values);
    mxSetM(output_values, 1);
    mxSetN(output_values, n_tokens_used);
    
    plhs[0] = mxCreateStructMatrix(1,1,0,NULL);
    mxAddField(plhs[0],"info");
    mxSetField(plhs[0],0,"info",output_info);
    mxAddField(plhs[0],"values");
    mxSetField(plhs[0],0,"values",output_values);
    
    
    
}