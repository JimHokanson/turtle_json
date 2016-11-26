#include "turtle_json.h"

#define N_PADDING 17

bool padding_is_necessary(unsigned char *input_bytes, size_t input_string_length){

    if (input_string_length >= N_PADDING){
        if (input_bytes[input_string_length-N_PADDING] == 0){
            mexPrintf("Found padding");
        }
    }
    
    //     //TODO: I'm not thrilled with this being here and above
//     json_string2[string_byte_length_value] = 0;
//     json_string2[string_byte_length_value+1] = '\\';
//     json_string2[string_byte_length_value+2] = '"';
//     for (int i = 3; i < N_PADDING; i++){
//         json_string2[string_byte_length_value + i] = 0;
//     }
    
    return true;
    
}

void add_parse_buffer(unsigned char *buffer, size_t array_length){
    //
    //  TODO: Describe this and why we are doing this ...
    
    buffer[array_length] = 0;
    //On encountering a quote, we always need to look back.
    buffer[array_length+1] = '\\';
    buffer[array_length+2] = '"';
    for (int i = 3; i < N_PADDING; i++){
        //length 1, index 0
        //length file_length, max index - file_length-1
        //so padding starts at file_length + 0
        buffer[array_length + i] = 0;
    }
}

void process_input_string(const mxArray *prhs[], unsigned char **json_string, size_t *json_string_length){
    //
    //  The input string is raw JSON, which may or may not
    //  have the parsing buffer added.
    //
    
    size_t input_string_length;
    size_t output_string_length;
    unsigned char *input_string;
    unsigned char *output_string;
    
    input_string = (unsigned char *) mxArrayToString(prhs[0]);
    input_string_length = mxGetNumberOfElements(prhs[0]);
    
    if (padding_is_necessary(input_string,input_string_length)){
        output_string_length = input_string_length + N_PADDING;
        output_string = mxMalloc(output_string_length);
        memcpy(output_string,input_string,input_string_length);
        mxFree(input_string);
//         mexPrintf("in len: %d\n",input_string_length);
//         mexPrintf("First char: %c\n",output_string[0]);
//         mexPrintf("2: %c\n",output_string[1]);
//         mexPrintf("3: %c\n",output_string[2]);
//         mexPrintf("4: %c\n",output_string[3]);
        add_parse_buffer(output_string, input_string_length);
    }else{
        output_string = input_string;
        output_string_length = input_string_length;
    }
    
    *json_string = output_string;
    *json_string_length = output_string_length;
    
}

void process_input_bytes(const mxArray *prhs[], unsigned char **json_string, size_t *json_string_length){
    //
    //  The first input (prhs[0] is a byte (uint8 or int8) array, which
    //  may or may not have the parsing buffer added.
    //
    
    size_t input_string_length;
    size_t output_string_length;
    unsigned char *input_string;
    unsigned char *output_string;
    
    input_string = (unsigned char *)mxGetData(prhs[0]);
    input_string_length = mxGetNumberOfElements(prhs[0]);
    
    if (padding_is_necessary(input_string,input_string_length)){
        output_string_length = input_string_length + N_PADDING;
        output_string = mxMalloc(output_string_length);
        memcpy(output_string,input_string,input_string_length);
        mxFree(input_string);
        add_parse_buffer(output_string, input_string_length);
    }else{
        output_string = input_string;
        output_string_length = input_string_length;
    }
    
    *json_string = output_string;
    *json_string_length = output_string_length;

}



void read_file_to_string(const mxArray *prhs[], unsigned char **p_buffer, size_t *string_byte_length){
    
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
        mexErrMsgIdAndTxt("turtle_json:file_open","Unable to open file");
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
    
    add_parse_buffer(buffer, file_length);
    
    *string_byte_length = file_length; 
    
    *p_buffer = buffer;
    
    mxFree(file_path);
}
        
void get_json_string(int nrhs, const mxArray *prhs[], unsigned char **json_string, size_t *string_byte_length, Options *options){
    //
    //  The input JSON can be:
    //  1) Path to a file
    //  2) raw character string  
    //  3) raw byte string
    
    mxArray *mxArrayTemp;
    unsigned char *raw_string;
    unsigned char *json_string2;
    size_t string_byte_length_value;
    
    //TODO: Get rid of this, and just do the checks below ...
    if (!(mxIsClass(prhs[0],"char") || options->has_raw_bytes)){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","First input needs to be a string or bytes");   
    }
    
    if (options->has_raw_string){
     	if (!mxIsClass(prhs[0],"char")){
            mexErrMsgIdAndTxt("turtle_json:invalid_input","'raw_string' input needs to be a string");   
        }
        process_input_string(prhs,json_string,string_byte_length);
    }else if (options->has_raw_bytes){
        process_input_bytes(prhs,json_string,string_byte_length);
    }else{
        //file_path
        read_file_to_string(prhs,json_string,string_byte_length);
    }

}

void init_options(int nrhs, const mxArray*prhs[],Options *options){
    
    mxArray *mxArrayTemp;
    
    //Initialization of defaults
    //--------------------------
    options->has_raw_string = false;
    options->has_raw_bytes = false;
    
    options->n_tokens = 0;
    options->n_keys = 0;
    options->n_strings = 0;
    options->n_numbers = 0;
    options->chars_per_token = 0;
    
    if (nrhs < 2){
        return;
    }
    
    if (!mxIsClass(prhs[1],"struct")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","Second input needs to be a struct");
    }
        
    mxArrayTemp = mxGetField(prhs[1],0,"raw_string");
    if (mxArrayTemp != NULL){
        //TODO: Should get that it is true ...
        if (mxIsClass(prhs[0],"char")){
            options->has_raw_string = true;
        }else if (mxIsClass(prhs[0],"uint8") || mxIsClass(prhs[0],"int8")){
            //Note, we'll allow bytes for a "raw_string"
            options->has_raw_bytes = true;
        }else{
            mexErrMsgIdAndTxt("turtle_json:n_inputs","Invalid # of inputs, 1 or 2 expected");
        }
    }else if (mxIsClass(prhs[0],"uint8") || mxIsClass(prhs[0],"int8")){
        options->has_raw_bytes = true;
    }

    mxArrayTemp = mxGetField(prhs[1],0,"n_tokens");
    if (mxArrayTemp != NULL){
        if (mxIsClass(mxArrayTemp,"double")){
            options->n_tokens = (int)mxGetScalar(mxArrayTemp);
        }else{
            mexErrMsgIdAndTxt("turtle_json:invalid_input","n_tokens option needs to be a double");
        }
    }

    mxArrayTemp = mxGetField(prhs[1],0,"n_keys");
    if (mxArrayTemp != NULL){
        if (mxIsClass(mxArrayTemp,"double")){
            options->n_keys = (int)mxGetScalar(mxArrayTemp);
        }else{
            mexErrMsgIdAndTxt("turtle_json:invalid_input","n_keys option needs to be a double");
        }
    }
    
    mxArrayTemp = mxGetField(prhs[1],0,"n_strings");
    if (mxArrayTemp != NULL){
        if (mxIsClass(mxArrayTemp,"double")){
            options->n_strings = (int)mxGetScalar(mxArrayTemp);
        }else{
            mexErrMsgIdAndTxt("turtle_json:invalid_input","n_strings option needs to be a double");
        }
    }
    
    mxArrayTemp = mxGetField(prhs[1],0,"n_numbers");
    if (mxArrayTemp != NULL){
        if (mxIsClass(mxArrayTemp,"double")){
            options->n_numbers = (int)mxGetScalar(mxArrayTemp);
        }else{
            mexErrMsgIdAndTxt("turtle_json:invalid_input","n_numbers option needs to be a double");
        }
    }
    
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    //  This function is meant to be called by the json.tokens constructor
    //
    //  Usage:
    //  ------
    //  token_info = turtle_json(file_path,options_struct)
    //
    //  token_info = turtle_json(raw_string,options_struct)
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

    Options options;

    mxLogical raw_is_padded;
    
    TIC(start_read);
    
    //# of inputs check  --------------------------------------------------
    if (!(nrhs == 1 || nrhs == 2)){
        mexErrMsgIdAndTxt("turtle_json:n_inputs","Invalid # of inputs, 1 or 2 expected");
    }
    
    if (!(nlhs == 1)){
        mexErrMsgIdAndTxt("turtle_json:n_inputs","Invalid # of outputs, 1 expected");
    }
    
    //Processing of inputs
    //-------------------------------------
    init_options(nrhs, prhs, &options);
    
    get_json_string(nrhs, prhs, &json_string, &string_byte_length, &options);

    //This needs to precede TOC_AND_LOG() since the logging touches plhs[0]
    plhs[0] = mxCreateStructMatrix(1,1,0,NULL);
    
    TOC_AND_LOG(start_read,elapsed_read_time);

    //Let's hold onto the string for the user. Technically it isn't needed
    //once we exit this function, since all information is contained in
    //the other variables.
    //
    //We subtract the length of the buffer so that the buffer is not 
    //displayed to the user
    setStructField(plhs[0],json_string,"json_string",mxUINT8_CLASS,string_byte_length-N_PADDING);
    
    //Token parsing
    //-------------
    TIC(start_parse);
    
    parse_json(json_string, string_byte_length, plhs, &options);
  
    TOC_AND_LOG(start_parse, elapsed_parse_time);
      
    
    //Post token parsing
    //------------------
    TIC(start_pp);
    
    parse_numbers(json_string,plhs);
    
    //keys
    //parse_char_data(json_string,plhs, true);
    
    //strings
    parse_char_data(json_string,plhs, false);
    
    //void parse_char_data(unsigned char *js,mxArray *plhs[], bool is_key)
    
    //parse_strings(json_string,plhs);
    
    TOC_AND_LOG(start_pp,elapsed_pp_time);
    

}

