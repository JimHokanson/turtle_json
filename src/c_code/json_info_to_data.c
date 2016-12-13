#include "turtle_json.h"

/*
    mex CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" COPTIMFLAGS="-O3 -DNDEBUG" json_info_to_data.c
    mex CC='/usr/local/Cellar/gcc6/6.1.0/bin/gcc-6' CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" COPTIMFLAGS="-O3 -DNDEBUG" json_info_to_data.c
 */

#define MAX_ARRAY_DIMS 10

#define RETRIEVE_DATA_INDEX2(x)\
    data.d1[x]

typedef struct {
    uint8_t *types;
    int *d1;

    mwSize *dims;
    
    int *child_count_object;
    int *next_sibling_index_object;
    int *object_ids;
    int *object_indices;
    mxArray *objects;
    
    int *next_sibling_index_key;
    
    int *child_count_array;
    int *next_sibling_index_array;
    uint8_t *array_depths;
    uint8_t *array_types; 

    mxArray *strings;

    double *numeric_data;    
} Data;

mxArray *parseArray(Data data, int md_index);

//http://stackoverflow.com/questions/18847833/is-it-possible-return-cell-array-that-contains-one-instance-in-several-cells
//--------------------------------------------------------------------------

mxArray *mxCreateReference(const mxArray *mx)
{
    struct mxArray_Tag_Partial *my = (struct mxArray_Tag_Partial *) mx;
    ++my->RefCount;
    return (mxArray *) mx;
}


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

mxArray *getString(Data data,int md_index){
	int temp_data_index = RETRIEVE_DATA_INDEX2(md_index);
	mxArray *temp_mxArray = mxGetCell(data.strings,temp_data_index);
    return mxCreateReference(temp_mxArray);    
}

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


void parseObject(Data data, mxArray *obj, int obj_index, int md_index){
    //
    //  Inputs
    //  ------
    //  s : the structure or structure array
    //  s_index : index in that structure
    //  main_index: TODO: rename to md_index
    //
    //  TODO: A lot of these variables need to be renamed
    
    int object_data_index = RETRIEVE_DATA_INDEX2(md_index);
    int n_keys = data.child_count_object[object_data_index];
    
//     mexPrintf("object_data_index: %d\n",object_data_index);
//     
//     if (data.types[md_index] != TYPE_OBJECT){
//         mexPrintf("BAD =====\n");
//         mexPrintf("Type: %d\n",data.types[md_index]);
//         mexPrintf("Cur obj md_index: %d\n",md_index);
//         mexErrMsgIdAndTxt("turtle_json:code_error","Bad md_index, doesn't point to object");
//     }
    
    mxArray *temp_mxArray;
    int     temp_data_index;;
    
    int cur_key_md_index = md_index + 1;
    int cur_key_data_index = RETRIEVE_DATA_INDEX2(cur_key_md_index);
    for (int iKey = 0; iKey < n_keys; iKey++){
// //         mexPrintf("Cur Key 2: %d\n",iKey);
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
                mxSetFieldByNumber(obj,obj_index,iKey,
                    parseArray(data,cur_key_value_md_index));
                break;
            case TYPE_KEY:
//                 mexPrintf("object_data_index: %d\n",object_data_index);
//                 mexPrintf("obj_index (struct array index): %d\n",obj_index);
//                 mexPrintf("Cur field name: %s\n",mxGetFieldNameByNumber(obj,iKey));
//                 mexPrintf("Cur obj md_index: %d\n",md_index);
//                 mexPrintf("Cur Key: %d\n",iKey);
//                 mexPrintf("Cur_key_md_index: %d\n",cur_key_md_index);
                mexErrMsgIdAndTxt("turtle_json:code_error","Found key type as child of key");
                break;
            case TYPE_STRING:
                mxSetFieldByNumber(obj,obj_index,iKey,getString(data,cur_key_value_md_index));
                break;
            case TYPE_NUMBER:
                mxSetFieldByNumber(obj,obj_index,iKey,getNumber(data,cur_key_value_md_index));
                break;
            case TYPE_NULL:
             	mxSetFieldByNumber(obj,obj_index,iKey,getNull(data,cur_key_value_md_index));
                break;
            case TYPE_TRUE:
                mxSetFieldByNumber(obj,obj_index,iKey,getTrue(data,cur_key_value_md_index));
                break;
            case TYPE_FALSE:
                mxSetFieldByNumber(obj,obj_index,iKey,getFalse(data,cur_key_value_md_index));
                break;
        }
        cur_key_md_index = data.next_sibling_index_key[cur_key_data_index];
        cur_key_data_index = RETRIEVE_DATA_INDEX2(cur_key_md_index);
    }
}

mxArray *parseArray(Data data, int md_index){
    
    int cur_array_data_index = RETRIEVE_DATA_INDEX2(md_index);
    
//         int *child_count_array;
//     int *next_sibling_index_array;
//     uint8_t *array_types; 

// #define ARRAY_OTHER_TYPE   0
// #define ARRAY_NUMERIC_TYPE 1
// #define ARRAY_STRING_TYPE  2
// #define ARRAY_LOGICAL_TYPE 3
// #define ARRAY_OBJECT_SAME_TYPE  4
// #define ARRAY_OBJECT_DIFF_TYPE 5
// #define ARRAY_ND_NUMERIC 6
// #define ARRAY_ND_STRING 7
// #define ARRAY_ND_LOGICAL 8
    
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
            //This is the "messiest" option of all.
            temp_count = data.child_count_array[cur_array_data_index];
            output = mxCreateCellMatrix(1,temp_count);
        
            //mexPrintf("Running array other type\n");
            temp_md_index = md_index + 1;    
            for (int iData = 0; iData < temp_count; iData++){
                switch (data.types[temp_md_index]){
                    case TYPE_OBJECT:
                        temp_data_index = RETRIEVE_DATA_INDEX2(temp_md_index);
                        temp_obj = getStruct(data,temp_data_index,1);
                        parseObject(data, temp_obj, 0, temp_md_index);
                        mxSetCell(output,iData,temp_obj);
                        temp_md_index = data.next_sibling_index_object[temp_data_index];
                        break;
                    case TYPE_ARRAY:
                        temp_data_index = RETRIEVE_DATA_INDEX2(temp_md_index);
                        mxSetCell(output,iData,parseArray(data,temp_md_index));
                        temp_md_index = data.next_sibling_index_array[temp_data_index];
                        break;
                    case TYPE_KEY:
                      	mexErrMsgIdAndTxt("turtle_json:code_error","Found key type as child of array");
                        break;
                    case TYPE_STRING:
                        mxSetCell(output,iData,getString(data,temp_md_index));
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
            //
            //  In this case, we have an array of numbers => [1,2,3,4]
            temp_count = data.child_count_array[cur_array_data_index];
            
            temp_md_index = md_index + 1;
            temp_data_index = RETRIEVE_DATA_INDEX2(md_index+1);
            
            output = mxCreateNumericMatrix(1,0,mxDOUBLE_CLASS,0);
            temp_value = mxMalloc(temp_count*sizeof(double));
            memcpy(temp_value,&data.numeric_data[temp_data_index],temp_count*sizeof(double));
            mxSetData(output,temp_value);
            mxSetN(output,temp_count);
            break;
        case ARRAY_STRING_TYPE:
            //
            //  This is a cell array of strings => {'as','df','cheese'}
            //
            temp_count = data.child_count_array[cur_array_data_index];
            output = mxCreateCellMatrix(1,temp_count);
            temp_md_index = md_index + 1;    
            for (int iData = 0; iData < temp_count; iData++){
                mxSetCell(output,iData,getString(data,temp_md_index));
                temp_md_index++;
            }
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
                parseObject(data, output, iObj, temp_md_index); 
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
                parseObject(data, temp_obj, 0, temp_md_index);
                mxSetCell(output,iData,temp_obj);
                temp_md_index = data.next_sibling_index_object[temp_data_index];
            }
            break;
        case ARRAY_ND_NUMERIC:
            temp_array_depth = data.array_depths[cur_array_data_index];
//             mexPrintf("CA: %d\n",cur_array_data_index);
//             mexPrintf("Depth: %d\n",temp_array_depth);
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
    
    //
    //  json_info_to_data(mex_struct)
    //
    //  Inputs
    //  ------
    //  mex_struct : output structure from turtle_json
    
    //
    
    
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
    data.array_depths = getU8Field(s,"array_depths");
    
    //String Data --------------------------
    data.strings = getMXField(s,"strings");
    
    data.dims = mxMalloc(MAX_ARRAY_DIMS*sizeof(mwSize));
    
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

