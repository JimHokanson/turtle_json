/*
 *  Wrapper for jsmn:
 *      http://zserge.com/jsmn.html
 *
 *  Compile via:
 *      mex jsmn_mex.c jsmn.c -O 
 *      
 *      //Not useful ... mex COMPFLAGS="$COMPFLAGS /arch:AVX /fp:fast" jsmn_mex.c jsmn.c -O 
 *      mex OPTIMFLAGS="/Ox" jsmn_mex.c jsmn.c
 *
 *      mex jsmn_mex.c jsmn.c COMPFLAGS="$COMPFLAGS -O3"
 *      
 *      setenv('MW_MINGW64_LOC','C:\TDM-GCC-64')
 *
 */

//TODO: These may not all be needed ...
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"

#include "stdint.h"  //uint_8
#include "mex.h"
#include "jsmn.h"

void setStructField2(mxArray *s, void *pr, const char *fieldname, mxClassID classid, mwSize N)
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
    
    int *n_tokens_used = mxMalloc(sizeof(int));
    
    
    //The initialization isn't really needed but it silences a warning
    //in the compiler - compiler not recognizing terminal errors in code
    //so it thinks you can pass in an uninitialized value to the main function
    size_t string_byte_length = 0;
    //same here 
    unsigned char *json_string = NULL;
    
    //Input Handling
    //---------------------------------------------------------------------
    if (!(nrhs == 1 || nrhs == 2)){
        mexErrMsgIdAndTxt("jsmn_mex:n_inputs","Invalid # of inputs ...");
    }
    
    if (mxIsUint8(prhs[0])){
        json_string = (unsigned char *)mxGetData(prhs[0]);
        //We've padded with 0
        string_byte_length = mxGetNumberOfElements(prhs[0])-1;
        if (string_byte_length <= 1){
            mexErrMsgIdAndTxt("jsmn_mex:null_string","Unhandled null string case");
        }
        if (json_string[string_byte_length] != 0){
            mexErrMsgIdAndTxt("jsmn_mex:bad_uint8_input","Currently uint8 should be padded with a trailing zero");
        }
    }else if (mxIsClass(prhs[0],"char")){
        json_string = (unsigned char *) mxArrayToString(prhs[0]);
        string_byte_length = mxGetNumberOfElements(prhs[0]);
        if (string_byte_length == 0){
            mexErrMsgIdAndTxt("jsmn_mex:null_string","Unhandled null string case");
        }
        
        //mexPrintf("Last Character: %c\n",json_string[string_byte_length]);
        
    }else {
        mexErrMsgIdAndTxt("jsmn_mex:invalid_input","Input should be a string or null terminated uint8");
    }
    
    plhs[0] = mxCreateStructMatrix(1,1,0,NULL);
    
    *n_tokens_used = jsmn_parse(json_string,string_byte_length,plhs);
  
    if (mxIsClass(prhs[0],"char")){
        mxFree(json_string);
    }
    
    
	setStructField2(plhs[0],n_tokens_used,"n_tokens_used",mxINT32_CLASS,1);
    
// // //     setStructField(plhs[0],values,"values",mxDOUBLE_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],types,"types",mxUINT8_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],sizes,"sizes",mxINT32_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],parents,"parents",mxINT32_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],tokens_after_close,"tokens_after_close",mxINT32_CLASS,n_tokens_used);
// // //     
// // //     setStructField(plhs[0],n_allocations,"n_allocations",mxINT32_CLASS,1);
}

