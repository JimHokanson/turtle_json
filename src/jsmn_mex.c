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
    
    //TODO: https://www.mathworks.com/matlabcentral/newsreader/view_thread/160022
    // mxArray **mxStrings; // 
    // mxStrings = (mxArray **) mxMalloc( mxStringsSize * sizeof(mxArray
    // mxStrings = (mxArray **) mxRealloc(mxStrings,(mxStringsSize += 5) * sizeof(mxArray *));
    
    //mxStrings[largestStringIndex] = mxCreateString( stringBase );
    
    /*
     // Create the cell matrix and fill with the gathered mxArray strings

    cellRows = largestStringIndex + 1;
    cellColumns = 1;
    plhs[0] = mxCreateCellMatrix(cellRows, cellColumns);
    for( i=0; i<cellRows; i++)
        mxSetCell(plhs[0], i, mxStrings[i] );
    
// Free the memory for the mxArray pointer array. Note that this
doesn't free the
// mxArrays that these pointers point to ... this only frees the array
of pointers
// that were used to keep track of them temorarily.

    mxFree( mxStrings );
}
*/
    
    mxArray *output_values;
    mxArray *output_types;
    mxArray *output_starts;
    mxArray *output_ends;
    mxArray *output_sizes;
    mxArray *output_parents;
    mxArray *output_tokens_after_close;
    mxArray *output_strings;
    
    double *values;
    uint8_t *types;
    int *starts;
    int *ends;
    int *sizes;
    int *parents;
    int *tokens_after_close;
    
    //mxArray *output_strings;
    
    mwSize n_tokens_allocated;
    mwSize n_tokens_to_allocate;
    mwSize n_tokens_used;
    mwSize n_reallocations = 0;
    
    int parse_result;
    
    char *json_string;
    size_t string_byte_length;
    
    jsmn_parser p;

    
    //Input Handling
    //---------------------------------------------------------------------
    if (!(nrhs == 1 || nrhs == 2)){
        mexErrMsgIdAndTxt("jsmn_mex:n_inputs","Invalid # of inputs ...");
    }
    
    if (mxIsUint8(prhs[0])){
        json_string = (char *)mxGetData(prhs[0]);
        //We've padded with 0
        string_byte_length = mxGetNumberOfElements(prhs[0])-1;
        if (string_byte_length <= 1){
            mexErrMsgIdAndTxt("jsmn_mex:null_string","Unhandled null string case");
        }
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
    values  = mxMalloc(n_tokens_to_allocate*sizeof(double));
    types   = mxMalloc(n_tokens_to_allocate);
    starts  = mxMalloc(n_tokens_to_allocate*sizeof(int));
    ends    = mxMalloc(n_tokens_to_allocate*sizeof(int));
    sizes   = mxMalloc(n_tokens_to_allocate*sizeof(int));
    parents = mxMalloc(n_tokens_to_allocate*sizeof(int));
    tokens_after_close = mxMalloc(n_tokens_to_allocate*sizeof(int));
    
    output_strings = mxCreateCellMatrix(1,n_tokens_to_allocate);
    
    
    n_tokens_allocated = n_tokens_to_allocate;
    
    jsmn_init(&p);
    
    while (1) {
        //The main function call
        //------------------------------------------
        parse_result = jsmn_parse(&p,json_string,string_byte_length,n_tokens_allocated,values,types,starts,ends,sizes,parents,tokens_after_close, output_strings);
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
        n_tokens_to_allocate = (mwSize )(1.2 * ((double)string_byte_length/p.position) * n_tokens_allocated);
        
        if (n_tokens_to_allocate == n_tokens_allocated){
            n_tokens_to_allocate = 2*n_tokens_allocated;
        }
        
        //mexPrintf("Trying to reallocate\n");
        values = mxRealloc(values,n_tokens_to_allocate*8);
        types = mxRealloc(types,n_tokens_to_allocate);
        starts = mxRealloc(starts,n_tokens_to_allocate*sizeof(int));
        ends = mxRealloc(ends,n_tokens_to_allocate*sizeof(int));
        sizes = mxRealloc(sizes,n_tokens_to_allocate*sizeof(int));
        parents = mxRealloc(parents,n_tokens_to_allocate*sizeof(int));
        tokens_after_close = mxRealloc(tokens_after_close,n_tokens_to_allocate*sizeof(int));
        mxSetN(output_strings,n_tokens_to_allocate);
        
        n_tokens_allocated = n_tokens_to_allocate;
    }
    
    
    
    if (mxIsClass(prhs[0],"char")){
        mxFree(json_string);
    }
    
    //Output population
    //---------------------------------------------------------    
//     output_info = mxCreateNumericArray(0, 0, mxINT32_CLASS, mxREAL);
//     mxSetData(output_info, t);
//     mxSetM(output_info, sizeof(jsmntok_t)/4);
//     mxSetN(output_info, n_tokens_used);
    
    output_values = mxCreateNumericArray(0, 0, mxDOUBLE_CLASS, mxREAL);
    mxSetData(output_values, values);
    mxSetM(output_values, 1);
    mxSetN(output_values, n_tokens_used);
    
    output_types = mxCreateNumericArray(0, 0, mxUINT8_CLASS, mxREAL);
    mxSetData(output_types, types);
    mxSetM(output_types, 1);
    mxSetN(output_types, n_tokens_used);  
    
    output_starts = mxCreateNumericArray(0, 0, mxINT32_CLASS, mxREAL);
    mxSetData(output_starts, starts);
    mxSetM(output_starts, 1);
    mxSetN(output_starts, n_tokens_used);
    
    output_ends = mxCreateNumericArray(0, 0, mxINT32_CLASS, mxREAL);
    mxSetData(output_ends, ends);
    mxSetM(output_ends, 1);
    mxSetN(output_ends, n_tokens_used);
    
    output_sizes = mxCreateNumericArray(0, 0, mxINT32_CLASS, mxREAL);
    mxSetData(output_sizes, sizes);
    mxSetM(output_sizes, 1);
    mxSetN(output_sizes, n_tokens_used);
    
    output_parents = mxCreateNumericArray(0, 0, mxINT32_CLASS, mxREAL);
    mxSetData(output_parents, parents);
    mxSetM(output_parents, 1);
    mxSetN(output_parents, n_tokens_used);
    
    output_tokens_after_close = mxCreateNumericArray(0, 0, mxINT32_CLASS, mxREAL);
    mxSetData(output_tokens_after_close, tokens_after_close);
    mxSetM(output_tokens_after_close, 1);
    mxSetN(output_tokens_after_close, n_tokens_used);
    
    mxSetN(output_strings,n_tokens_used);
    
    plhs[0] = mxCreateStructMatrix(1,1,0,NULL);
    mxAddField(plhs[0],"values");
    mxSetField(plhs[0],0,"values",output_values);
    mxAddField(plhs[0],"types");
    mxSetField(plhs[0],0,"types",output_types);
    mxAddField(plhs[0],"starts");
    mxSetField(plhs[0],0,"starts",output_starts);
    mxAddField(plhs[0],"ends");
    mxSetField(plhs[0],0,"ends",output_ends);
    mxAddField(plhs[0],"sizes");
    mxSetField(plhs[0],0,"sizes",output_sizes);
    mxAddField(plhs[0],"parents");
    mxSetField(plhs[0],0,"parents",output_parents);
    mxAddField(plhs[0],"tokens_after_close");
    mxSetField(plhs[0],0,"tokens_after_close",output_tokens_after_close);
    mxAddField(plhs[0],"strings");
    mxSetField(plhs[0],0,"strings",output_strings);
}