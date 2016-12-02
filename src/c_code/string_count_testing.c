#insert "mex.h"


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{

    uint8_t *data = mxGetData(prhs[0]);
    
    //_mm256_lddqu_si256
    
    //_mm256_set1_epi8
    
    __m256i result;
    result = _mm256_cmpeq_epi8
    
}