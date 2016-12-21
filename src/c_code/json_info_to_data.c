#include "turtle_json.h"

/*
 * mex CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" COPTIMFLAGS="-O3 -DNDEBUG" json_info_to_data.c
 * mex CC='/usr/local/Cellar/gcc6/6.1.0/bin/gcc-6' CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" COPTIMFLAGS="-O3 -DNDEBUG" json_info_to_data.c
 */

#define MAX_ARRAY_DIMS 10

#define RETRIEVE_DATA_INDEX2(x)\
    data.d1[x]
                
typedef struct {
    uint8_t *types;
    int *d1;
    
    //This is for arrays
    mwSize *dims;
    
    int *child_count_object;
    int *next_sibling_index_object;
    int *object_ids;
    mxArray *objects;
    
    int *next_sibling_index_key;
    
    int *child_count_array;
    int *next_sibling_index_array;
    uint8_t *array_depths;
    uint8_t *array_types;
    
    mxArray *strings;
    
    double *numeric_data;
} Data;

mxArray *parse_array(Data data, int md_index);


void set_double_output(mxArray **s, double value){
    mxArray *temp = mxCreateDoubleMatrix(1,1,0);
    double *data = mxGetData(temp);
    *data = value;
    *s = temp;
}

//http://stackoverflow.com/questions/18847833/is-it-possible-return-cell-array-that-contains-one-instance-in-several-cells
//--------------------------------------------------------------------------
mxArray *mxCreateReference(const mxArray *mx){
    struct mxArray_Tag_Partial *my = (struct mxArray_Tag_Partial *) mx;
    ++my->RefCount;
    return (mxArray *) mx;
}

mxArray *getStruct(Data data, int object_data_index, int n_objects){
    //
    //  The goal of this function is to allocate memory for the fields
    //  in a structure array. We take a reference structure that is empty
    //  but that has its fields defined, and we:
    //  1) duplicate it
    //  2) initialize its data - this part was tricky, since apparently
    //  the "data" of a structure array is an array of mxArrays, 1 for each
    //  field per object.
    //
    //
    
    int object_id = data.object_ids[object_data_index];
    mxArray *ref_object = mxGetCell(data.objects,object_id);
    //Copies the field names from one to the other
    mxArray *return_obj = mxDuplicateArray(ref_object);
    
    //Not sure which is better ...
    //int n_fields = data.child_count_object[object_data_index];
    int n_fields  = mxGetNumberOfFields(return_obj);
    
    //mxArray **object_data = mxMalloc(n_fields*n_objects*sizeof(mxArray*));
    mxArray **object_data = mxCalloc(n_fields*n_objects,sizeof(mxArray*));
    
    mxSetData(return_obj,object_data);
//     for (int iObj = 0; iObj < n_objects; iObj++){
//         for (int iField = 0; iField < n_fields; iField++){
//             mxSetFieldByNumber(return_obj,iObj,iField,0);
//         }
//     }
    
    mxSetN(return_obj,n_objects);
    return return_obj;
}

mxArray *getString(int *d1, mxArray *strings, int md_index){    
    int temp_data_index = RETRIEVE_DATA_INDEX(md_index);
    mxArray *temp_mxArray = mxGetCell(strings,temp_data_index);
    return mxCreateReference(temp_mxArray);
}

// mxArray *getString(Data data, int md_index){    
//     //int temp_data_index = RETRIEVE_DATA_INDEX(md_index);
//     int temp_data_index = RETRIEVE_DATA_INDEX2(md_index);
//     mxArray *temp_mxArray = mxGetCell(data.strings,temp_data_index);
//     return mxCreateReference(temp_mxArray);
// }


mxArray *getNumber(Data data,int md_index){
    int temp_data_index = RETRIEVE_DATA_INDEX2(md_index);
    mxArray *temp_mxArray = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,0);
    double *numeric_data = mxGetData(temp_mxArray);
    *numeric_data = data.numeric_data[temp_data_index];
    return temp_mxArray;
}

mxArray *getNull(Data data,int md_index){
    //TODO: Just increment a reference
    int temp_data_index = RETRIEVE_DATA_INDEX2(md_index);
    mxArray *temp_mxArray = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,0);
    double *numeric_data = mxGetData(temp_mxArray);
    *numeric_data = mxGetNaN();
    return temp_mxArray;
}

mxArray *getTrue(Data data,int md_index){
    //TODO: Just increment a reference
    mxArray *temp_mxArray = mxCreateLogicalMatrix(1,1);
    bool *ldata = mxGetData(temp_mxArray);
    *ldata = 1;
    return temp_mxArray;
}

mxArray *getFalse(Data data,int md_index){
    //TODO: Just increment a reference
    //Default value is false
    return mxCreateLogicalMatrix(1,1);
}

uint8_t *get_u8_field_safe(const mxArray *s,const char *fieldname){
    mxArray *temp = mxGetField(s,0,fieldname);
    if (temp){
        return (uint8_t *)mxGetData(temp);
    }else{
        mexErrMsgIdAndTxt("turtle_json:field_retrieval","Failed to retrieve field: %s",fieldname);
    }
}

int *get_int_field_safe(const mxArray *s,const char *fieldname){
    mxArray *temp = mxGetField(s,0,fieldname);
    if (temp){
        return (int *)mxGetData(temp);
    }else{
        mexErrMsgIdAndTxt("turtle_json:field_retrieval","Failed to retrieve field: %s",fieldname);
    }
}

int *get_int_field_and_length(const mxArray *s,const char *fieldname,int *n_values){
    mxArray *temp = mxGetField(s,0,fieldname);
    if (temp){
        *n_values = mxGetN(temp);
        return (int *)mxGetData(temp);
    }else{
        mexErrMsgIdAndTxt("turtle_json:field_retrieval","Failed to retrieve field: %s",fieldname);
    }
}

mxArray *getMXField(const mxArray *s,const char *fieldname){
    mxArray *temp = mxGetField(s,0,fieldname);
    if (temp){
        return temp;
    }else{
        mexErrMsgIdAndTxt("turtle_json:field_retrieval","Failed to retrieve field: %s",fieldname);
    }
}

//=========================================================================

int index_safely(int *value_array, int n_values, int index){

    if (index < 0){
        mexErrMsgIdAndTxt("turtle_json:indexing","index out of range, less than 0");
    }else if (index >= n_values){
        mexErrMsgIdAndTxt("turtle_json:indexing","index out of range, exceeds # of elements");
    }
    
    return value_array[index];
    
}

mxArray *get_object_ref_from_md_index(const mxArray *mex_input,int md_index){
    
    //  Returns the empty structure that is associated with an object
    //
    //  This empty structure does have key names
    //
    //  TODO: Do we need to return a duplicate?
    //
    //  This is currently being used to get a field number
    
    int n_values;
    int *d1 = get_int_field_and_length(mex_input,"d1",&n_values);
    
    //TODO: verify that md_index is of object type ...
    
    //This comes from the user, let's be careful on indexing
    int object_data_index = index_safely(d1,n_values,md_index);
    
    const mxArray *object_info = mxGetField(mex_input,0,"object_info");
    
    int *object_ids = get_int_field_safe(object_info,"object_ids");
    
    int object_id = object_ids[object_data_index];
    
    const mxArray *objects = getMXField(object_info,"objects");
        
    return mxGetCell(objects,object_id);

}




void populateDims(Data data, int array_data_index, int md_index, int array_depth){
    
    //TODO: We need to populate the dimensions in reverse
    //Start with highest depth and go to 0
    
    int temp_data_index;
    mwSize *dims = data.dims;
    dims[0] = data.child_count_array[array_data_index];
    for (int iDim = 1; iDim < array_depth; iDim++){
        md_index++;
        temp_data_index = RETRIEVE_DATA_INDEX2(md_index);
        dims[iDim] = data.child_count_array[temp_data_index];
    }
    
}


void parse_object(Data data, mxArray *obj, int ouput_struct_index, int md_index){
    //
    //  Inputs
    //  ------
    //  obj : the structure or structure array to populate
    //  ouput_struct_index : index in that structure
    //  md_index: TODO: rename to md_index
    //
    
    int object_data_index = RETRIEVE_DATA_INDEX2(md_index);
    int n_keys = data.child_count_object[object_data_index];
        
    mxArray *temp_mxArray;
    int     temp_data_index;
    
    const int *next_sibling_index_key = data.next_sibling_index_key;
    const uint8_t *types = data.types;
    
    int cur_key_md_index = md_index + 1;
    int cur_key_data_index = RETRIEVE_DATA_INDEX2(cur_key_md_index);
    for (int iKey = 0; iKey < n_keys; iKey++){
        int cur_key_value_md_index = cur_key_md_index + 1;
        switch (types[cur_key_value_md_index]){
            case TYPE_OBJECT:
                temp_data_index = RETRIEVE_DATA_INDEX2(cur_key_value_md_index);
                temp_mxArray = getStruct(data,temp_data_index,1);
                parse_object(data,temp_mxArray,0,cur_key_value_md_index);
                mxSetFieldByNumber(obj,ouput_struct_index,iKey,temp_mxArray);
                break;
            case TYPE_ARRAY:
                mxSetFieldByNumber(obj,ouput_struct_index,iKey,
                        parse_array(data,cur_key_value_md_index));
                break;
            case TYPE_KEY:
                mexErrMsgIdAndTxt("turtle_json:code_error","Found key type as child of key");
                break;
            case TYPE_STRING:
                mxSetFieldByNumber(obj,ouput_struct_index,iKey,getString(data.d1,data.strings,cur_key_value_md_index));
                //mxSetFieldByNumber(obj,ouput_struct_index,iKey,getString(data,cur_key_value_md_index));
                break;
            case TYPE_NUMBER:
                mxSetFieldByNumber(obj,ouput_struct_index,iKey,getNumber(data,cur_key_value_md_index));
                break;
            case TYPE_NULL:
                mxSetFieldByNumber(obj,ouput_struct_index,iKey,getNull(data,cur_key_value_md_index));
                break;
            case TYPE_TRUE:
                mxSetFieldByNumber(obj,ouput_struct_index,iKey,getTrue(data,cur_key_value_md_index));
                break;
            case TYPE_FALSE:
                mxSetFieldByNumber(obj,ouput_struct_index,iKey,getFalse(data,cur_key_value_md_index));
                break;
        }
        cur_key_md_index = next_sibling_index_key[cur_key_data_index];
        cur_key_data_index = RETRIEVE_DATA_INDEX2(cur_key_md_index);
    }
}

void parse_cellstr(mxArray **output, int *d1, int array_size, int array_md_index, mxArray *strings){
    mxArray *p_output = mxCreateCellMatrix(1,array_size);
    int temp_md_index = array_md_index + 1;
    for (int iData = 0; iData < array_size; iData++){
        mxSetCell(p_output,iData,getString(d1,strings,temp_md_index));
        temp_md_index++;
    }
    *output = p_output;
}

mxArray* parse_1d_array(int *d1, double *numeric_data, int array_size, int array_md_index){
            
    int first_numeric_md_index = array_md_index;
    int first_numeric_data_index = RETRIEVE_DATA_INDEX(first_numeric_md_index);
    
    mxArray *output = mxCreateNumericMatrix(1,0,mxDOUBLE_CLASS,0);

    double *temp_value = mxMalloc(array_size*sizeof(double));
    memcpy(temp_value,&numeric_data[first_numeric_data_index],array_size*sizeof(double));
    mxSetData(output,temp_value);
	mxSetN(output,array_size);
    
    return output;
}

mxArray *parse_array(Data data, int md_index){
    
    int cur_array_data_index = RETRIEVE_DATA_INDEX2(md_index);
        
    int temp_count;
    int temp_data_index;
    int temp_md_index;
    int temp_array_depth;
    mwSize *dims;
    double *temp_value;
    mxArray *output;
    mxArray *temp_obj;
    
    switch (data.array_types[cur_array_data_index]){
        case ARRAY_OTHER_TYPE:
            //This is the "messiest" option of all. Since we need to go through item
            //by item and parse the result ...
            temp_count = data.child_count_array[cur_array_data_index];
            output = mxCreateCellMatrix(1,temp_count);
            
            temp_md_index = md_index + 1;
            for (int iData = 0; iData < temp_count; iData++){
                switch (data.types[temp_md_index]){
                    case TYPE_OBJECT:
                        temp_data_index = RETRIEVE_DATA_INDEX2(temp_md_index);
                        temp_obj = getStruct(data,temp_data_index,1);
                        parse_object(data, temp_obj, 0, temp_md_index);
                        mxSetCell(output,iData,temp_obj);
                        temp_md_index = data.next_sibling_index_object[temp_data_index];
                        break;
                    case TYPE_ARRAY:
                        temp_data_index = RETRIEVE_DATA_INDEX2(temp_md_index);
                        mxSetCell(output,iData,parse_array(data,temp_md_index));
                        temp_md_index = data.next_sibling_index_array[temp_data_index];
                        break;
                    case TYPE_KEY:
                        mexErrMsgIdAndTxt("turtle_json:code_error","Found key type as child of array");
                        break;
                    case TYPE_STRING:
                        mxSetCell(output,iData,getString(data.d1,data.strings,temp_md_index));
                        //mxSetCell(output,iData,getString(data,temp_md_index));
                        temp_md_index++;
                        break;
                    case TYPE_NUMBER:
                        mxSetCell(output,iData,getNumber(data,temp_md_index));
                        temp_md_index++;
                        break;
                    case TYPE_NULL:
                        mxSetCell(output,iData,getNull(data,temp_md_index));
                        temp_md_index++;
                        break;
                    case TYPE_TRUE:
                        mxSetCell(output,iData,getTrue(data,temp_md_index));
                        temp_md_index++;
                        break;
                    case TYPE_FALSE:
                        mxSetCell(output,iData,getFalse(data,temp_md_index));
                        temp_md_index++;
                        break;
                }
            }
            break;
        case ARRAY_NUMERIC_TYPE:
            output = parse_1d_array(data.d1,data.numeric_data,
                    data.child_count_array[cur_array_data_index],md_index);
            break;
        case ARRAY_STRING_TYPE:
            //
            //  This is a cell array of strings => {'as','df','cheese'}
            
            parse_cellstr(&output,data.d1,data.child_count_array[cur_array_data_index],md_index,data.strings);
            break;
        case ARRAY_LOGICAL_TYPE:
            output = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,0);
            temp_value = mxGetData(output);
            *temp_value = 4;
            break;
        case ARRAY_OBJECT_SAME_TYPE:
            temp_md_index = md_index + 1;
            temp_data_index = RETRIEVE_DATA_INDEX2(md_index+1);
            temp_count = data.child_count_array[cur_array_data_index];
            output = getStruct(data,temp_data_index,temp_count);
            for (int iObj = 0; iObj < temp_count; iObj++){
                parse_object(data, output, iObj, temp_md_index);
                temp_md_index = data.next_sibling_index_object[temp_data_index];
                temp_data_index = RETRIEVE_DATA_INDEX2(temp_md_index);
            }
            break;
        case ARRAY_OBJECT_DIFF_TYPE:
            temp_count = data.child_count_array[cur_array_data_index];
            output = mxCreateCellMatrix(1,temp_count);
            temp_md_index = md_index + 1;
            for (int iData = 0; iData < temp_count; iData++){
                temp_data_index = RETRIEVE_DATA_INDEX2(temp_md_index);
                temp_obj = getStruct(data,temp_data_index,1);
                parse_object(data, temp_obj, 0, temp_md_index);
                mxSetCell(output,iData,temp_obj);
                temp_md_index = data.next_sibling_index_object[temp_data_index];
            }
            break;
        case ARRAY_ND_NUMERIC:
            temp_array_depth = data.array_depths[cur_array_data_index];
            if (temp_array_depth > MAX_ARRAY_DIMS){
                mexErrMsgIdAndTxt("turtle_json:max_nd_array_depth","The maximum nd array depth depth was exceeded");
            }
            
            //-1 for previous value before the next sibling
            temp_md_index = data.next_sibling_index_array[cur_array_data_index]-1;
            
            //We have nested arrays
            //  [  [  [   #
            //  0  1  2   3   <= indices past md_index
            //                Note, the depth is also 3
            
            temp_count = data.d1[temp_md_index] - data.d1[md_index + temp_array_depth] + 1;
            
//             mexPrintf("temp_md_index: %d\n",temp_md_index);
//             mexPrintf("data.d1: %d\n",data.d1[md_index + temp_array_depth]);
//             mexPrintf("data.d2: %d\n",data.d1[temp_md_index]);
//             mexPrintf("Count: %d\n",temp_count);
//             mexErrMsgIdAndTxt("turtle_json:wtf","The maximum was exceeded");
            
            temp_value = mxMalloc(temp_count*sizeof(double));
            
            temp_data_index = RETRIEVE_DATA_INDEX2((md_index + temp_array_depth));
            
            memcpy(temp_value,&data.numeric_data[temp_data_index],temp_count*sizeof(double));
            
            populateDims(data,cur_array_data_index,md_index,temp_array_depth);
            
            output = mxCreateNumericArray(0,0,mxDOUBLE_CLASS,0);
            mxSetData(output,temp_value);
            mxSetDimensions(output,data.dims,temp_array_depth);
            break;
        case ARRAY_ND_STRING:
            output = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,0);
            temp_value = mxGetData(output);
            *temp_value = 7;
            break;
        case ARRAY_ND_LOGICAL:
            output = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,0);
            temp_value = mxGetData(output);
            *temp_value = 8;
            break;
            
    }
    return output;
}
//=========================================================================
//=========================================================================
//=========================================================================

//0 =======================================================================
void f0__full_parse(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[]){
    
    //
    //  
    //
    //  TODO: We could add on options here eventually
    //
    
    if (nrhs != 3){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","json_info_to_data.f0 requires 3 inputs");
    }else if (!mxIsClass(prhs[1],"struct")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","2nd input to json_info_to_data.f0 needs to be a structure");
    }else if (!mxIsClass(prhs[2],"double")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","3rd input to json_info_to_data.f0 needs to be a double");
    }
    
    if (nlhs != 1){
    	mexErrMsgIdAndTxt("turtle_json:invalid_output","json_info_to_data.f0 requires 1 output");
    }
    
    const mxArray *s = prhs[1];
    int md_index = ((int) mxGetScalar(prhs[2]))-1;
    
    Data data;
    
    //Main Data -----------------------------------------------------------
    int n_values;
    data.types = get_u8_field_safe(s, "types");
    data.d1 = get_int_field_and_length(s,"d1",&n_values);
    
    //Object Data  --------------------------------------------------------
    const mxArray *object_info = mxGetField(s,0,"object_info");
    data.child_count_object = get_int_field_safe(object_info,"child_count_object");
    data.next_sibling_index_object = get_int_field_safe(object_info,"next_sibling_index_object");
    data.object_ids = get_int_field_safe(object_info,"object_ids");
    data.objects = getMXField(object_info,"objects");
    
    const mxArray *key_info = mxGetField(s,0,"key_info");
    data.next_sibling_index_key = get_int_field_safe(key_info,"next_sibling_index_key");
    
    //Array Data  ---------------------------------------------------------
    const mxArray *array_info = mxGetField(s,0,"array_info");
    data.child_count_array = get_int_field_safe(array_info,"child_count_array");
    data.next_sibling_index_array = get_int_field_safe(array_info,"next_sibling_index_array");
    data.array_types = get_u8_field_safe(array_info,"array_types");
    data.array_depths = get_u8_field_safe(array_info,"array_depths");
    
    //String Data --------------------------
    data.strings = getMXField(s,"strings");
    
    data.dims = mxMalloc(MAX_ARRAY_DIMS*sizeof(mwSize));
    
    //Numeric Data -------------------------
    data.numeric_data = (double *)mxGetData(mxGetField(s,0,"numeric_p"));
    
    if (md_index < 0 || md_index >= n_values){
        mexErrMsgIdAndTxt("turtle_json:invalid_output","md_index out of range for f0");
    }
    
    if (data.types[md_index] == TYPE_KEY){
        //We'll return what the key points to
        ++md_index;
    }
    
    int data_index;
    switch (data.types[md_index]){
        case TYPE_OBJECT:
            data_index = RETRIEVE_DATA_INDEX2(md_index);
            plhs[0] = getStruct(data, data_index,1);
            parse_object(data, plhs[0], 0, md_index);
            break;
        case TYPE_ARRAY:
            data_index = RETRIEVE_DATA_INDEX2(0);
            plhs[0] = parse_array(data, md_index);
            break;
        case TYPE_KEY:
            //We should already check against this ...
            mexErrMsgIdAndTxt("turtle_json:code_error","key child of key found, code error made somewhere");
            break;
        case TYPE_STRING:
            plhs[0] = getString(data.d1,data.strings,md_index);
            //plhs[0] = getString(data,md_index);
            break;
        case TYPE_NUMBER:
            plhs[0] = getNumber(data,md_index);
            break;
        case TYPE_NULL:
            plhs[0] = getNull(data,md_index);
            break;
        case TYPE_TRUE:
            plhs[0] = getTrue(data,md_index);
            break;
        case TYPE_FALSE:
            plhs[0] = getFalse(data,md_index);
            break;
    }
}

//=========================================================================
void f1__get_key_index(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[]){
    //
    //  Given an object and a key name, get the key index. We also check
    //  that the key exists.
    //
    //  index_1b = json_info_to_data(1,mex_struct,obj_md_index,key_name)
    
    if (nrhs != 4){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","json_info_to_data.getKeyIndex requires at four inputs");
    }else if (!mxIsClass(prhs[1],"struct")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","2nd input to json_info_to_data.getKeyIndex needs to be a structure");
    }else if (!mxIsClass(prhs[2],"double")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","3rd input to json_info_to_data.getKeyIndex needs to be a double");
    }else if (!mxIsClass(prhs[3],"char")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","4th input to json_info_to_data.getKeyIndex needs to be a char");
    }
    
    if (nlhs != 1){
    	mexErrMsgIdAndTxt("turtle_json:invalid_output","json_info_to_data.f1 requires 1 output");
    }
    
    //-1 from 1b to 0b
    int md_index = ((int)mxGetScalar(prhs[2])) - 1;
    mxArray *s = get_object_ref_from_md_index(prhs[1],md_index);   
    
    char *field_name = mxArrayToString(prhs[3]);
    //+1 to make 1b
    int index_1b = mxGetFieldNumber(s,field_name)+1;
    mxFree(field_name); 
    
    if (index_1b == 0){
        mexErrMsgIdAndTxt("turtle_json:invalid_key_for_object","the specified key is not a member of the specified object");
    }
    
    set_double_output(&plhs[0],(double)index_1b);       
}

//=========================================================================
void f2__get_key_value_type_and_index(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[]){
    //
    //  2: key_index to (type,md_index_1b)
    //      [type,md_index_1b] = json_info_to_data(2,mex_struct,obj_md_index,key_index_1b
    //
    if (nrhs != 4){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","json_info_to_data.f2 requires 4 inputs");
    }else if (!mxIsClass(prhs[1],"struct")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","2nd input to json_info_to_data.f2 needs to be a structure");
    }else if (!mxIsClass(prhs[2],"double")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","3rd input to json_info_to_data.f2 needs to be a double");
    }else if (!mxIsClass(prhs[3],"double")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","4th input to json_info_to_data.f2 needs to be a double");
    }
    
    if (nlhs != 2){
    	mexErrMsgIdAndTxt("turtle_json:invalid_output","json_info_to_data.getKeyIndex requires 2 output");
    }
    
    //Algorithm get object_info
    //Check # of keys
    //Walk along keys until specified value
    //Return results
    int n_values;
    const mxArray *mex_input = prhs[1];
    int *d1 = get_int_field_and_length(mex_input,"d1",&n_values);
    uint8_t *types = get_u8_field_safe(mex_input,"types");
        
    int md_index = ((int)mxGetScalar(prhs[2]))-1;
    int object_data_index = index_safely(d1,n_values,md_index);
    
    //TODO: 
    const mxArray *object_info = mxGetField(mex_input,0,"object_info");
    
    int *object_counts = get_int_field_and_length(object_info,"child_count_object",&n_values);
    
    int n_keys = index_safely(object_counts,n_values,object_data_index);
    
    int key_index = ((int)mxGetScalar(prhs[3]))-1;
    
    if (key_index < 0 || key_index >= n_keys){
        mexErrMsgIdAndTxt("turtle_json:f2:invalid_key_index","key index is out of range");
    }
    
    mxArray *key_info = mxGetField(mex_input,0,"key_info");
    int *next_sibling_index_key = get_int_field_safe(key_info,"next_sibling_index_key");
    
    //-----------------------------------------------
    
    
    int next_md_index;
    int cur_key_data_index;
    
    //Stopping condition
    //------------------
    //key_index 
    //0 - skip loop
    //1 - only run first iteration of loop (so while < key_index)
    
    //Advance from object to the 1st key
    md_index++;
    for (int iKey = 0; iKey < key_index; iKey++){
        cur_key_data_index = d1[md_index];
        md_index = next_sibling_index_key[cur_key_data_index];
    }
    
    //md_index should now point to the key, but we want the value
    //advancing to value
    md_index++;
    
    set_double_output(&plhs[0],(double)types[md_index]);   
    //Making the index 1 based
    set_double_output(&plhs[1],(double)(md_index+1)); 
}

void f3__get_cellstr(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[]){
    
    if (nrhs != 3){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","json_info_to_data.f3 requires 3 inputs");
    }else if (!mxIsClass(prhs[1],"struct")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","2nd input to json_info_to_data.f3 needs to be a structure");
    }else if (!mxIsClass(prhs[2],"double")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","3rd input to json_info_to_data.f3 needs to be a double");
    }
    
    if (nlhs != 1){
    	mexErrMsgIdAndTxt("turtle_json:invalid_output","json_info_to_data.f3 requires 1 output");
    }

    int n_values;
    const mxArray *mex_input = prhs[1];
    int *d1 = get_int_field_and_length(mex_input,"d1",&n_values);
    uint8_t *types = get_u8_field_safe(mex_input,"types");
        
    int array_md_index = ((int)mxGetScalar(prhs[2]))-1;
    int cur_array_data_index = index_safely(d1,n_values,array_md_index);   
    
    if (!(types[array_md_index] == TYPE_ARRAY)){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","Requested cellstr, but index did not point to an array");
    }
    
    const mxArray *array_info = mxGetField(mex_input,0,"array_info");
    const int *child_count_array = get_int_field_safe(array_info,"child_count_array");
    //data.child_count_array = get_int_field_safe(array_info,"child_count_array");
    //data.next_sibling_index_array = get_int_field_safe(array_info,"next_sibling_index_array");
    uint8_t *array_types = get_u8_field_safe(array_info,"array_types");
    
    if (!(array_types[cur_array_data_index] == ARRAY_STRING_TYPE)){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","Requested cellstr, but array was not a cellstr");
    }
        
    parse_cellstr(&plhs[0],d1,child_count_array[cur_array_data_index], 
            array_md_index,getMXField(mex_input,"strings"));
    
}

void f4__get_nd_array(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[]){
    
    if (nrhs != 3){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","json_info_to_data.f4 requires 3 inputs");
    }else if (!mxIsClass(prhs[1],"struct")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","2nd input to json_info_to_data.f4 needs to be a structure");
    }else if (!mxIsClass(prhs[2],"double")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","3rd input to json_info_to_data.f4 needs to be a double");
    }else if (!mxIsClass(prhs[3],"double")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","3rd input to json_info_to_data.f4 needs to be a double");
    }
    
    if (nlhs != 1){
    	mexErrMsgIdAndTxt("turtle_json:invalid_output","json_info_to_data.f4 requires 1 output");
    }

    int n_values;
    const mxArray *mex_input = prhs[1];
    int *d1 = get_int_field_and_length(mex_input,"d1",&n_values);
    uint8_t *types = get_u8_field_safe(mex_input,"types");
        
    int array_md_index = ((int)mxGetScalar(prhs[2]))-1;
    uint8_t expected_array_type = ((uint8_t)mxGetScalar(prhs[3]));
    
    
    int cur_array_data_index = index_safely(d1,n_values,array_md_index);   
    
    if (!(types[array_md_index] == TYPE_ARRAY)){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","Requested nd-array, but index did not point to an array");
    }
    
    const mxArray *array_info = mxGetField(mex_input,0,"array_info");
    uint8_t *array_types = get_u8_field_safe(array_info,"array_types");
    
    uint8_t observed_array_type = array_types[cur_array_data_index];
    
    switch (observed_array_type){
        case ARRAY_OTHER_TYPE:
            mexErrMsgIdAndTxt("turtle_json:invalid_input","nd_array option not support for non nd-array");
            break;
        case ARRAY_NUMERIC_TYPE:
            if (expected_array_type == 0){
             //Yay!   
            }else{
                mexErrMsgIdAndTxt("turtle_json:invalid_input","nd_array numeric observed but not expected");
            }
            break;
        case ARRAY_STRING_TYPE:
         	if (expected_array_type == 1){
             //Yay!   
            }else{
                mexErrMsgIdAndTxt("turtle_json:invalid_input","nd_array string observed but not expected");
            }
            break;
        case ARRAY_LOGICAL_TYPE:
            if (expected_array_type == 2){
             //Yay!   
            }else{
                mexErrMsgIdAndTxt("turtle_json:invalid_input","nd_array logical observed but not expected");
            }
            break;
        case ARRAY_OBJECT_SAME_TYPE:
            mexErrMsgIdAndTxt("turtle_json:invalid_input","nd_array option not support for non nd-array");
            break;
        case ARRAY_OBJECT_DIFF_TYPE:
            mexErrMsgIdAndTxt("turtle_json:invalid_input","nd_array option not support for non nd-array");
            break;
        case ARRAY_ND_NUMERIC:
           	if (expected_array_type == 0){
             //Yay!   
            }else{
                mexErrMsgIdAndTxt("turtle_json:invalid_input","nd_array numeric observed but not expected");
            }
            break;
        case ARRAY_ND_STRING:
          	if (expected_array_type == 1){
             //Yay!   
            }else{
                mexErrMsgIdAndTxt("turtle_json:invalid_input","nd_array string observed but not expected");
            }
            break;
        case ARRAY_ND_LOGICAL:
         	if (expected_array_type == 2){
             //Yay!   
            }else{
                mexErrMsgIdAndTxt("turtle_json:invalid_input","nd_array logical observed but not expected");
            }
            break;
    }
        
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    
    //  Calling Forms
    //  ----------------------------------------------
    //  0: full parsing
    //      data = json_info_to_data(0,mex_struct,start_index)
    //  1: object key name to key_index (in struct)
    //      index_1b = json_info_to_data(1,mex_struct,obj_md_index,key_name)
    //  2: key_index to (type,md_index_1b)
    //      [type,md_index_1b] = json_info_to_data(2,mex_struct,obj_md_index,key_index_1b)
    //
    //  3: Retrieve cellstr
    //      cellstr = json_info_to_data(3,mex_struct,array_md_index)
    //  4: Retrieve nd-array
    //      nd_arrray = json_info_to_data(4,mex_struct,array_md_index,expected_array_type)
    //      expected_array_type:
    //          0 - numeric
    //          1 - string
    //          2 - logical
    //
    //
    //
    //  json_info_to_data(mex_struct)
    //
    //  Inputs
    //  ------
    //  mex_struct : output structure from turtle_json
    
    if (nrhs < 1){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","json_info_to_data.mex requires at least one input");
    }
    
    //Ideally we could allow any number, but Matlab tends to use doubles
    if (!mxIsClass(prhs[0],"double")){
        mexErrMsgIdAndTxt("turtle_json:invalid_input","First input needs to be a double");
    }
    
    switch ((int)mxGetScalar(prhs[0])){
        case 0:
            f0__full_parse(nlhs,plhs,nrhs,prhs);
            break;
        case 1:
            f1__get_key_index(nlhs,plhs,nrhs,prhs);
            break;
        case 2:
            f2__get_key_value_type_and_index(nlhs,plhs,nrhs,prhs);
            break;
      	case 3:
            f3__get_cellstr(nlhs,plhs,nrhs,prhs);
            break;
//         case 4:
//             f4__get_nd_array(nlhs,plhs,nrhs,prhs);
//             break;
        default:
            mexErrMsgIdAndTxt("turtle_json:invalid_input","Function option not recognized");
            
    }
}
