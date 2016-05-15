
void parse_numbers(unsigned char *js, mxArray *plhs[]) {

    string_p = mxRealloc(string_p,(current_string_index + 1)*sizeof(unsigned char *));
    setStructField(plhs[0],string_p,"string_p",mxUINT64_CLASS,current_string_index + 1);
    
    numeric_p = mxRealloc(numeric_p,(current_numeric_index + 1)*sizeof(unsigned char *));
    setStructField(plhs[0],numeric_p,"numeric_p",mxUINT64_CLASS,current_numeric_index + 1);
    
    //mxArray temp = mxGetField(plhs[0],"numeric_p",
    
    
    double *numeric_data = mxMalloc((current_numeric_index + 1)*sizeof(double));
    
    #pragma omp parallel num_threads(4)
    {
        int tid = omp_get_thread_num();
        for (int i = tid; i <= current_numeric_index; i += 4){
            numeric_data[i] = i;
        }
    }
    
    setStructField(plhs[0],numeric_data,"numeric_data",mxDOUBLE_CLASS,current_numeric_index + 1);    

}