#include "jsmn.h"
#include "stdio.h"
#include <stdlib.h>
#include "mex.h"
#include <math.h>
#include "stdint.h"  //uint_8
#include <Expr.h>

//Possible changes:
//1) remove start and end
//2) fast parse strings by only looking for '"' then backtrack to verify
//  Note that \" may not be " as in "test string\\"
//3) for keys, use a hash map and only parse individual elements, and point
//everything to those elements
//4) Move to an array of depths and parents
//
//  This would remove a memory requirement but would add a check for
//  sufficient depth availability ...
//
//  - indices are parents
//  - single scalar 'depth' to track depth




//=========================================================================
//              Parse JSON   -    Parse JSON    -    Parse JSON
//=========================================================================
int jsmn_parse(const char *js, size_t string_byte_length) {
    
    /*
     *  Inputs
     *  ------
     *  parser :
     *    Initialized parser from jsmn_init()
     *  js :
     *    The JSON string to parse
     */
    
	const void *array_jump[] = { &&S1,&&S2,&&S3,&&S3,&&S4 };
    
    S1:
        goto *array_jump[3];
    S2:
        return 1;
    S3:
        return 3;
    S4:
        return 1;
    
    
        
    return 1;
                
                
                
                
}

