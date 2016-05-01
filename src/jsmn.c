#include "jsmn.h"
#include "stdio.h"
#include <stdlib.h>
#include "mex.h"
#include <math.h>
#include "stdint.h"  //uint_8

#define SKIP_WHITESPACE while (is_whitespace[js[++parser_position]]){}
#define ERROR_DEPTH mexErrMsgIdAndTxt("jsmn_mex:depth_limit","Max depth exceeded");

enum STATES {
    S_OPEN_OBJECT_IN_KEY,
    S_OPEN_OBJECT_IN_ARRAY,
    S_CLOSE_KEY_AND_OBJECT,
    S_CLOSE_EMPTY_OBJECT,
    S_OPEN_ARRAY_IN_KEY,
    S_OPEN_ARRAY_IN_ARRAY,
    S_CLOSE_ARRAY,
    S_CLOSE_EMPTY_ARRAY,
    S_PARSE_KEY,
    S_PARSE_STRING_IN_KEY,
    S_PARSE_STRING_IN_ARRAY,
    S_PARSE_NUMBER_IN_KEY,
    S_PARSE_NUMBER_IN_ARRAY,
    S_PARSE_NULL_IN_KEY,
    S_PARSE_NULL_IN_ARRAY,
    S_PARSE_TRUE_IN_KEY,
    S_PARSE_TRUE_IN_ARRAY,
    S_PARSE_FALSE_IN_KEY,
    S_PARSE_FALSE_IN_ARRAY,
    S_PARSE_END_OF_VALUE_IN_ARRAY,
    S_PARSE_END_OF_VALUE_IN_KEY,
    S_PARSE_END_OF_FILE,
    S_PARSE_COMMA_IN_ARRAY,
    S_PARSE_COMMA_IN_OBJECT,
    S_ERROR_OPEN_OBJECT,
    S_ERROR_OPEN_ARRAY,
    S_ERROR_OPEN_KEY_1,
    S_ERROR_OPEN_KEY_2,
    S_ERROR_UNIMPLEMENTED,
    S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY
};

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

/*
 * temp = cell(1,256);
 * temp(:)={'false'};
 * temp(49:58) = {'true'};
 * clipboard('copy',['const bool is_number_array[256] = {' sl.cellstr.join(temp,'d',',') '};'])
 *
 * %NOTE: the ascii table is actually 1 based as well so we add 1
 * %i.e. in Matlab, space is 32
 * %in c, space is also 32
 * %This arises since the 1st character in the ascii table starts counting at 0
 * temp = cell(1,256);
 * temp(:)={'false'};
 * ws_chars = [' ',sprintf('\n'),sprintf('\r'),sprintf('\t')];
 * temp(double(ws_chars)+1) = {'true'};
 * clipboard('copy',['const bool is_whitespace[256] = {' sl.cellstr.join(temp,'d',',') '};'])
 *
 * %POSITIVE_VALUES
 * values = 1:9;
 * n_entries = 16;
 * str = cell(1,n_entries);
 * for i = 1:n_entries
 * temp = cell(1,58);
 * temp(:) = {'0'};
 * temp(50:58) = arrayfun(@(x) sprintf('%d',x),values*1*10^(i-1),'un',0);
 * str{i} = ['const double p1e' int2str(i-1) '[58] = {' sl.cellstr.join(temp,'d',',') '};'];
 * end
 * clipboard('copy',sl.cellstr.join(str,'d',char(10)))
 *
 * values = 1:9;
 * %   1.json had 20 :/
 * n_entries = 20;
 * str = cell(1,n_entries);
 * for i = 1:n_entries
 * temp = cell(1,58);
 * temp(:) = {'0'};
 * format = ['%0.' int2str(i) 'f'];
 * temp(50:58) = arrayfun(@(x) sprintf(format,x),values.*10^(-i),'un',0);
 * str{i} = ['const double p1e_' int2str(i) '[58] = {' sl.cellstr.join(temp,'d',',') '};'];
 * end
 * clipboard('copy',sl.cellstr.join(str,'d',char(10)))
 *
 *
 */

const bool is_whitespace[256] = {false,false,false,false,false,false,false,false,false,true,true,false,false,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};
const bool is_number_array[256] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true,true,true,true,true,true,true,true,true,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};

//Values for Integer portion of number
//------------------------------------
//Note that I'm avoiding subtracting 0 which makes these arrays rather large ...
const double p1e0[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9};
const double p1e1[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,20,30,40,50,60,70,80,90};
const double p1e2[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,200,300,400,500,600,700,800,900};
const double p1e3[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000,2000,3000,4000,5000,6000,7000,8000,9000};
const double p1e4[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10000,20000,30000,40000,50000,60000,70000,80000,90000};
const double p1e5[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100000,200000,300000,400000,500000,600000,700000,800000,900000};
const double p1e6[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000000,2000000,3000000,4000000,5000000,6000000,7000000,8000000,9000000};
const double p1e7[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10000000,20000000,30000000,40000000,50000000,60000000,70000000,80000000,90000000};
const double p1e8[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100000000,200000000,300000000,400000000,500000000,600000000,700000000,800000000,900000000};
const double p1e9[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000000000,2000000000,3000000000,4000000000,5000000000,6000000000,7000000000,8000000000,9000000000};
const double p1e10[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10000000000,20000000000,30000000000,40000000000,50000000000,60000000000,70000000000,80000000000,90000000000};
const double p1e11[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100000000000,200000000000,300000000000,400000000000,500000000000,600000000000,700000000000,800000000000,900000000000};
const double p1e12[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000000000000,2000000000000,3000000000000,4000000000000,5000000000000,6000000000000,7000000000000,8000000000000,9000000000000};
const double p1e13[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10000000000000,20000000000000,30000000000000,40000000000000,50000000000000,60000000000000,70000000000000,80000000000000,90000000000000};
const double p1e14[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100000000000000,200000000000000,300000000000000,400000000000000,500000000000000,600000000000000,700000000000000,800000000000000,900000000000000};
const double p1e15[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000000000000000,2000000000000000,3000000000000000,4000000000000000,5000000000000000,6000000000000000,7000000000000000,8000000000000000,9000000000000000};

//Values for fractional component
//-------------------------------
//Note, it is unclear how accurate we can really be with this ...
//and accordingly, how far out we should really populate ...
//
//I've gone out to the maximum of double (I think)
//
//i.e. 2.0123456789012345 ?
const double p1e_1[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
const double p1e_2[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09};
const double p1e_3[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.001,0.002,0.003,0.004,0.005,0.006,0.007,0.008,0.009};
const double p1e_4[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0001,0.0002,0.0003,0.0004,0.0005,0.0006,0.0007,0.0008,0.0009};
const double p1e_5[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00001,0.00002,0.00003,0.00004,0.00005,0.00006,0.00007,0.00008,0.00009};
const double p1e_6[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000001,0.000002,0.000003,0.000004,0.000005,0.000006,0.000007,0.000008,0.000009};
const double p1e_7[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000001,0.0000002,0.0000003,0.0000004,0.0000005,0.0000006,0.0000007,0.0000008,0.0000009};
const double p1e_8[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000001,0.00000002,0.00000003,0.00000004,0.00000005,0.00000006,0.00000007,0.00000008,0.00000009};
const double p1e_9[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000001,0.000000002,0.000000003,0.000000004,0.000000005,0.000000006,0.000000007,0.000000008,0.000000009};
const double p1e_10[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000001,0.0000000002,0.0000000003,0.0000000004,0.0000000005,0.0000000006,0.0000000007,0.0000000008,0.0000000009};
const double p1e_11[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000001,0.00000000002,0.00000000003,0.00000000004,0.00000000005,0.00000000006,0.00000000007,0.00000000008,0.00000000009};
const double p1e_12[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000000001,0.000000000002,0.000000000003,0.000000000004,0.000000000005,0.000000000006,0.000000000007,0.000000000008,0.000000000009};
const double p1e_13[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000000001,0.0000000000002,0.0000000000003,0.0000000000004,0.0000000000005,0.0000000000006,0.0000000000007,0.0000000000008,0.0000000000009};
const double p1e_14[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000000001,0.00000000000002,0.00000000000003,0.00000000000004,0.00000000000005,0.00000000000006,0.00000000000007,0.00000000000008,0.00000000000009};
const double p1e_15[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000000000001,0.000000000000002,0.000000000000003,0.000000000000004,0.000000000000005,0.000000000000006,0.000000000000007,0.000000000000008,0.000000000000009};
const double p1e_16[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000000000001,0.0000000000000002,0.0000000000000003,0.0000000000000004,0.0000000000000005,0.0000000000000006,0.0000000000000007,0.0000000000000008,0.0000000000000009};
const double p1e_17[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000000000001,0.00000000000000002,0.00000000000000003,0.00000000000000004,0.00000000000000005,0.00000000000000006,0.00000000000000007,0.00000000000000008,0.00000000000000009};
const double p1e_18[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000000000000001,0.000000000000000002,0.000000000000000003,0.000000000000000004,0.000000000000000005,0.000000000000000006,0.000000000000000007,0.000000000000000008,0.000000000000000009};
const double p1e_19[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000000000000001,0.0000000000000000002,0.0000000000000000003,0.0000000000000000004,0.0000000000000000005,0.0000000000000000006,0.0000000000000000007,0.0000000000000000008,0.0000000000000000009};
const double p1e_20[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000000000000001,0.00000000000000000002,0.00000000000000000003,0.00000000000000000004,0.00000000000000000005,0.00000000000000000006,0.00000000000000000007,0.00000000000000000008,0.00000000000000000009};

//*p++                  js[parser_position++]
//*p   ==========       js[parser_position]
//++p  ==========       ++parser_position

double string_to_double(const char *p,char **char_offset) {
    
    /*
     *  I found atof to be the main bottleneck in this code. It was
     *  suggested to me by some internet searching that a custom algorithm
     *  would be close enough in 99.99 of the cases. I found a couple
     *  basic ones online that used loops and multiplation. This was my
     *  attempt to make something as fast possible. I welcome a faster
     *  approach!
     */
    
    double value = 0;
    double exponent_value;
    bool negate;
    int64_t n_numeric_chars_plus_1;
    char *number_start;
    
    if (*p == '-'){
        ++p;
        negate = true;
    }else{
        negate = false;
    }
    
    number_start = p;
    while (is_number_array[*p++]) {
    }
    
    n_numeric_chars_plus_1 = p-number_start; //# of numeric characters (off by 1 due to ++)
    
    //reset p so that we can increment our way along the number
    p = number_start;
    //Now that we know how many #s we have (off by 1 due to ++)
    //we can start at the beginning and add each to its correct place
    //e.g:
    //  4086 =
    //  4000 +
    //   0   +
    //    80 +
    //     6
    
    //I'm curious if you could take advantage of something like SSE
    //if each case was written out explicitly rather than via fall through
    switch (n_numeric_chars_plus_1) {
        case 17:
            value += p1e15[*p++];
        case 16:
            value += p1e14[*p++];
        case 15:
            value += p1e13[*p++];
        case 14:
            value += p1e12[*p++];
        case 13:
            value += p1e11[*p++];
        case 12:
            value += p1e10[*p++];
        case 11:
            value += p1e9[*p++];
        case 10:
            value += p1e8[*p++];
        case 9:
            value += p1e7[*p++];
        case 8:
            value += p1e6[*p++];
        case 7:
            value += p1e5[*p++];
        case 6:
            value += p1e4[*p++];
        case 5:
            value += p1e3[*p++];
        case 4:
            value += p1e2[*p++];
        case 3:
            value += p1e1[*p++]; //1e1 == 10, 2 #s plus the off by 1
        case 2:
            value += p1e0[*p++]; //1e0 == 1, an unfortunate mismatch of exponent and scalar
            break;
        case 1:
            //TODO: Provide a location
            mexErrMsgIdAndTxt("jsmn_mex:no_number","Too many #s");
        default:
            mexErrMsgIdAndTxt("jsmn_mex:too_many_integers","The integer component of the number had too many digits");
    }
    
    //This would be written neater as a while loop that updates a pointer
    //to the various arrays but I don't think it would be as fast
    //
    //Unlike the integer part, we know how to interpret each
    //numeric value as we are parsing it
    //
    //0.1234 =
    //0.1 +
    //0.02 +
    //0.003 +
    //0.0004
    
    if (*p == '.') {
        ++p;
        //TODO: Is no digit ok?
        if(is_number_array[*p]){
            value += p1e_1[*p++];
            if(is_number_array[*p]){
                value += p1e_2[*p++];
                if(is_number_array[*p]){
                    value += p1e_3[*p++];
                    if(is_number_array[*p]){
                        value += p1e_4[*p++];
                        if(is_number_array[*p]){
                            value += p1e_5[*p++];
                            if(is_number_array[*p]){
                                value += p1e_6[*p++];
                                if(is_number_array[*p]){
                                    value += p1e_7[*p++];
                                    if(is_number_array[*p]){
                                        value += p1e_8[*p++];
                                        if(is_number_array[*p]){
                                            value += p1e_9[*p++];
                                            if(is_number_array[*p]){
                                                value += p1e_10[*p++];
                                                if(is_number_array[*p]){
                                                    value += p1e_11[*p++];
                                                    if(is_number_array[*p]){
                                                        value += p1e_12[*p++];
                                                        if(is_number_array[*p]){
                                                            value += p1e_13[*p++];
                                                            if(is_number_array[*p]){
                                                                value += p1e_14[*p++];
                                                                if(is_number_array[*p]){
                                                                    value += p1e_15[*p++];
                                                                    if(is_number_array[*p]){
                                                                        value += p1e_16[*p++];
                                                                        if(is_number_array[*p]){
                                                                            value += p1e_17[*p++];
                                                                            if(is_number_array[*p]){
                                                                                value += p1e_18[*p++];
                                                                                if(is_number_array[*p]){
                                                                                    value += p1e_19[*p++];
                                                                                    if(is_number_array[*p]){
                                                                                        value += p1e_20[*p++];
                                                                                        if(is_number_array[*p]){
                                                                                            mexErrMsgIdAndTxt("jsmn_mex:too_many_decimals","The fractional component of the number had too many digits");
                                                                                            
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //End of if '.'
    if (negate) {
        value = -value;
    }
    
    if (*p == 'E' || *p == 'e') {
        ++p;
        switch (*p){
            case '-':
                ++p;
                negate = true;
                break;
            case '+':
                ++p;
            default:
                negate = false;
        }
        
        number_start = p;
        while (is_number_array[*p++]) {
        }
        
        exponent_value = 0;
        n_numeric_chars_plus_1 = p-number_start;
        //reset p so that we can increment our way down
        p = number_start;
        switch (n_numeric_chars_plus_1) {
            case 17:
                exponent_value += p1e15[*p++];
            case 16:
                exponent_value += p1e14[*p++];
            case 15:
                exponent_value += p1e13[*p++];
            case 14:
                exponent_value += p1e12[*p++];
            case 13:
                exponent_value += p1e11[*p++];
            case 12:
                exponent_value += p1e10[*p++];
            case 11:
                exponent_value += p1e9[*p++];
            case 10:
                exponent_value += p1e8[*p++];
            case 9:
                exponent_value += p1e7[*p++];
            case 8:
                exponent_value += p1e6[*p++];
            case 7:
                exponent_value += p1e5[*p++];
            case 6:
                exponent_value += p1e4[*p++];
            case 5:
                exponent_value += p1e3[*p++];
            case 4:
                exponent_value += p1e2[*p++];
            case 3:
                exponent_value += p1e1[*p++];
            case 2:
                exponent_value += p1e0[*p++];
                break;
            case 1:
                mexErrMsgIdAndTxt("jsmn_mex:empty_exponent","An exponent was given with no numeric value");
            default:
                //TODO: Give error location in string
                mexErrMsgIdAndTxt("jsmn_mex:large_exponent","There were more than 15 digits in a numeric exponent");
        }
        if (negate){
            exponent_value = -exponent_value;
        }
        value *= pow(10.0, exponent_value);
    }
    
    *char_offset = p;
    
    return value;
}
//-------------------------------------------------------------------------
//--------------------  End of Number Parsing  ----------------------------
//-------------------------------------------------------------------------
//TODO: Allow looking for closer - verify math later
double string_to_double_no_math(char *p, char **char_offset) {
    
    while (is_number_array[*(++p)]) {
    }
    
    if (*p == '.'){
        ++p;
        while (is_number_array[*(++p)]) {
        }
    }
    
    if (*p == 'E' || *p == 'e') {
        ++p;
        switch (*p){
            case '-':
                ++p;
                break;
            case '+':
                ++p;
        }
        
        while (is_number_array[*(++p)]) {
        }
    }
    
    *char_offset = p;
    
    return 0;
}


void seek_string_end(const char *js, int *input_parser_position){
    
    //  seek_string_end(js,&parser_position)
    
    //TODO: This could be completely wrong if we have unicode
    //although I think unicode has to be escaped????
    int parser_position = *input_parser_position;
    char c;
    
    while ((c = js[++parser_position])) {
        if (c == '\"') {
            //The idea here is that we don't check this all the time
            //If we hit \" we would need to keep going back until
            //we determine if the '\' is real or not
            if (js[parser_position-1] == '\\'){
                mexErrMsgIdAndTxt("jsmn_mex:unhandled_case","Code not yet written");
            }
            *input_parser_position = parser_position;
            return;
        }
    }
    
    mexErrMsgIdAndTxt("jsmn_mex:unterminated_string","Unable to find a terminating string quote");
}

//=========================================================================
//                          String Parsing
//=========================================================================
void parse_string_helper(const char *js, int current_token_index, int *input_parser_position, size_t len, mxArray **mxStrings){
    
    //TODO: This function needs work
    
    //      parse_string_helper(js,token,parser_position)
    
    //TODO: http://www.mathworks.com/matlabcentral/answers/71173-passing-unicode-string-from-c-mex-function-to-matlab
    
    int parser_position = *input_parser_position;
    char c;
    int i;
    char *output_str;
    int start_position = parser_position+1;
    int n_chars;
    mwSize dims[1] = {10};
    
    //Looping over the string ...
    //---------------------------------------------------------
    while ((c = js[++parser_position])) {
        
        if (c == '\"') {
            //Again, this is Matlab based, add flag and
            //handle accordingly ...
            //We're pointing to the " in c
            //which in Matlab points to the character before
            //c
            
            //http://www.mathworks.com/matlabcentral/newsreader/view_thread/301249
            
            n_chars = parser_position - start_position + 1;
            
            ////mx_output_string = mxCreateCharArray(1,dims);
            
            //Approach 1
            //---------------------------------
            //This would require a char conversion later
//             mx_output_string = mxCreateNumericArray(0, 0, mxUINT16_CLASS, mxREAL);
//             output_str = mxMalloc(n_chars);
//             mxSetM(mx_output_string, 1);
//             mxSetN(mx_output_string, n_chars);
//             mxSetData(mx_output_string, output_str);
            
            //Aproach 2
            //---------
            //memcopy and setting of value
            
//             n_chars = parser_position - start_position + 1;
//             output_str = mxMalloc(n_chars);
//             memcpy(output_str,&js[start_position],n_chars);
//             output_str[n_chars-1] = 0;
//
//             mxStrings[current_token_index] = mxCreateString(output_str);
            
            *input_parser_position = parser_position;
            return;
            //goto parse_string_end;
        }
        
        //This is just error checking
        //---------------------------------------------
        /* Backslash: Quoted symbol expected */
        if (c == '\\' && parser_position + 1 < len) {
            //parser_position++;
            switch (js[parser_position++]) {
                /* Allowed escaped symbols */
                case '\"':
                case '/' :
                case '\\':
                case 'b' :
                case 'f' :
                case 'r' :
                case 'n'  :
                case 't' :
                    break;
                    /* Allows escaped symbol \uXXXX */
                case 'u':
                    //TODO: Make this an error ...
                    //I don't know that we could properly handle this ...
                    parser_position++;
                    for(i = 0; i < 4 && parser_position < len; i++) {
                        /* If it isn't a hex character we have an error */
                        //TODO: This can be a lookup table
                        if(!((js[parser_position] >= 48 && js[parser_position] <= 57) || /* 0-9 */
                                (js[parser_position] >= 65 && js[parser_position] <= 70) || /* A-F */
                                (js[parser_position] >= 97 && js[parser_position] <= 102))) { /* a-f */
                            
                            mexPrintf("Invalid Char: %c\n",js[parser_position]);
                            mexPrintf("Location: %d\n",parser_position);
                            
                            mexErrMsgIdAndTxt("wtf:wtf5","Invalid unicode characters");
                        }
                        parser_position++;
                    }
                    parser_position--;
                    break;
                    /* Unexpected symbol */
                default:
//                     mexPrintf("Invalid Char: %c\n",js[parser_position]);
//                     mexPrintf("Location: %d\n",parser_position);
                    mexErrMsgIdAndTxt("jsmn_mex:invalid_escapse_char","Invalid escape character encountered while parsing string");
            }
        }
    }
    
    mexErrMsgIdAndTxt("jsmn_mex:unterminated_string","Unable to find a terminating string quote");
}


//Parser initialization
//----------------------------------
void jsmn_init(jsmn_parser *parser) {
    parser->position      = -1;
    parser->current_token = -1;
    parser->super_token   = -1;
    parser->last_function_type = -1;
    parser->n_numbers = 0;
}

//We work with the parser variables directly
//Here we
void refill_parser(jsmn_parser *parser,
        int parser_position,
        int current_token_index,
        int super_token_index,
        int last_function_type){
    
    
    //  refill_parser(parser,parser_position,next_token_index,super_token_index,false)
    
    parser->position = parser_position;
    parser->current_token = current_token_index;
    parser->super_token = super_token_index;
    parser->last_function_type = last_function_type;
}

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
    
    int objects_or_arrays_i = -1;
    int keys_i = -1;
    int strings_i = -1;
    int numeric_i = -1;
    
    int objects_or_arrays_max_i = 1000;
    int keys_max_i = 1000;
    int strings_max_i = 1000;
    int numeric_max_i = 1000;
    
    enum STATES next_state;
    const int MAX_DEPTH = 200;
    
    //TODO: This needs to be fixed
    int parent_types[201];
    int parent_indices[201];
    int parent_sizes[201];
    int current_depth = 0;
    
    int parser_position = -1;
    char *pEndNumber;
    int current_token_index = -1;
    int n_tokens_to_allocate;
    uint8_t *main_types;
    int *main_indices;
    
    n_tokens_to_allocate = ceil(string_byte_length/2);
    main_types = mxMalloc(n_tokens_to_allocate);
    main_indices = mxMalloc(n_tokens_to_allocate*sizeof(int));
    
    SKIP_WHITESPACE;
            
    switch (js[parser_position]) {
        case '{':
            next_state = S_OPEN_OBJECT_IN_KEY;
            break;
        case '[':
            next_state = S_OPEN_ARRAY_IN_KEY;
            break;
        default:
            mexErrMsgIdAndTxt("jsmn_mex:invalid_start","Starting token needs to be an opening object or array");
    }
            
    while (1){
        switch(next_state){

            //=============================================================
            case S_OPEN_OBJECT_IN_ARRAY:
                parent_sizes[current_depth] +=1;

                //Fall Through --------------------
            case S_OPEN_OBJECT_IN_KEY:

                ++current_token_index;
                main_types[current_token_index] = TYPE_OBJECT;
                main_indices[current_token_index] = ++objects_or_arrays_i;

                if (objects_or_arrays_i > objects_or_arrays_max_i){
                    //TODO: Resize
                }

                if (current_depth == MAX_DEPTH){
                    ERROR_DEPTH
                }else{
                    ++current_depth;
                    parent_types[current_depth] = TYPE_OBJECT;
                    parent_indices[current_depth] = objects_or_arrays_i;
                    parent_sizes[current_depth] = 0;
                }

                SKIP_WHITESPACE

                switch (js[parser_position]){
                    case '"':
                        next_state = S_PARSE_KEY;
                        break;
                    case '}':
                        next_state = S_CLOSE_EMPTY_OBJECT;
                        break;
                    default:
                        next_state = S_ERROR_OPEN_OBJECT;
                }

                break;

                        //=============================================================
            case S_CLOSE_KEY_AND_OBJECT:
                //TODO: set token close
                --current_depth;

                //Fall Through ------  closing the object
            case S_CLOSE_EMPTY_OBJECT:

                //TODO: Insert logic for object
                //token close
                //set count

                if(current_depth == 1){
                    next_state = S_PARSE_END_OF_FILE;
                }else{
                    --current_depth;
                    if (parent_types[current_depth] == TYPE_KEY){
                        next_state = S_PARSE_END_OF_VALUE_IN_KEY;
                    }
                    else{
                        next_state = S_PARSE_END_OF_VALUE_IN_ARRAY;
                    }
                }

                break;

                //=============================================================
            case S_OPEN_ARRAY_IN_ARRAY:
                parent_sizes[current_depth] +=1;

                //Fall Through -------------------------------
            case S_OPEN_ARRAY_IN_KEY:
                ++current_token_index;

                main_types[current_token_index] = TYPE_ARRAY;
                main_indices[current_token_index] = ++objects_or_arrays_i;

                if (objects_or_arrays_i > objects_or_arrays_max_i){
                    //TODO: Resize
                }

                if (current_depth == MAX_DEPTH){
                    ERROR_DEPTH
                }else{
                    ++current_depth;
                    parent_types[current_depth] = TYPE_ARRAY;
                    parent_indices[current_depth] = objects_or_arrays_i;
                    parent_sizes[current_depth] = 0;
                }

                //TODO: This could be a call to S_PARSE_END_OF_VALUE_IN_ARRAY
                SKIP_WHITESPACE

                        switch(js[parser_position]){
                            case '"':
                                next_state = S_PARSE_STRING_IN_ARRAY;
                                break;
                            case '-':
                            case '0':
                            case '1':
                            case '2':
                            case '3':
                            case '4':
                            case '5':
                            case '6':
                            case '7':
                            case '8':
                            case '9':
                                next_state = S_PARSE_NUMBER_IN_ARRAY;
                                break;
                            case '{':
                                next_state = S_OPEN_OBJECT_IN_ARRAY;
                                break;
                            case '[':
                                next_state = S_OPEN_ARRAY_IN_ARRAY;
                                break;
                            case ']':
                                next_state = S_CLOSE_ARRAY;
                                break;
                            case 't':
                                next_state = S_PARSE_TRUE_IN_ARRAY;
                                break;
                            case 'f':
                                next_state = S_PARSE_FALSE_IN_ARRAY;
                                break;
                            case 'n':
                                next_state = S_PARSE_NULL_IN_ARRAY;
                                break;
                            default:
                                next_state = S_ERROR_OPEN_ARRAY;
                        }

                        break;

                        //=============================================================
            case S_CLOSE_ARRAY:

                //TODO: Insert logic for object
                //token close
                //set count

                if(current_depth == 1){
                    next_state = S_PARSE_END_OF_FILE;
                }else{
                    --current_depth;
                    if (parent_types[current_depth] == TYPE_KEY){
                        next_state = S_PARSE_END_OF_VALUE_IN_KEY;
                    }else{
                        next_state = S_PARSE_END_OF_VALUE_IN_ARRAY;
                    }
                }
//                 mexPrintf("WTF\n");
//                 mexPrintf("Type: %d\n",main_types[current_depth]);
//                 mexPrintf("State: %d\n",next_state);

                break;

                //=============================================================
            case S_PARSE_KEY:
                parent_sizes[current_depth] +=1;
                ++current_token_index;

                main_types[current_token_index] = TYPE_KEY;
                main_indices[current_token_index] = ++keys_i;


                //TODO: Change below ....
                if (keys_i > keys_max_i){
                    //TODO: Resize
                }

                if (current_depth == MAX_DEPTH){
                    ERROR_DEPTH
                }else{
                    ++current_depth;
                    parent_types[current_depth] = TYPE_KEY;
                    parent_indices[current_depth] = keys_i;
                }


                //TODO: Set start

                seek_string_end(js,&parser_position);

                //TODO: Set end


                SKIP_WHITESPACE;

                if (js[parser_position] == ':'){

                    //Advance to the next token
                    SKIP_WHITESPACE

                            switch(js[parser_position]){
                                case '"':
                                    next_state = S_PARSE_STRING_IN_KEY;
                                    break;
                                case '-':
                                case '0':
                                case '1':
                                case '2':
                                case '3':
                                case '4':
                                case '5':
                                case '6':
                                case '7':
                                case '8':
                                case '9':
                                    next_state = S_PARSE_NUMBER_IN_KEY;
                                    break;
                                case '{':
                                    next_state = S_OPEN_OBJECT_IN_KEY;
                                    break;
                                case '[':
                                    next_state = S_OPEN_ARRAY_IN_KEY;
                                    break;
                                case 't':
                                    next_state = S_PARSE_TRUE_IN_KEY;
                                    break;
                                case 'f':
                                    next_state = S_PARSE_FALSE_IN_KEY;
                                    break;
                                case 'n':
                                    next_state = S_PARSE_NULL_IN_KEY;
                                    break;
                                default:
                                    next_state = S_ERROR_OPEN_KEY_2;
                            }
                }else{
                    next_state = S_ERROR_OPEN_KEY_1;
                }

                break;

                //=============================================================
            case S_PARSE_STRING_IN_ARRAY:
                parent_sizes[current_depth] +=1;
                ++current_token_index;

                main_types[current_token_index] = TYPE_STRING;
                main_indices[current_token_index] = ++strings_i;

                if (strings_i > strings_max_i){
                    //TODO: Resize
                }

                //TODO: Set start
                //
                seek_string_end(js,&parser_position);

                //TODO: Set end

                next_state = S_PARSE_END_OF_VALUE_IN_ARRAY;

                break;

                //=============================================================
            case S_PARSE_STRING_IN_KEY:
                ++current_token_index;

                main_types[current_token_index] = TYPE_STRING;
                main_indices[current_token_index] = ++strings_i;

                if (strings_i > strings_max_i){
                    //TODO: Resize
                }

                //TODO: Set start
                //
                seek_string_end(js,&parser_position);

                //TODO: Set end

                next_state = S_PARSE_END_OF_VALUE_IN_KEY;

                break;

                //=============================================================
            case S_PARSE_NUMBER_IN_KEY:

                ++current_token_index;

                main_types[current_token_index] = TYPE_NUMBER;
                main_indices[current_token_index] = ++numeric_i;

                if (numeric_i > numeric_max_i){
                    //TODO: Resize

                }

                //TODO: log start

                string_to_double_no_math(js+parser_position,&pEndNumber);
                parser_position = (int)(pEndNumber - js);
                parser_position--;

                next_state = S_PARSE_END_OF_VALUE_IN_KEY;

                break;

                //=============================================================
            case S_PARSE_NUMBER_IN_ARRAY:
                parent_sizes[current_depth] +=1;
                ++current_token_index;

                main_types[current_token_index] = TYPE_NUMBER;
                main_indices[current_token_index] = ++numeric_i;

                if (numeric_i > numeric_max_i){
                    //TODO: Resize

                }

                //TODO: log start

                string_to_double_no_math(js+parser_position,&pEndNumber);
                parser_position = (int)(pEndNumber - js);
                parser_position--; 

                next_state = S_PARSE_END_OF_VALUE_IN_ARRAY;

                break;

                //=============================================================
            case S_PARSE_NULL_IN_KEY:

                ++current_token_index;

                main_types[current_token_index] = TYPE_NUMBER;
                main_indices[current_token_index] = ++numeric_i;

                if (numeric_i > numeric_max_i){
                    //TODO: Resize

                }

                //TODO: log start

                parser_position  += 3;
                next_state = S_PARSE_END_OF_VALUE_IN_KEY;

                break;

                //=============================================================
            case S_PARSE_NULL_IN_ARRAY:

                parent_sizes[current_depth] +=1;
                ++current_token_index;

                main_types[current_token_index] = TYPE_NUMBER;
                main_indices[current_token_index] = ++numeric_i;

                if (numeric_i > numeric_max_i){
                    //TODO: Resize

                }

                //TODO: log start

                parser_position  += 3;
                next_state = S_PARSE_END_OF_VALUE_IN_ARRAY;

                break;

                //=============================================================
            case S_PARSE_TRUE_IN_KEY:
                ++current_token_index;

                main_types[current_token_index] = TYPE_LOGICAL;
                main_indices[current_token_index] = 1;
                
                parser_position  += 3;
                next_state = S_PARSE_END_OF_VALUE_IN_KEY;
                break;
                
            case S_PARSE_TRUE_IN_ARRAY:
                parent_sizes[current_depth] +=1;
                ++current_token_index;

                main_types[current_token_index] = TYPE_LOGICAL;
                main_indices[current_token_index] = 1;
                
                parser_position  += 3;
                next_state = S_PARSE_END_OF_VALUE_IN_ARRAY;
                break;
                
            case S_PARSE_FALSE_IN_KEY:
                ++current_token_index;

                main_types[current_token_index] = TYPE_LOGICAL;
                main_indices[current_token_index] = 0;
                
                parser_position  += 4;
                next_state = S_PARSE_END_OF_VALUE_IN_KEY;
                break;
                
            case S_PARSE_FALSE_IN_ARRAY:
                parent_sizes[current_depth] +=1;
                ++current_token_index;

                main_types[current_token_index] = TYPE_LOGICAL;
                main_indices[current_token_index] = 0;
                
                parser_position  += 4;
                next_state = S_PARSE_END_OF_VALUE_IN_ARRAY;
                break;
                
            case S_PARSE_END_OF_VALUE_IN_ARRAY:
                SKIP_WHITESPACE;

                switch(js[parser_position]){
                    case ',':
                        next_state = S_PARSE_COMMA_IN_ARRAY;
                        break;
                    case ']':
                        next_state = S_CLOSE_ARRAY;
                        break;
                    default :
//                         mexPrintf("Current depth: %d\n",current_depth);
//                         mexPrintf("Current type: %d\n",parent_types[current_depth-4]);
//                         mexPrintf("Current type: %d\n",parent_types[current_depth-3]);
//                         mexPrintf("Current type: %d\n",parent_types[current_depth-2]);
//                         mexPrintf("Current type: %d\n",parent_types[current_depth-1]);
//                         mexPrintf("Current type: %d\n",parent_types[current_depth]);
//                         mexPrintf("Current char: %c\n",js[parser_position]);
                        mexPrintf("Current position: %d\n",parser_position);
                        mexErrMsgIdAndTxt("jsmn_mex:invalid_token","Token in array must be followed by a comma or a closing array ""]"" character ");
                }
                break;

                //=============================================================
            case S_PARSE_END_OF_VALUE_IN_KEY:

                SKIP_WHITESPACE;

                switch(js[parser_position]){
                    case ',':
                        next_state = S_PARSE_COMMA_IN_OBJECT;
                        break;
                    case '}':
                        next_state = S_CLOSE_KEY_AND_OBJECT;
                        break;
                    default :
                        mexErrMsgIdAndTxt("jsmn_mex:invalid_token","Token of key must be followed by a comma or a closing object ""}"" character");
                }

                break;

                //=============================================================
            case S_PARSE_END_OF_FILE:
                SKIP_WHITESPACE

                        if (!(js[parser_position] == '\0')){
                            mexErrMsgIdAndTxt("jsmn_mex:invalid_end","non-whitespace characters found after end of root token close");
                        }

                goto finish_main;

                break;

                //=============================================================
            case S_PARSE_COMMA_IN_ARRAY:
                SKIP_WHITESPACE

                        switch (js[parser_position]){
                            case '-':
                            case '0':
                            case '1':
                            case '2':
                            case '3':
                            case '4':
                            case '5':
                            case '6':
                            case '7':
                            case '8':
                            case '9':
                                next_state = S_PARSE_NUMBER_IN_ARRAY;
                                break;
                            case 'n':
                                next_state = S_PARSE_NULL_IN_ARRAY;
                                break;
                            case 't':
                                next_state = S_PARSE_TRUE_IN_ARRAY;
                                break;
                            case 'f':
                                next_state = S_PARSE_FALSE_IN_ARRAY;
                                break;
                            case '"':
                                next_state = S_PARSE_STRING_IN_ARRAY;
                                break;
                            case '{':
                                next_state = S_OPEN_OBJECT_IN_ARRAY;
                                break;
                            case '[':
                                next_state = S_OPEN_ARRAY_IN_ARRAY;
                                break;
                            default:
                                next_state = S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY;

                        }

                        break;

                        //=============================================================
            case S_PARSE_COMMA_IN_OBJECT:

                SKIP_WHITESPACE
                        --current_depth;

                if (js[parser_position] == '"'){
                    next_state = S_PARSE_KEY;
                }else{
                    //TODO: Change this ...
                    mexErrMsgIdAndTxt("jsmn_mex:no_key","Key expected");
                }

                break;

                //=============================================================
            case S_ERROR_OPEN_OBJECT:
                mexErrMsgIdAndTxt("jsmn_mex:invalid_token","S_ERROR_OPEN_OBJECT");
            case S_ERROR_OPEN_ARRAY:
                mexErrMsgIdAndTxt("jsmn_mex:invalid_token","S_ERROR_OPEN_ARRAY");
            case S_ERROR_OPEN_KEY_1:
                mexErrMsgIdAndTxt("jsmn_mex:invalid_token","S_ERROR_OPEN_KEY_1");
            case S_ERROR_OPEN_KEY_2:
                mexErrMsgIdAndTxt("jsmn_mex:invalid_token","S_ERROR_OPEN_KEY_2");
            case S_ERROR_UNIMPLEMENTED:
                mexErrMsgIdAndTxt("jsmn_mex:invalid_token","S_ERROR_UNIMPLEMENTED");
            case S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY:
                mexErrMsgIdAndTxt("jsmn_mex:invalid_token","S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY");
                //mexErrMsgIdAndTxt("jsmn_mex:no_primitive","Primitive value was not found after the comma");

            default:
                mexPrintf("-- %d --\n",next_state);
                mexErrMsgIdAndTxt("jsmn_mex:invalid_token","TODO: Fill in all errors");
        }
    }

    finish_main:
        return current_token_index+1;
                
//         int num_tokens,
//         double *values,
//         uint8_t *types,
//         int *sizes,
//         int *parents,
//         int *tokens_after_close,
//         mxArray **mxStrings
                
                
                
                
                
// // //     const double MX_NAN = mxGetNaN();
// // //
// // //     //Parser local variables
// // //     //--------------------------------------
// // //     int parser_position = parser->position;
// // //     int current_token_index = parser->current_token;
// // //     int super_token_index = parser->super_token;
// // //
// // //     int num_tokens_minus_1 = num_tokens-1;
// // //
// // //     //Frequently accessed super token attributes
// // //     //------------------------------------------
// // //     //This is true when inside an attribute
// // //     bool super_token_is_key;
// // //     //This was moved to a variable specifically for large arrays
// // //     int super_token_size;
// // //
// // //     char *pEndNumber;
// // //
// // // //     //reinitialize super if we've reallocated memory for the parser
// // // //     if (super_token_index != -1){
// // // //         super_token_is_key = types[super_token_index] == JSMN_KEY;
// // // //     }else{
// // // //         super_token_is_key = false;
// // // //     }
                
                
                
                
                
}

