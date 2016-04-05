#include "mex.h"

// mex read_to_uint8.c


void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[] )
{
    //  TODO: Update documentation
    

	FILE *file;
    char *name;
	char *buffer;
	unsigned long fileLen;

    name = mxArrayToString(prhs[0]);

    plhs[0] = mxCreateNumericArray(0, 0, mxUINT8_CLASS, mxREAL);
    
	//Open file
	file = fopen(name, "rb");
	if (!file)
	{
		mexPrintf("Unable to open file %s\n", name);
		return;
	}
	
	//Get file length
	fseek(file, 0, SEEK_END);
	fileLen=ftell(file);
	fseek(file, 0, SEEK_SET);

	//Allocate memory
	buffer = mxMalloc(fileLen+1);
	if (!buffer)
	{
		mexPrintf("Memory error!");
                    fclose(file);
		return;
	}

	//Read file contents into buffer
	fread(buffer, fileLen, 1, file);
	fclose(file);

	//Do what ever with buffer

  
    
    plhs[0] = mxCreateNumericArray(0, 0, mxUINT8_CLASS, mxREAL);
    mxSetData(plhs[0] , buffer);
    mxSetM(plhs[0] , 1);
    mxSetN(plhs[0] , fileLen+1);
    
    
}