#include "turtle_json.h"

//  This is the entry function 

#define N_PADDING 17

//TODO: Clean these two up ... I don't think both are needed
//                      1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7
#define BUFFER_STRING "\0\\\"\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        
//                            1 2  3  4 5 6 7 8 9 0 1 2 3 4 5 6 7
uint8_t BUFFER_STRING2[20] = {0,92,34,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

bool padding_is_necessary(unsigned char *input_bytes, size_t input_string_length){
    //
    //  Return whether or not the parse buffer is present/necessary. If
    //  already present, it is not necessary.
    //
    //  This is for a string array or byte array input, where we don't 
    //  know if it has been appropriately padded to prevent parsing
    //  past the end of the string.
    
    if (input_string_length >= N_PADDING){ 
        return memcmp(&input_bytes[input_string_length-N_PADDING],BUFFER_STRING2,N_PADDING);
    }else{
        return true;
    }
}

void add_parse_buffer(unsigned char *buffer, size_t array_length){
    //
    //  Buffer is currently 17 characters of the form:
    //  0\"000000...  <= 0 means null character, not zero
    //
    //  The " character
    //  stops a potentially non-terminated string. The \ character
    //  is a natural check anyways in our algorithm where we first find
    //  a " character, and then backtrack. If we happen to find this 
    //  last " character, and we backtrack and find the \ character, then
    //  we need a special check, for the initial null character, to
    //  differentiate this stop from a normally escaped " character.
    //
    //  i.e. differentiate:
    //      ...test string\" is a good idea",           <= escaped "
    //      vs
    //      ...test string is not terminated0\"0000000  <= bad json string
    //
    //  The length of the buffer is to prevent search problems with SIMD
    //  that process multiple characters at a time (i.e. we don't want to
    //  parse past the end of the string)
    
    //TODO: Do a memcpy
    
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

void process_input_string(const mxArray *prhs[], unsigned char **json_string, size_t *json_string_length, int *buffer_added){
    //
    //  The input string is raw JSON, which may or may not
    //  have the parsing buffer added.
    //
    //
    
    
    size_t input_string_length;
    size_t output_string_length;
    unsigned char *input_string;
    unsigned char *output_string;
    
    //  TODO: It is not clear that these methods will respect UTF-8 encoding
    //      => mxArrayToString_UTF8 - when was this deprecated?
    //      => mxArrayToUTF8String  - 2015a
    //      - see next TODO, we are likely better off rolling our own ...
    //      - although for now, correct would be better than fast
    //  TODO: If the buffer is not present here, we are doing a memory reallocation
    //  anyway, so we might as well both transform the string and add the buffer
    //  at the same time, rather than do two memory reallocations
    input_string = (unsigned char *) mxArrayToString(prhs[0]);
    input_string_length = mxGetNumberOfElements(prhs[0]);
    
    if (padding_is_necessary(input_string,input_string_length)){
        *buffer_added = 1;
        output_string_length = input_string_length + N_PADDING;
        output_string = mxMalloc(output_string_length);
        memcpy(output_string,input_string,input_string_length);
        mxFree(input_string);
        add_parse_buffer(output_string, input_string_length);
    }else{
        *buffer_added = 0;
        output_string = input_string;
        output_string_length = input_string_length;
    }
    
    *json_string = output_string;
    *json_string_length = output_string_length - N_PADDING;
    
}

void process_input_bytes(const mxArray *prhs[], unsigned char **json_string, size_t *json_string_length, int *buffer_added, int *is_input){
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
        *buffer_added = 1;
        output_string_length = input_string_length + N_PADDING;
        output_string = mxMalloc(output_string_length);
        memcpy(output_string,input_string,input_string_length);
        add_parse_buffer(output_string, input_string_length);
    }else{
        *buffer_added = 0;
        *is_input = 1;
        output_string = input_string;
        output_string_length = input_string_length ;
    }
    
    *json_string = output_string;
    *json_string_length = output_string_length - N_PADDING;

}
//=========================================================================
//=========================================================================
void read_file_to_string(const mxArray *prhs[], unsigned char **p_buffer, size_t *string_byte_length){
    
	FILE *file;
    char *file_path;
	size_t file_length;
    size_t file_path_string_length;
    
    unsigned char *buffer;

    file_path = mxArrayToString(prhs[0]);
    
    //http://stackoverflow.com/questions/2575116/fopen-fopen-s-and-writing-to-files
    #ifdef WIN32
        errno_t err;
        if( (err  = fopen_s( &file, file_path, "rb" )) !=0 ) {
    #else
        if ((file = fopen(file_path, "rb")) == NULL) {
    #endif
            file_path_string_length = mxGetNumberOfElements(prhs[0]);
            //TODO: This could be improved for printing long file paths
            if (file_path_string_length > 100){
                file_path[96] = '.';
                file_path[97] = '.';
                file_path[98] = '.';
              	file_path[99] = '\0';
            }
            mexErrMsgIdAndTxt("turtle_json:file_open",
            	"Unable to open file: %s\nIf a string, consider using json.stringToTokens or json.parse instead",file_path);
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
        
void get_json_string(mxArray *plhs[], int nrhs, const mxArray *prhs[], unsigned char **json_string, size_t *string_byte_length, Options *options){
    //
    //  The input JSON can be:
    //  1) Path to a file
    //  2) raw character string  
    //  3) raw byte string
    
    mxArray *mxArrayTemp;
    unsigned char *raw_string;
    unsigned char *json_string2;
    int buffer_added = 0;
    int is_input = 0;
    
    //TODO: Get rid of this, and just do the checks below ...
    //TODO: Actually, I think we do the checks in the input options handling ...
    if (!(mxIsClass(prhs[0],"char") || options->has_raw_bytes)){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","First input needs to be a string or bytes");   
    }
    
    if (options->has_raw_string){
     	if (!mxIsClass(prhs[0],"char")){
            mexErrMsgIdAndTxt("turtle_json:invalid_input","'raw_string' input needs to be a string");   
        }
        process_input_string(prhs,json_string,string_byte_length,&buffer_added);
    }else if (options->has_raw_bytes){
        process_input_bytes(prhs,json_string,string_byte_length,&buffer_added,&is_input);
    }else{
        //file_path
        read_file_to_string(prhs,json_string,string_byte_length);
    }
    
    setIntScalar(plhs[0],"buffer_added",buffer_added);
    
	//Let's hold onto the string for the user. Technically it isn't needed
    //once we exit this function, since all information is contained in
    //the other variables.
    //
    //We subtract the length of the buffer so that the buffer is not 
    //displayed to the user
    
    //http://stackoverflow.com/questions/19813718/mex-files-how-to-return-an-already-allocated-matlab-array
    if (is_input){
        mxArrayTemp = mxCreateSharedDataCopy(prhs[0]);
        mxSetN(mxArrayTemp,*string_byte_length);
        mxAddField(plhs[0],"json_string");
        mxSetField(plhs[0],0,"json_string",mxArrayTemp);
    }else{
        setStructField(plhs[0],*json_string,"json_string",mxUINT8_CLASS,*string_byte_length);
    }
    
}

void init_options(int nrhs, const mxArray*prhs[],Options *options){
    //
    //  Option parsing
    //  
    //  See Also
    //  --------
    //  json.tokens 
    
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
        //TODO: Should check that the logical value is true ...
        if (mxIsClass(prhs[0],"char")){
            options->has_raw_string = true;
        }else if (mxIsClass(prhs[0],"uint8") || mxIsClass(prhs[0],"int8")){
            //Note, we'll allow bytes for a "raw_string"
            options->has_raw_bytes = true;
        }else{
            //TODO: I also got this when the first input was not a string => e.g. (data,'raw_string',true)
            //TODO: This error message looks wrong 
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
    //  Optional Inputs
    //  ---------------
    //
    //
    //  Outputs:
    //  --------
    //  token_info
    //      - see wrapping Matlab function
    
    //I think it might be better to 
    //make structures that hold info on each thing
    //  - objects
    //  - arrays
    //  etc
//     const char *fieldnames[] = {
//         "buffer_added","json_string","allocation_info","timing_info",
//         "types", "d1","child_count_object","next_sibling_index_object",
//         "object_depths","n_objects_at_depth","object_ids","object_indices"}
    
    TIC(start_mex);
    
    //TODO: Detect the supported features of the processor
    //http://stackoverflow.com/questions/6121792/how-to-check-if-a-cpu-supports-the-sse3-instruction-set
    //https://github.com/JimHokanson/turtle_json/issues/13
    
    size_t string_byte_length;
    unsigned char *json_string = NULL;
    bool is_file_path = true;

    Options options;

    mxLogical raw_is_padded;
    
    plhs[0] = mxCreateStructMatrix(1,1,0,NULL);
    mxArray *timing_info = mxCreateStructMatrix(1, 1, 0, 0);

    //# of inputs check  --------------------------------------------------
    if (!(nrhs == 1 || nrhs == 2)){
        mexErrMsgIdAndTxt("turtle_json:n_inputs","Invalid # of inputs, 1 or 2 expected");
    }else if (!(nlhs == 1)){
        mexErrMsgIdAndTxt("turtle_json:n_inputs","Invalid # of outputs, 1 expected");
    }
    
    //Processing of inputs
    //-------------------------------------
    init_options(nrhs, prhs, &options);
    
    TIC(start_read);
    get_json_string(plhs, nrhs, prhs, &json_string, &string_byte_length, &options);
    TOC(start_read,elapsed_read_time);
    
    //Token parsing
    //-------------
    TIC(start_parse);
    parse_json(json_string, string_byte_length, plhs, &options, timing_info);
    TOC(start_parse, elapsed_parse_time);
    
    //Post token parsing
    post_process(json_string, plhs, timing_info);
    
    TOC(start_mex,total_elapsed_time_mex);

    ADD_STRUCT_FIELD(timing_info,timing_info);
}

