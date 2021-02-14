/* printmem.cpp */
//https://undocumentedmatlab.com/articles/matlabs-internal-memory-representation
#include "mex.h"
#include "math.h"
void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
    
   /*
  if (nrhs < 1 || !mxIsUint64(prhs[0]) || mxIsEmpty(prhs[0]))
    mexErrMsgTxt("First argument must be a uint64 memory address");
  unsigned long *addr = static_cast<unsigned long *>(mxGetData(prhs[0]));
  unsigned char *mem = (unsigned char *) addr[0];
  if (nrhs < 2 || !mxIsDouble(prhs[1]) || mxIsEmpty(prhs[1]))
    mexErrMsgTxt("Second argument must be a double-type integer byte size.");
  unsigned int nbytes = static_cast<unsigned int>(mxGetScalar(prhs[1]));
  for (int i = 0; i < nbytes; i++) {
    printf("%.2x ", mem[i]);
    if ((i+1) % 16 == 0) printf("\n");
 }
 printf("\n");
    */
    
    //void *name_or_CrossLinkReverse;
    //mxClassID ClassID;
    //int VariableType;
    //mxArray *CrossLink;
    //size_t ndim;
    //unsigned int RefCount; /* Number of sub-elements identical to this one */

    /* 2019a
     * 3,3,3
     * address
     * address
     * type
     * ndim
     * 0        0
     * long #   1
     * 9        10
     * long #
     */
    
    //2019b - like 2020b
    //
        
    struct mxArray_Tag_Partial {
        size_t b1;
        size_t b2;
        size_t b3;
        size_t b4;
        size_t b5;
        size_t b6;
        size_t b7;
        size_t b8;
        size_t b9;
    };
    
    //int mexCallMATLAB(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[], const char *functionName);

    
    struct mxArray_Tag_Partial *my = (struct mxArray_Tag_Partial *) prhs[0];
    
        mxArray *version = mxCreateNumericMatrix(1,100,mxUINT8_CLASS,mxREAL);
        //mxArray *version = mxCreateString("asdfsadfsadfasdf                                                                           ");
        mexCallMATLAB(1,&version,0, NULL, "version");
        //char *str = (char *) mxGetData(version);
        
        char *str = mxArrayToString(version);
        //mxGetString(version,str,10);
        float temp = strtof(str,NULL);
        int major_ver = (int)temp;
        int minor_ver = (int)roundf((temp-(float)major_ver)*10);
        int final_ver = major_ver*100 + minor_ver;

        mexPrintf("%d\n",final_ver);
    
    mexPrintf("Address of a: %p\n", prhs[0]);
    mwSize *b = ((mwSize *) prhs[0])+3;
    mexPrintf("b: %d: %d\n",*b);
    //++(*b);
    mexPrintf("b: %d: %d\n",*b);
    mexPrintf("%d\n",my->b1);
    mexPrintf("%d\n",my->b2);
    mexPrintf("%d\n",my->b3);
    mexPrintf("%d\n",my->b4);
    mexPrintf("%d\n",my->b5);
    mexPrintf("%d\n",my->b6);
    mexPrintf("%d\n",my->b7);
    mexPrintf("%d\n",my->b8);
    mexPrintf("%d\n",my->b9);

 
 
 
}