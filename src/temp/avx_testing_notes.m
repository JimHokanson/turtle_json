01
// get AVX intrinsics
02
#include <immintrin.h>
03
// get CPUID capability
04
#include <intrin.h>
05
 
06
// written for clarity, not conciseness
07
#define OSXSAVEFlag (1UL<<27)
08
#define AVXFlag     ((1UL<<28)|OSXSAVEFlag)
09
#define VAESFlag    ((1UL<<25)|AVXFlag|OSXSAVEFlag)
10
#define FMAFlag     ((1UL<<12)|AVXFlag|OSXSAVEFlag)
11
#define CLMULFlag   ((1UL<< 1)|AVXFlag|OSXSAVEFlag)
12
  
13
bool DetectFeature(unsigned int feature)
14
    {
15
    int CPUInfo[4], InfoType=1, ECX = 1;
16
    __cpuidex(CPUInfo, 1, 1);       // read the desired CPUID format
17
    unsigned int ECX = CPUInfo[2];  // the output of CPUID in the ECX register. 
18
    if ((ECX & feature) != feature) // Missing feature 
19
        return false; 
20
    __int64 val = _xgetbv(0);       // read XFEATURE_ENABLED_MASK register
21
    if ((val&6) != 6)               // check OS has enabled both XMM and YMM support.
22
        return false; 
23
    return true;
24
    }
