#include "mex.h" 
#include "TargetConditionals.h"


#ifdef __x86_64__
 #include <immintrin.h>
#else   
 #include "sse2neon.h"
#endif

//


//Summary
//- code is running Rosetta so we can't detect ARM
//- can detect use of SIMD instructions
//
//  results (for m1 mac)
//  - AVX and AVX2 missing
//  - SSE4_2 is supported


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
 
//This doesn't work (due to Rosetta I believe)
//--------------------------------------------
#ifdef __arm64__
//do smth on arm (M1)
    mexPrintf("m1\n");
#else
//do smth on x86 (Intel)
    mexPrintf("intel\n");
#endif   
    

//These all seem to work
//--------------------------------------------
#ifdef __AVX__
mexPrintf("AVX\n");    
#endif

#ifdef __AVX2__
mexPrintf("AVX2\n");    
#endif

#ifdef __SSE4_2__
mexPrintf("SSE4_2\n");    
#endif

#ifdef __SSE4_1__
mexPrintf("SSE4_2\n");    
#endif

#ifdef __SSE4_1__
mexPrintf("SSE4_1\n");    
#endif

#ifdef __SSE2__
mexPrintf("SSE2\n");    
#endif




//This identifies x86 on OS_OSX
#if TARGET_OS_OSX
  // Put CPU-independent macOS code here.
  #if TARGET_CPU_ARM64
    mexPrintf("m2\n");
    // Put 64-bit Apple silicon macOS code here.
  #elif TARGET_CPU_X86_64
    mexPrintf("m3\n");
    // Put 64-bit Intel macOS code here.
  #endif
#elif TARGET_OS_MACCATALYST
    mexPrintf("m4\n");
   // Put Mac Catalyst-specific code here.
#elif TARGET_OS_IOS
    mexPrintf("m5\n");
  // Put iOS-specific code here.
#endif
    

}