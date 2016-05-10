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
#include <time.h> //clock

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

void readFileToString(const mxArray *prhs[], unsigned char **p_buffer, size_t *string_byte_length){
    
	FILE *file;
    char *file_path;
	size_t file_length;
    int N_PADDING = 17;
    unsigned char *buffer;

    file_path = mxArrayToString(prhs[0]);
    
	//Open file
	//file = fopen(file_path, "rb");
	fopen_s(&file, file_path, "rb");
    if (!file)
	{
        mexErrMsgIdAndTxt("jsmn_mex:file_open","Unable to open file");
	}
	
	//Get file length
	fseek(file, 0, SEEK_END);
	file_length = ftell(file);
	fseek(file, 0, SEEK_SET);

	//Allocate memory
	buffer = mxMalloc(file_length+N_PADDING);

	//Read file contents into buffer
	fread(buffer, file_length, 1, file);
    fclose(file);
    buffer[file_length] = 0; 
    for (int i = 0; i < N_PADDING; i++){
        //length 1, index 0
        //length file_length, max index - file_length-1
        //so padding starts at file_length + 0
        buffer[file_length + i] = 0;
    }
    
    //This would allow us to only seek for "
    buffer[file_length+1] = '"';
    
    *string_byte_length = file_length; 
    
    *p_buffer = buffer;
    
    mxFree(file_path);
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    //  Usage:
    //  ------
    //  token_info = jsmn_mex(file_path,*is_string,*allocation_size)
    //
    //  Inputs #2 and #3 are not yet implemented
    //
    //  TODO: Eventually we should allow passing in a padded version
    //  of the string rather than expanding it ourselves
    //
    //  Outputs:
    //  --------
    //  token_info
    //      - see wrapping Matlab function
    
    
    
    //The initialization isn't really needed but it silences a warning
    //in the compiler - compiler not recognizing terminal errors in code
    //so it thinks you can pass in an uninitialized value to the main function
    size_t string_byte_length = 0;
    //same here 
    unsigned char *json_string = NULL;
    
    bool is_file_path = true;
    
    clock_t start_read, end_read;

    //Input Handling
    //---------------------------------------------------------------------
    if (!(nrhs == 1 || nrhs == 2 || nrhs == 3)){
        mexErrMsgIdAndTxt("jsmn_mex:n_inputs","Invalid # of inputs ...");
    }
    
    if (!(nlhs == 1)){
        mexErrMsgIdAndTxt("jsmn_mex:n_inputs","Invalid # of outputs");
    }
    
    start_read = clock();
    if (mxIsClass(prhs[0],"char")){
        
        if (nrhs > 1){
            if (mxIsClass(prhs[1],"double")){
                is_file_path = mxGetScalar(prhs[1]) == 0;
            }else{
                mexErrMsgIdAndTxt("jsmn_mex:invalid_input","Second input needs to be a double");    
            }
        }
        
        if (is_file_path){
            readFileToString(prhs,&json_string,&string_byte_length);
            //mexPrintf("String byte length: %d\n",string_byte_length);
        }else{
            json_string = (unsigned char *) mxArrayToString(prhs[0]);
            string_byte_length = mxGetNumberOfElements(prhs[0]);
            
            //TODO: Need to buffer
            
            mexErrMsgIdAndTxt("jsmn_mex:not_yet_implemented","Not yet implemented");
            
            if (string_byte_length == 0){
                mexErrMsgIdAndTxt("jsmn_mex:null_string","Unhandled null string case");
            }
        }
    }else {
        mexErrMsgIdAndTxt("jsmn_mex:invalid_input","Input should be a string or null terminated uint8");
    }
    
    end_read = clock();
    
    plhs[0] = mxCreateStructMatrix(1,1,0,NULL);
    
    double *elapsed_read_time = mxMalloc(sizeof(double));
    *elapsed_read_time = (double)(end_read - start_read)/CLOCKS_PER_SEC;
    
    setStructField2(plhs[0],elapsed_read_time,"elapsed_read_time",mxDOUBLE_CLASS,1);

    if (is_file_path){
        setStructField2(plhs[0],json_string,"json_string",mxUINT8_CLASS,string_byte_length);
    }
    

    jsmn_parse(json_string,string_byte_length,plhs);
  

    
    
    
    
    
// // //     setStructField(plhs[0],values,"values",mxDOUBLE_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],types,"types",mxUINT8_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],sizes,"sizes",mxINT32_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],parents,"parents",mxINT32_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],tokens_after_close,"tokens_after_close",mxINT32_CLASS,n_tokens_used);
// // //     
// // //     setStructField(plhs[0],n_allocations,"n_allocations",mxINT32_CLASS,1);
}

