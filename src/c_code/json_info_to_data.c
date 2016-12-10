#include "turtle_json.h"

/*
    mex CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" COPTIMFLAGS="-O3 -DNDEBUG" json_info_to_data.c
    mex CC='/usr/local/Cellar/gcc6/6.1.0/bin/gcc-6' CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" COPTIMFLAGS="-O3 -DNDEBUG" json_info_to_data.c
 */

#define RETRIEVE_DATA_INDEX2(x)\
    data.d1[x]-1

typedef struct {
    uint8_t *types;
    int *d1;

    int *child_count_object;
    int *next_sibling_index_object;
    int *object_ids;
    int *object_indices;
    mxArray *objects;
    
    int *next_sibling_index_key;
    
    int *child_count_array;
    int *next_sibling_index_array;
    uint8_t *array_types;;   

    mxArray *strings;

    double *numeric_data;    
} Data;

mxArray *getStruct(Data data, int object_data_index, int n_objects){
    //
    //  The goal of this function is to allocate memory for the fields
    //  in a structure array
        
    //TODO: Make all of this zero based
    int objects_index = data.object_ids[object_data_index] - 1;
    mxArray *ref_object = mxGetCell(data.objects,objects_index);
    //Copies the field names from one to the other
    mxArray *return_obj = mxDuplicateArray(ref_object);
    
    //Not sure which is better ...
    //int n_fields = data.child_count_object(object_data_index);
    int n_fields  = mxGetNumberOfFields(return_obj);
    
    mxArray **object_data = mxMalloc(n_fields*n_objects*sizeof(mxArray*));        
    mxSetData(return_obj,object_data);
    for (int iObj = 0; iObj < n_objects; iObj++){
        for (int iField = 0; iField < n_fields; iField++){
            mxSetFieldByNumber(return_obj,iObj,iField,0);
        }
    }
    mxSetN(return_obj,n_objects);
    return return_obj;
}

void parseObject(Data data, mxArray *obj, int obj_index, int main_index){
    //
    //  Inputs
    //  ------
    //  s : the structure or structure array
    //  s_index : index in that structure
    //  main_index: 
    //TODO: Pass in a pointer to the object to populate and an index
    
    int object_data_index = RETRIEVE_DATA_INDEX2(main_index);
    int n_keys = data.child_count_object[object_data_index];
    
    mxArray *temp_mxArray;
    int     temp_data_index;;
    
    int cur_key_md_index = main_index + 1;
    int cur_key_data_index = RETRIEVE_DATA_INDEX2(cur_key_md_index);
    for (int iKey = 0; iKey < n_keys; iKey++){
        int cur_key_value_md_index = cur_key_md_index + 1;
        switch (data.types[cur_key_value_md_index]){
            case TYPE_OBJECT:
                temp_data_index = RETRIEVE_DATA_INDEX2(cur_key_value_md_index);
                temp_mxArray = getStruct(data,temp_data_index,1);
                parseObject(data,temp_mxArray,0,cur_key_value_md_index);
                mxSetFieldByNumber(obj,obj_index,iKey,temp_mxArray);
                //getStruct(data, object_data_index,1);
                break;
            case TYPE_ARRAY:
                break;
            case TYPE_KEY:
                mexErrMsgIdAndTxt("turtle_json:code_error","Found key type as child of key");
                break;
            case TYPE_STRING:
                break;
            case TYPE_NUMBER:
                temp_data_index = RETRIEVE_DATA_INDEX2(cur_key_value_md_index);
                temp_mxArray = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,0);
                double *numeric_data = mxGetData(temp_mxArray);
                *numeric_data = data.numeric_data[temp_data_index];
                mxSetFieldByNumber(obj,obj_index,iKey,temp_mxArray);
                break;
            case TYPE_NULL:
                break;
            case TYPE_TRUE:
                break;
            case TYPE_FALSE:
                break;
        }
        cur_key_md_index = data.next_sibling_index_key[cur_key_data_index]-1;
        cur_key_data_index = RETRIEVE_DATA_INDEX2(cur_key_md_index);
    }
    
    //Object
    //------
}

mxArray *parseArray(Data data, int index){
    mexPrintf("Parsing an array\n");
}

uint8_t *getU8Field(const mxArray *s,const char *fieldname){
    mxArray *temp = mxGetField(s,0,fieldname);
    if (temp){
        return (uint8_t *)mxGetData(temp);
    }else{
        mexErrMsgIdAndTxt("turtle_json:field_retrieval","Failed to retrieve field: %s",fieldname);
    }
}

int *getIntField(const mxArray *s,const char *fieldname){
    mxArray *temp = mxGetField(s,0,fieldname);
    if (temp){
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

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    
    const mxArray *s = prhs[0];
    Data data;
    
    //Main Data ----------------------------
    data.types = getU8Field(s, "types");
    data.d1 = getIntField(s,"d1");
    
    //Object Data  -------------------------
    data.child_count_object = getIntField(s,"child_count_object");
    data.next_sibling_index_object = getIntField(s,"next_sibling_index_object");
    data.object_ids = getIntField(s,"object_ids");
    data.object_indices = getIntField(s,"object_indices");
    data.objects = getMXField(s,"objects");
    
    data.next_sibling_index_key = getIntField(s,"next_sibling_index_key");
  
    //Array Data  --------------------------
    data.child_count_array = getIntField(s,"child_count_array");
    data.next_sibling_index_array = getIntField(s,"next_sibling_index_array");
    data.array_types = getU8Field(s,"array_types");
    
    //String Data --------------------------
    data.strings = getMXField(s,"strings");
    
    //Numeric Data -------------------------
    data.numeric_data = (double *)mxGetData(mxGetField(s,0,"numeric_p"));
    
    if (data.types[0] == TYPE_OBJECT){
        int object_data_index = RETRIEVE_DATA_INDEX2(0);
        plhs[0] = getStruct(data, object_data_index,1);
        parseObject(data, plhs[0], 0, 0);
    }else{
        
        //parseArray(0,0);
    }
    
}

