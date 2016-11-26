#include "mex.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    //  This function is meant to be called by the json.tokens constructor
    //
    //  Usage:
    
    double *data = malloc(100*8);
    double *data2 = mxMalloc(100*8);
    mxArray *wtf = mxCreateNumericMatrix(0,0,mxDOUBLE_CLASS,mxREAL);
    mxSetData(wtf,data);
    //mxSetData(wtf,data2);
    //free(data);
    
}