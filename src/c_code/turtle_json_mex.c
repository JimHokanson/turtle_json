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
    //On encountering a quote, we always need to look back.
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
        
void process_inputs(int nrhs, const mxArray *prhs[], unsigned char **json_string, size_t *string_byte_length){

    
    //TODO: This needs to be 2 functions:
    //1) Extraction of the string
    //2) Extraction of options
    
    //String Extraction
    //---------------------------------
    //1) file_path
    //2) raw_string_as_char, 'raw_string' = true, 'padding' = ?
    //3) raw_string_as_uint8, padding=?
    
    
    mxArray *mxArrayTemp;
    unsigned char *raw_string;
    unsigned char *json_string2;
    size_t string_byte_length_value;
    
    //Options
    //-----------
    //1) chars_per_token - NYI
    //2) n_tokens - NYI
    //3) raw_string - NYI
    //4) raw_is_padded - NYI
    
    //Format 1:
    //  token_info = jsmn_mex(file_path,options_struct)
    //Format 2:
    //  token_info = jsmn_mex(raw_string,options_struct)
    
    //TODO: Let's rewrite this as a function
    
    //TODO: Allow this to be uint8 or int8
    if (!mxIsClass(prhs[0],"char")){
        mexErrMsgIdAndTxt("jsmn_mex:invalid_input","First input needs to be a string");   
    }
    
    
    //If only a single input
    if (nrhs == 1){
        //Check if string or uint8
        readFileToString(prhs,json_string,string_byte_length);
        return;
    }
    
    //Two inputs ...
    if (!mxIsClass(prhs[1],"struct")){
        mexErrMsgIdAndTxt("jsmn_mex:invalid_input","Second input needs to be a struct");
    }
    
    mxArrayTemp = mxGetField(prhs[1],0,"raw_string");
    if (mxArrayTemp == NULL){
        readFileToString(prhs,json_string,string_byte_length);
        return;
    }
    
    string_byte_length_value = *string_byte_length;
    
    //Extraction of the string pointer
    //---------------------------------------------------------------------
    //TODO: Need to build in uint8 support
    if (!mxIsClass(mxArrayTemp,"char")){
        mexErrMsgIdAndTxt("jsmn_mex:invalid_input","raw_string needs to be a string");   
    }
    
    //TODO: Currently we pad regardless of whether or not
    //it is needed. We could allow not padding if the input
    //has already been padded

    raw_string = (unsigned char *) mxArrayToString(mxArrayTemp);
    string_byte_length_value = mxGetNumberOfElements(mxArrayTemp);
    *string_byte_length = string_byte_length_value;

    //For now we'll assume we're not padded
    //-----------------------------------------------
    json_string2 = mxMalloc(string_byte_length_value+N_PADDING);
    memcpy(json_string2,raw_string,string_byte_length_value);
    mxFree(raw_string);

    //TODO: I'm not thrilled with this being here and above
    json_string2[string_byte_length_value] = 0;
    json_string2[string_byte_length_value+1] = '\\';
    json_string2[string_byte_length_value+2] = '"';
    for (int i = 3; i < N_PADDING; i++){
        json_string2[string_byte_length_value + i] = 0;
    }
    
    *json_string = json_string2;
    
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    //  This function is meant to be called by the json.tokens constructor
    //
    //  Usage:
    //  ------
    //  token_info = jsmn_mex(file_path,options_struct)
    //
    //  token_info = jsmn_mex(raw_string,options_struct)
    //
    //  Inputs
    //  ------
    //  file_path
    //  raw_string
    //
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

    int chars_per_token; //NYI
    int n_tokens;   //NYI
    
    mxLogical raw_is_padded;
            
    
    
    TIC(start_read);
    
    //# of inputs check  --------------------------------------------------
    if (!(nrhs == 1 || nrhs == 2)){
        mexErrMsgIdAndTxt("jsmn_mex:n_inputs","Invalid # of inputs, 1 or 2 expected");
    }
    
    if (!(nlhs == 1)){
        mexErrMsgIdAndTxt("jsmn_mex:n_inputs","Invalid # of outputs, 1 expected");
    }
    
    //Processing of inputs ------------------------------------------------
    //TODO: Split into string parsing and option parsing ...
    //Options first, then string ...
    process_inputs(nrhs, prhs, &json_string, &string_byte_length);
    
    //This needs to precede TOC_AND_LOG() since the logging touches plhs[0]
    plhs[0] = mxCreateStructMatrix(1,1,0,NULL);
    
    TOC_AND_LOG(start_read,elapsed_read_time);

    //Let's hold onto the string for the user. Technically it isn't needed
    //once we exit this function, since all information is contained in
    //the other variables.
    setStructField(plhs[0],json_string,"json_string",mxUINT8_CLASS,string_byte_length);
    
    //Token parsing
    //-------------
    TIC(start_parse);
    
    parse_json(json_string, string_byte_length, plhs);
  
    TOC_AND_LOG(start_parse, elapsed_parse_time);
      
    
    //Post token parsing
    //------------------
    TIC(start_pp);
    
    parse_numbers(json_string,plhs);
    
    parse_keys(json_string,plhs);
    
    parse_strings(json_string,plhs);
    
    TOC_AND_LOG(start_pp,elapsed_pp_time);
    

}

//TODO: This code needs to be written
//This code is currently in two locations - file reading and adding to input string
void addBuffer(unsigned char *buffer){
  //TODO: Add on buffer   
}

