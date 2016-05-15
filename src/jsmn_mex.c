/*
 *  Compile via:
 *      
 *      //must install openmp when installing tdm-gcc
 *      setenv('MW_MINGW64_LOC','C:\TDM-GCC-64')
 *
 *      mex CFLAGS="$CFLAGS -std=c11 -fopenmp -mtune=ivybridge" LDFLAGS="$LDFLAGS -fopenmp" COPTIMFLAGS="-O3 -DNDEBUG" jsmn_mex.c jsmn.c jsmn_mex_post_process.c -O -v
 *
 *
 */

#include "jsmn.h"


void readFileToString(const mxArray *prhs[], unsigned char **p_buffer, size_t *string_byte_length){
    
	FILE *file;
    char *file_path;
	size_t file_length;
    int N_PADDING = 17;
    unsigned char *buffer;

    file_path = mxArrayToString(prhs[0]);
    
    //http://stackoverflow.com/questions/2575116/fopen-fopen-s-and-writing-to-files
    
    #ifdef WIN32
        errno_t err;
        if( (err  = fopen_s( &file, file_path, "rb" )) !=0 ) {
    #else
        if ((file = fopen(file_path, "rb")) == NULL) {
    #endif
        mexErrMsgIdAndTxt("jsmn_mex:file_open","Unable to open file");
    }
    
    
// 	fopen_s(&file, file_path, "rb");
//     if (!file)
// 	{
//         mexErrMsgIdAndTxt("jsmn_mex:file_open","Unable to open file");
// 	}
	
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
    
    plhs[0] = mxCreateStructMatrix(1,1,0,NULL);
    
    //Input Handling
    //---------------------------------------------------------------------
    if (!(nrhs == 1 || nrhs == 2 || nrhs == 3)){
        mexErrMsgIdAndTxt("jsmn_mex:n_inputs","Invalid # of inputs ...");
    }
    
    if (!(nlhs == 1)){
        mexErrMsgIdAndTxt("jsmn_mex:n_inputs","Invalid # of outputs");
    }
    
    TIC(start_read)
    
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
    
    //end_read = clock();
    
    
    
    TOC_AND_LOG(start_read,elapsed_read_time)
//     
//     double *elapsed_read_time = mxMalloc(sizeof(double));
//     *elapsed_read_time = (double)(end_read - start_read)/CLOCKS_PER_SEC;
//     
//     setStructField(plhs[0],elapsed_read_time,,mxDOUBLE_CLASS,1);

    if (is_file_path){
        setStructField(plhs[0],json_string,"json_string",mxUINT8_CLASS,string_byte_length);
    }
    
//     clock_t start_parse, end_parse;
//     start_parse = clock();
//     
    TIC(start_parse)
    
    //Main parsing call
    jsmn_parse(json_string,string_byte_length,plhs);
  
    TOC_AND_LOG(start_parse,elapsed_parse_time)
    
//     end_parse = clock();
//     
//     clock_t start_post_process, end_post_process;
//     start_post_process = clock();
//     double *elapsed_ = mxMalloc(sizeof(double));
//     *elapsed_pp_time = (double)(end_post_process - start_post_process)/CLOCKS_PER_SEC;
//     
//     setStructField(plhs[0],elapsed_pp_time,"elapsed_pp_time",mxDOUBLE_CLASS,1);
//     
    TIC(start_pp)
    
    parse_numbers(json_string,plhs);
    
    TOC_AND_LOG(start_pp,elapsed_pp_time)
    
//     end_post_process = clock();
//     double *elapsed_pp_time = mxMalloc(sizeof(double));
//     *elapsed_pp_time = (double)(end_post_process - start_post_process)/CLOCKS_PER_SEC;
//     
//     setStructField(plhs[0],elapsed_pp_time,"elapsed_pp_time",mxDOUBLE_CLASS,1);
    
    
    
// // //     setStructField(plhs[0],values,"values",mxDOUBLE_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],types,"types",mxUINT8_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],sizes,"sizes",mxINT32_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],parents,"parents",mxINT32_CLASS,n_tokens_used);
// // //     setStructField(plhs[0],tokens_after_close,"tokens_after_close",mxINT32_CLASS,n_tokens_used);
// // //     
// // //     setStructField(plhs[0],n_allocations,"n_allocations",mxINT32_CLASS,1);
}

