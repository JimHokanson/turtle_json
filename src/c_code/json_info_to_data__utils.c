#include "turtle_json.h"
#include "json_info_to_data.h"

//  Name: json_info_to_data__utils.c

//Scalar retrieval
//=========================================================================
//TODO: rename to correct casing
mxArray* getString(int *d1, mxArray *strings, int md_index){    
    int temp_data_index = RETRIEVE_DATA_INDEX(md_index);
    mxArray *temp_mxArray = mxGetCell(strings,temp_data_index);
    return mxCreateReference(temp_mxArray);
}
mxArray* getNumber(Data data, int md_index){
    int temp_data_index = data.d1[md_index];
    mxArray *temp_mxArray = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,0);
    double *numeric_data = mxGetData(temp_mxArray);
    *numeric_data = data.numeric_data[temp_data_index];
    return temp_mxArray;
}
mxArray* getNull(Data data, int md_index){
    //TODO: Just increment a reference
    int temp_data_index = data.d1[md_index];
    mxArray *temp_mxArray = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,0);
    double *numeric_data = mxGetData(temp_mxArray);
    *numeric_data = mxGetNaN();
    return temp_mxArray;
}
mxArray* getTrue(Data data, int md_index){
    //TODO: Just increment a reference
    mxArray *temp_mxArray = mxCreateLogicalMatrix(1,1);
    bool *ldata = mxGetData(temp_mxArray);
    *ldata = 1;
    return temp_mxArray;
}
mxArray* getFalse(Data data, int md_index){
    //TODO: Just increment a reference
    //Default value is false
    return mxCreateLogicalMatrix(1,1);
}

//Field retrieval with checks on existence 
//=========================================================================
uint8_t* get_u8_field_safe(const mxArray *s,const char *fieldname){
    mxArray *temp = mxGetField(s,0,fieldname);
    if (temp){
        return (uint8_t *)mxGetData(temp);
    }else{
        mexErrMsgIdAndTxt("turtle_json:field_retrieval",
                "Failed to retrieve field: %s",fieldname);
    }
}
int* get_int_field_safe(const mxArray *s,const char *fieldname){
    mxArray *temp = mxGetField(s,0,fieldname);
    if (temp){
        return (int *)mxGetData(temp);
    }else{
        mexErrMsgIdAndTxt("turtle_json:field_retrieval",
                "Failed to retrieve field: %s",fieldname);
    }
}
int* get_int_field_and_length_safe(const mxArray *s,const char *fieldname,int *n_values){
    //
    //  Example
    //  -----------------
    //  int n_values;
    //  int *d1 = get_int_field_and_length_safe(mex_input,"d1",&n_values);
    
    mxArray *temp = mxGetField(s,0,fieldname);
    if (temp){
        *n_values = mxGetN(temp);
        return (int *)mxGetData(temp);
    }else{
        mexErrMsgIdAndTxt("turtle_json:field_retrieval",
                "Failed to retrieve field: %s",fieldname);
    }
}
mxArray* get_mx_field_safe(const mxArray *s,const char *fieldname){
    mxArray *temp = mxGetField(s,0,fieldname);
    if (temp){
        return temp;
    }else{
        mexErrMsgIdAndTxt("turtle_json:field_retrieval",
                "Failed to retrieve field: %s",fieldname);
    }
}
//=========================================================================





void set_double_output(mxArray **s, double value){
    mxArray *temp = mxCreateDoubleMatrix(1,1,0);
    double *data = mxGetData(temp);
    *data = value;
    *s = temp;
}

//http://stackoverflow.com/questions/18847833/is-it-possible-return-cell-array-that-contains-one-instance-in-several-cells
//--------------------------------------------------------------------------

int ref_offset = -1;

mxArray* mxCreateReference(const mxArray *mx){
    //return mxDuplicateArray(mx);
    //return mxCreateSharedDataCopy(mx);
    #ifdef ALLOW_REF_COUNT
        if (ref_offset == -1){
            //Grabs output of version() e.g. 9.9.0.15 etc.
            //and translates into 909 - we add a 0 because we would want
            //9.12 to be 912 and newer/higher than 9.9
            mxArray *version = mxCreateNumericMatrix(1,100,mxUINT8_CLASS,mxREAL);
            mexCallMATLAB(1,&version,0, NULL, "version");
            char *str = mxArrayToString(version);
            float temp = strtof(str,NULL);
            int major_ver = (int)temp;
            int minor_ver = (int)roundf((temp-(float)major_ver)*10);
            int version_id = major_ver*100 + minor_ver;

            //907 -> 2019b
            if (version_id < 907){
                ref_offset = 4;
            }else{
                ref_offset = 3;
            }
        }

        mwSize *ref_count = ((mwSize *) mx) + ref_offset; 
        (*ref_count)++;

        //struct mxArray_Tag_Partial *my = (struct mxArray_Tag_Partial *) mx;
        //++my->RefCount;
        return (mxArray *) mx;
    #else
        return mxDuplicateArray(mx);
    #endif
}


//TODO: I am phasing this out ...
int index_safely(int *value_array, int n_values, int index){
    if (index < 0){
        mexErrMsgIdAndTxt("turtle_json:indexing","index out of range, less than 0");
    }else if (index >= n_values){
        mexErrMsgIdAndTxt("turtle_json:indexing","index out of range, exceeds # of elements");
    }
    return value_array[index];
}


