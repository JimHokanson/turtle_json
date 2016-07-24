/*
 *  Compile via:
 *      
 //must install openmp when installing tdm-gcc
 
 //Windows
 setenv('MW_MINGW64_LOC','C:\TDM-GCC-64')

 mex CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" COPTIMFLAGS="-O3 -DNDEBUG" turtle_json_mex.c turtle_json_main.c turtle_json_post_process.c -O -v

 mex CC='/usr/local/Cellar/gcc6/6.1.0/bin/gcc-6' CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" COPTIMFLAGS="-O3 -DNDEBUG" turtle_json_mex.c turtle_json_main.c turtle_json_post_process.c -O -v

 *
 brew update
 xcode-select --install
 http://stackoverflow.com/questions/24652996/homebrew-not-working-on-osx
 

 brew search gcc 
 //Took about 70 minutes on my laptop
 brew install homebrew/versions/gcc6 --without-multilib
 //May not be necessary, seems like the install does this ...
 brew link --overwrite --force gcc6
 brew unlink gcc6 && brew link gcc6 
 brew install --with-clang llvm
 
 *
 */

#include "turtle_json.h"

int N_PADDING = 17;

void readFileToString(const mxArray *prhs[], unsigned char **p_buffer, size_t *string_byte_length){
    
	FILE *file;
    char *file_path;
	size_t file_length;
    
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
	
	//Get file length
	fseek(file, 0, SEEK_END);
	file_length = ftell(file);
	fseek(file, 0, SEEK_SET);

	//Allocate memory
	buffer = mxMalloc(file_length+N_PADDING);

	//Read file contents into buffer
    //---------------------------------------------------
	fread(buffer, file_length, 1, file);
    fclose(file);
    buffer[file_length] = 0; 
    buffer[file_length+1] = '\\';
     buffer[file_length+2] = '"';
    for (int i = 3; i < N_PADDING; i++){
        //length 1, index 0
        //length file_length, max index - file_length-1
        //so padding starts at file_length + 0
        buffer[file_length + i] = 0;
    }
    
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
    size_t string_byte_length;
    unsigned char *json_string = NULL;
    bool is_file_path = true;

                
//             in.chars_per_token = sl.in.NULL;
//             in.n_tokens = sl.in.NULL;
//             in.raw_string = sl.in.NULL;
//             in.raw_is_padded = false;
    

    int chars_per_token;
    int n_tokens;
    unsigned char *raw_string;
    mxLogical raw_is_padded;
            
    mxArray *mxArrayTemp;
    
    
    
    //Input Handling
    //---------------------------------------------------------------------
    if (!(nrhs == 1 || nrhs == 2)){
        mexErrMsgIdAndTxt("jsmn_mex:n_inputs","Invalid # of inputs, 1 or 2 expected");
    }
    
    if (!(nlhs == 1)){
        mexErrMsgIdAndTxt("jsmn_mex:n_inputs","Invalid # of outputs, 1 expected");
    }
    
    TIC(start_read);
    
    //TODO: rewrite with less nesting
    
    if (!mxIsClass(prhs[0],"char")){
        mexErrMsgIdAndTxt("jsmn_mex:invalid_input","First input needs to be a string");   
    }
    
    if (nrhs == 1){
        readFileToString(prhs,&json_string,&string_byte_length);
    }else{
        
        if (!mxIsClass(prhs[1],"struct")){
            mexErrMsgIdAndTxt("jsmn_mex:invalid_input","Second input needs to be a struct");
        }
        
        mxArrayTemp = mxGetField(prhs[1],0,"raw_string");
        
        if (mxArrayTemp == NULL){
        	readFileToString(prhs,&json_string,&string_byte_length);
        }else{
            if (!mxIsClass(mxArrayTemp,"char")){
                mexErrMsgIdAndTxt("jsmn_mex:invalid_input","raw_string needs to be a string");   
            }
            
            //TODO: Need to build in uint8 support
            //TODO: Currently we pad regardless of whether or not
            //it is needed
            //TODO: Even if we pad, we might not get a padded result
            //due to string handling ...

            raw_string = (unsigned char *) mxArrayToString(mxArrayTemp);
            string_byte_length = mxGetNumberOfElements(mxArrayTemp);

            //For now we'll assume we're not padded
            //-----------------------------------------------
            json_string = mxMalloc(string_byte_length+N_PADDING);
            memcpy(json_string,raw_string,string_byte_length);
            mxFree(raw_string);

            //TODO: I'm not thrilled with this being here and above
            json_string[string_byte_length] = 0;
            json_string[string_byte_length+1] = '\\';
            json_string[string_byte_length+2] = '"';
            for (int i = 3; i < N_PADDING; i++){
                json_string[string_byte_length + i] = 0;
            }
        }

    }

    //Note, this needs to precede TOC_AND_LOG()
    //since the logging touches plhs[0]
    plhs[0] = mxCreateStructMatrix(1,1,0,NULL);
        
    TOC_AND_LOG(start_read,elapsed_read_time);

    //Let's hold onto this for the user. Technically it isn't needed
    //once we exit this function, since all information is contained in
    //the other variables
    setStructField(plhs[0],json_string,"json_string",mxUINT8_CLASS,string_byte_length);
    
    //Token parsing
    //-------------
    TIC(start_parse);
    
    parse_json(json_string,string_byte_length,plhs);
  
    TOC_AND_LOG(start_parse,elapsed_parse_time);
      
    
    //Post token parsing
    //------------------
    TIC(start_pp);
    
    parse_numbers(json_string,plhs);
    
    parse_keys(json_string,plhs);
    
    parse_strings(json_string,plhs);
    
    TOC_AND_LOG(start_pp,elapsed_pp_time);
    

}

