#include "jsmn.h"
#include "stdio.h"
#include <stdlib.h>
#include "mex.h"
#include <math.h>
#include "stdint.h"  //uint_8

/*
temp = cell(1,256);
temp(:)={'false'};
temp(49:58) = {'true'};
clipboard('copy',['const bool is_number_array[256] = {' sl.cellstr.join(temp,'d',',') '};'])

%NOTE: the ascii table is actually 1 based as well so we add 1
%i.e. in Matlab, space is 32
%in c, space is also 32
%This arises since the 1st character in the ascii table starts counting at 0
temp = cell(1,256);
temp(:)={'false'};
ws_chars = [' ',sprintf('\n'),sprintf('\r'),sprintf('\t')];
temp(double(ws_chars)+1) = {'true'};
clipboard('copy',['const bool is_whitespace[256] = {' sl.cellstr.join(temp,'d',',') '};'])

%POSITIVE_VALUES
values = 1:9;
n_entries = 16;
str = cell(1,n_entries);
for i = 1:n_entries
    temp = cell(1,58);
    temp(:) = {'0'};
    temp(50:58) = arrayfun(@(x) sprintf('%d',x),values*1*10^(i-1),'un',0);
    str{i} = ['const double p1e' int2str(i-1) '[58] = {' sl.cellstr.join(temp,'d',',') '};'];
end
clipboard('copy',sl.cellstr.join(str,'d',char(10)))

values = 1:9;
%   1.json had 20 :/
n_entries = 20;
str = cell(1,n_entries);
for i = 1:n_entries
    temp = cell(1,58);
    temp(:) = {'0'};
    format = ['%0.' int2str(i) 'f'];
    temp(50:58) = arrayfun(@(x) sprintf(format,x),values.*10^(-i),'un',0);
    str{i} = ['const double p1e_' int2str(i) '[58] = {' sl.cellstr.join(temp,'d',',') '};'];
end
clipboard('copy',sl.cellstr.join(str,'d',char(10)))

 
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




//=========================================================================
//                          String Parsing
//=========================================================================
void parse_string_helper(const char *js, int *input_parser_position, size_t len){
    
    //TODO: This function needs work
    
    //      parse_string_helper(js,token,parser_position)
    int parser_position = *input_parser_position;
    char c;
    int i;
    
    //Looping over the string ...
    //---------------------------------------------------------
    while (c = js[++parser_position]) {
        
        /* Quote: end of string */
        if (c == '\"') {
            //Again, this is Matlab based, add flag and
            //handle accordingly ...
            //We're pointing to the " in c
            //which in Matlab points to the character before
            //c
            
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
    parser->is_key        = false;   
}


//TODO: Implement this so that the parser is valid upon return
void refill_parser(jsmn_parser *parser,    
        int parser_position,
        int current_token_index,
        int super_token_index,
        int is_key){
    
    
    //refill_parser(parser,parser_position,next_token_index,super_token_index,false)
    
    parser->position = parser_position;
    parser->current_token = current_token_index;
    parser->super_token = super_token_index;
    parser->is_key = is_key;
}

//=========================================================================
//              Parse JSON   -    Parse JSON    -    Parse JSON
//=========================================================================
int jsmn_parse(jsmn_parser *parser, 
        const char *js, 
        size_t len, 
        unsigned int num_tokens,
        double *values,
        uint8_t *types,
        int *starts,
        int *ends,
        int *sizes,
        int *parents,
        int *tokens_after_close) {
    
    /*
     *  Inputs
     *  ------
     *  parser :
     *    Initialized parser from jsmn_init()
     *  js :
     *    The JSON string to parse
     */
    
    const double MX_NAN = mxGetNaN();
    
    //Parser local variables
    //--------------------------------------
    int parser_position = parser->position;
    int current_token_index = parser->current_token;
    int super_token_index = parser->super_token;
    
    int num_tokens_minus_1 = num_tokens-1;
    
    //Frequently accessed super token attributes
    //------------------------------------------
    //This is true when inside an attribute
    bool super_token_is_key; 
    //This was moved to a variable specifically for large arrays
    int super_token_size;
    
    char *pEndNumber;
    
    //reinitialize super if we've reallocated memory for the parser
    if (super_token_index != -1){        
        super_token_is_key = types[super_token_index] == JSMN_KEY;
    }else{
        super_token_is_key = false;
    }

    //Initialization
    //---------------------------------------------------------------------
    if(parser_position == -1){
        while (is_whitespace[js[++parser_position]]) {  
        }

        switch (js[parser_position]) {
            case '{':
                goto parse_opening_object;
            case '[':
                goto parse_opening_array;
            default:
                mexErrMsgIdAndTxt("jsmn_mex:invalid_start","Starting token needs to be an opening object or array");
                    
        }
	}else{
    //=====================================================================
    //                          Reinitialization
    //=====================================================================
    //We will have terminated on starting a token since we exited
    //at token reallocation        
        switch(js[parser_position]){
        case '"':
            if (parser->is_key){
                goto parse_key;
            }else{
                goto parse_string;
            }
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
            goto parse_number;
        case '{':
            if(current_token_index == -1){
                goto parse_opening_object;
            }else{
                goto parse_object;
            }
        case '[':
            if(current_token_index == -1){
                goto parse_opening_array;
            }else {
                goto parse_array;
            }
        case 't':
            goto parse_true;
        case 'f':
            goto parse_false;
        case 'n':
            goto parse_null;
        default:
            mexErrMsgIdAndTxt("jsmn_mex:code_error","Unable to restart parsing following memory reallocation");
        }    
        
    }
    //----------------- End of (re)initialization -------------------------
    
    
//=========================================================================
//                       Opening an Object '{'  
//========================================================================= 
parse_object:

    //parent has 1 more object
    sizes[super_token_index] = ++super_token_size;
  
    //fall through to the opening object case
parse_opening_object:
    
    if (current_token_index >= num_tokens_minus_1) {
        refill_parser(parser,parser_position,current_token_index,super_token_index,0);
        return JSMN_ERROR_NOMEM;
    }
    ++current_token_index;

    types[current_token_index]   = JSMN_OBJECT;
    starts[current_token_index]  = parser_position+1;
    //ends - defined later
    //size - defined later
    parents[current_token_index] = super_token_index;
    //token_after_close - defined later
    values[current_token_index]  = 0;


    //Now make this the super token
    //-----------------------------
    super_token_size = 0;
    super_token_is_key = false;
    super_token_index = current_token_index;
    
    while (is_whitespace[js[++parser_position]]){  
    }

    switch (js[parser_position]){
        case '"':
            goto parse_key;
        case '}':
            goto close_empty_object;
        default:
            mexErrMsgIdAndTxt("jsmn_mex:invalid_start","An attribute or closing '}' needs to follow '{'");
    }
                
                
//=========================================================================
//                        Opening an Array '['  
//=========================================================================   
parse_array:

    sizes[super_token_index] = ++super_token_size;
    
parse_opening_array:

    if (current_token_index >= num_tokens_minus_1) {
        refill_parser(parser,parser_position,current_token_index,super_token_index,0);
        return JSMN_ERROR_NOMEM;
    }
    ++current_token_index;
    types[current_token_index]  = JSMN_ARRAY;
    starts[current_token_index] = parser_position+1;
    //ends - defined later
    //sizes - defined later
    parents[current_token_index] = super_token_index;
    //tokens_after_close - defined later
    values[current_token_index]  = 0;
    
    //Now make this the super token
    //-------------------------------
    super_token_size = 0;
    super_token_is_key = false;
    super_token_index = current_token_index;

    while (is_whitespace[js[++parser_position]]){  
    }

    switch(js[parser_position]){
        case '"':
            goto parse_string;
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
            goto parse_number;
        case '{':
            goto parse_object;
        case '[':
            goto parse_array;
        case ']':
            goto close_array;   
        case 't':
            goto parse_true;
        case 'f':
            goto parse_false;
        case 'n':
            goto parse_null;
        default:
            mexErrMsgIdAndTxt("jsmn_mex:missing_attribute_value","Character following attribute opening ':' was not recognized");
    }

//=========================================================================
//                      Closing an object   '}'   
//=========================================================================      
close_object:
    //---------------- Start of Close Object ------------------
    //We need to close out the attribute (if present)
    //and the object
    //
    //{ "test" : 1  }
    //1  2       3  4  
    //     p        x   <= p is parent/super_token

    //Closing the attribute
    //----------------------
    //if (super_token_is_key){
       //ends, for a key this is defined as the end of the string
       sizes[super_token_index] = super_token_size; //useless, == 1
       tokens_after_close[super_token_index] = current_token_index+2;

       //Now move up to "{" character
       super_token_index = parents[super_token_index];
       

    //} //else{ empty object }

//TODO: place in here close empty object - will allow removal of super_token_is_key
//-------------------------------------------------------------------------------------
close_empty_object:
    
    ends[super_token_index] = parser_position + 1;
    //sizes - only incremented at keys
    tokens_after_close[super_token_index] = current_token_index+2;
    
    if(parents[super_token_index] == -1){
        goto process_end_of_file;
    }else{
        super_token_index     = parents[super_token_index];
        super_token_size      = sizes[super_token_index];
        super_token_is_key = types[super_token_index] == JSMN_KEY;

        goto process_end_of_value;
    } 
                
                
                
//=========================================================================
//                         Closing an Array   ']'   
//========================================================================= 
close_array:
    //super_token->token_after_close = current_token_index+2;
    
    
    ends[super_token_index]  = parser_position + 1;
    sizes[super_token_index] = super_token_size;
    tokens_after_close[super_token_index] = current_token_index+2;
    
    if(parents[super_token_index] == -1){
        goto process_end_of_file;
    }else{
        super_token_index     = parents[super_token_index];
        super_token_size      = sizes[super_token_index];
        super_token_is_key = types[super_token_index] == JSMN_KEY;
        
        goto process_end_of_value;

    }

//=========================================================================
//                        Parsing an attribute { "my_attribute":
//=========================================================================
parse_key:

    if (current_token_index >= num_tokens_minus_1) {
        refill_parser(parser,parser_position,current_token_index,super_token_index,1);
        return JSMN_ERROR_NOMEM;
    }
    
    ++current_token_index;
    types[current_token_index]  = JSMN_KEY;
    starts[current_token_index] = parser_position+2;
    
    parse_string_helper(js,&parser_position,len);  
    ends[current_token_index] = parser_position;
    
    //sizes - defined later, although always == 1
    parents[current_token_index] = super_token_index;
    //token_after_close - defined later
    values[current_token_index] = 0;
    
    while (is_whitespace[js[++parser_position]]){  
    }

    if (js[parser_position] == ':'){
        //Make the attribute string the super token
        sizes[super_token_index] = ++super_token_size;
        
        super_token_is_key = true;
        super_token_size = 0;
        super_token_index = current_token_index;

        //Advance now to the next token
        while (is_whitespace[js[++parser_position]]){  
        }

        switch(js[parser_position]){
            case '"':
                goto parse_string;
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
                goto parse_number;
            case '{':
                goto parse_object;
            case '[':
                goto parse_array;
            case 't':
                goto parse_true;
            case 'f':
                goto parse_false;
            case 'n':
                goto parse_null;
            default:
                mexErrMsgIdAndTxt("jsmn_mex:missing_attribute_value","Character following attribute opening ':' was not recognized");
        }
    }else{
        //TODO: provide more info (string and position)
        mexErrMsgIdAndTxt("jsmn_mex:attribute_missing_colon","Object attribute not followed by a colon");
    }
    
    
//=========================================================================
//                        Parsing a String '"'
//========================================================================= 
parse_string:

    if (current_token_index >= num_tokens_minus_1) {
        //mexPrintf("String Position: %d\n",parser_position);
        refill_parser(parser,parser_position,current_token_index,super_token_index,0);
        return JSMN_ERROR_NOMEM;
    }
    ++super_token_size;
    
    ++current_token_index;    
    types[current_token_index] = JSMN_STRING;    
    starts[current_token_index] = parser_position+2;
    
    parse_string_helper(js,&parser_position,len);
    ends[current_token_index] = parser_position;
    
    //sizes - not currently defined
    parents[current_token_index] = super_token_index;
    tokens_after_close[current_token_index] = current_token_index+1;
    values[current_token_index] = 0;

    
    //The string is not an attribute, so we could
    // have another value, or close the current object/array
    goto process_end_of_value;
                     
//=========================================================================
//                        Parsing a Comma ','
//=========================================================================
parse_comma:
    //When we get a comma inside an object we need to set
    //the super from the attribute to the parent
    //{ a : 1 , b : 2}
    //  p     x 
    //p   //Need to move p here so that the parent of 'b'
    // is the opening '{', not 'a'
    //
    //mexPrintf(",\n");
    
    while (is_whitespace[js[++parser_position]]){  
    }
    
    //TODO: We could have 1 version for within arrays and another
    //for within objects ...
    if (super_token_is_key){
        //ends, for a key this is defined as the end of the string
        sizes[super_token_index] = super_token_size; //always 1 ...
        tokens_after_close[super_token_index] = current_token_index+2;
        

        super_token_index = parents[super_token_index];
        super_token_size  = sizes[super_token_index];
        super_token_is_key = false;
        
        if (js[parser_position] == '"'){
            goto parse_key;    
        }else{
            mexErrMsgIdAndTxt("jsmn_mex:no_key","Key expected");
        }
        
    }else{
        //primitive required
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
                goto parse_number;
            case 'n':
                goto parse_null;
            case 't':
                goto parse_true;
            case 'f':
                goto parse_false;
            case '"':
                goto parse_string;
            case '{':
                goto parse_object;
            case '[':
                goto parse_array;
            default:
                mexErrMsgIdAndTxt("jsmn_mex:no_primitive","Primitive value was not found after the comma");
        }
    }
    
//=========================================================================
//                        Parsing a Number '-0123458789'
//=========================================================================
    

parse_number:
    if (current_token_index >= num_tokens_minus_1) {
        refill_parser(parser,parser_position,current_token_index,super_token_index,0);
        return JSMN_ERROR_NOMEM;
    }
    super_token_size++;
    ++current_token_index;
    types[current_token_index] = JSMN_NUMBER;
    starts[current_token_index] = parser_position+1;
    sizes[current_token_index] = 1;
    parents[current_token_index] = super_token_index;
    
    
    values[current_token_index] = string_to_double(js+parser_position,&pEndNumber);
    parser_position = (int)(pEndNumber - js);  
    ends[current_token_index] = parser_position;
    tokens_after_close[current_token_index] = current_token_index+2;
    parser_position--; //backtrack so we terminate on the #

    goto process_end_of_value;


//=========================================================================
//                        Parsing a Null 'null'
//=========================================================================    
parse_null:
    if (current_token_index >= num_tokens_minus_1) {
        refill_parser(parser,parser_position,current_token_index,super_token_index,0);
        return JSMN_ERROR_NOMEM;
    }    
    super_token_size++;
    ++current_token_index;
    types[current_token_index]   = JSMN_NUMBER;
    starts[current_token_index]  = parser_position+1;
    //TODO: Check null
    parser_position  += 3; //advance to final 'l' in 'null'
    ends[current_token_index] = parser_position+1;
    sizes[current_token_index]   = 1;
    parents[current_token_index] = super_token_index;
    tokens_after_close[current_token_index] = current_token_index+2;
    values[current_token_index]  = MX_NAN;

    goto process_end_of_value;

//=========================================================================
//                        Parsing a True 'true'
//========================================================================    

parse_true:
    //------------  Start of True Token  --------------
    if (current_token_index >= num_tokens_minus_1) {
        refill_parser(parser,parser_position,current_token_index,super_token_index,0);
        return JSMN_ERROR_NOMEM;
    }
    super_token_size++;
    ++current_token_index;
    types[current_token_index]   = JSMN_LOGICAL;
    starts[current_token_index]  = parser_position+1;
    //TODO: Error check true - make optional with compile flag
    parser_position  += 3; //advance to final 'e' in 'true'
    ends[current_token_index]    = parser_position+1;
    sizes[current_token_index]   = 1;
    parents[current_token_index] = super_token_index;
    tokens_after_close[current_token_index] = current_token_index+2;
    values[current_token_index]  = 1;

    goto process_end_of_value;

//=========================================================================
//                        Parsing a False 'false'
//=========================================================================                 
parse_false:
    //------------  Start of False Token  --------------
    if (current_token_index >= num_tokens_minus_1) {
        refill_parser(parser,parser_position,current_token_index,super_token_index,0);
        return JSMN_ERROR_NOMEM;
    }
    super_token_size++;
    ++current_token_index;
    types[current_token_index]   = JSMN_LOGICAL;
    starts[current_token_index]  = parser_position+1;
    //TODO: Error check true 
    parser_position  += 4; //advance to final 'e' in 'false'
    ends[current_token_index]    = parser_position+1;
    sizes[current_token_index]   = 1;
    parents[current_token_index] = super_token_index;
    tokens_after_close[current_token_index] = current_token_index+2;
    values[current_token_index]  = 0;

    goto process_end_of_value;
                                
//=============================================================
//                  End of Value Handling    
//=============================================================
process_end_of_value:
        while (is_whitespace[js[++parser_position]]){
        }

        switch(js[parser_position]){
            case ',':
               goto parse_comma; 
            case '}':
               if (super_token_is_key){
                  goto close_object; 
               }else{
                  mexErrMsgIdAndTxt("jsmn_mex:invalid_close","close object '}' is not matched with an open object");   
               }
            case ']':
               if (types[super_token_index] == JSMN_ARRAY){
                  goto close_array;
               }else{                  
                  mexErrMsgIdAndTxt("jsmn_mex:invalid_close","close array ']' is not matched with an open array"); 
               }
            default :  
                mexErrMsgIdAndTxt("jsmn_mex:invalid_token","String value must be followed by a comma or a closing token");
        }

 //========================================================================
 //                         Process End of File
 //========================================================================
 //At this point the super_token_index is -1 meaning we've closed the 
 //opening object or array. Proceed to the end of file throwing an error
 //if something besides whitespace is found.
 process_end_of_file:
     while (is_whitespace[js[++parser_position]]){  
        }

        if (!(js[parser_position] == '\0')){            
            mexErrMsgIdAndTxt("jsmn_mex:invalid_end","non-whitespace characters found after end of root token close");
        }
     return current_token_index+1;
    
}

