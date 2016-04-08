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
    int n_numeric_chars_plus_1;
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




//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//                  String Parsing
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void parse_string_helper(const char *js, unsigned int *input_parser_position, size_t len){
    
    //TODO: This function needs work
    
    //      parse_string_helper(js,token,parser_position)
    unsigned int parser_position = *input_parser_position;
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
	parser->pos = -1;
	parser->toknext = 0;
	parser->toksuper = -1;
    parser->is_key = false;
}


//TODO: Implement this so that the parser is valid upon return
void refill_parser(jsmn_parser *parser,    
        unsigned int parser_position,
        int next_token_index,
        int super_token_index,
        int is_key){
    
    
    //refill_parser(parser,parser_position,next_token_index,super_token_index,false)
    
    parser->pos = parser_position;
    parser->toknext = next_token_index;
    parser->toksuper = super_token_index;
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
    unsigned int parser_position;
    int next_token_index;
    int current_token_index;
    int super_token_index;
    int num_tokens_minus_1 = num_tokens-1;
    
    //Frequently accessed super token attributes
    //------------------------------------------
    //This is true when inside an attribute
    bool super_token_is_string; 
    //This was moved to a variable specifically for large arrays
    int super_token_size;
    
	char c;
    char *pEndNumber;
    
    //jsmntype_t type;
//     jsmntok_t null_super[1];
//     jsmntok_t *token;
//     jsmntok_t *super_token;
    
    uint8_t *types_array = types;
    
    //parser back to local variables
    parser_position   = parser->pos;
    next_token_index  = parser->toknext;
    super_token_index = parser->toksuper;
    
    current_token_index = next_token_index-1;
    
    //reinitialize super if we've reallocated memory for the parser
    if (super_token_index != -1){
        
        //Need + 1 because we do:
        //*types++; so we are constantly point 1 ahead ...
        //assign 1st value (current_token_index == 0) then
        //point to 2
        //types = &types[current_token_index] + 1;
        values = &values[current_token_index] + 1;
        
        //starts = &starts[current_token_index];
        
        //mexPrintf("wtf: %d\n",types-types_array);
        //mexPrintf("wtf2: %d\n",current_token_index);
        
        //super_token = &tokens[super_token_index];
        
        super_token_is_string = types_array[super_token_index] == JSMN_STRING;
    }else{
        //TODO: We should find the first character and make sure
        //that it is [ or {
        //super_token = null_super;
        super_token_is_string = false;
    }

    //Initialization
    //---------------------------------------------------------------------
    if(parser_position == -1){
        while (is_whitespace[js[++parser_position]]) {  
        }

        switch (js[parser_position]) {
            case '{':
                goto parse_opening_object;
                break;
            case '[':
                goto parse_opening_array;
                break;
            default:
                mexErrMsgIdAndTxt("jsmn_mex:invalid_start","Starting token needs to be an opening object or array");
                    
        }
	}else{
    //=====================================================================
    //                          Reinitialization
    //=====================================================================
    //We will have terminated on starting a token since we exited
    //at token reallocation
        
        
    
        //mexPrintf("Reinit\n");
        
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

    sizes[super_token_index] = ++super_token_size;
    
parse_opening_object:
    
    if (current_token_index >= num_tokens_minus_1) {
        //mexPrintf("Object Position: %d\n",parser_position);

        refill_parser(parser,parser_position,current_token_index+1,super_token_index,0);
        return JSMN_ERROR_NOMEM;
    }
    ++current_token_index;
    //Initialization
    //-----------------------------
    //token = &tokens[++current_token_index];

    //*types++ = JSMN_OBJECT;
    
    types[current_token_index] = JSMN_OBJECT;
    
    //token->type   = JSMN_OBJECT;
    //token->start  = parser_position+1;
    starts[current_token_index] = parser_position+1;
    
    //token->end    = -1;
    //token->size //Handled below
    //token->parent = super_token_index;
    parents[current_token_index] = super_token_index;
    //token->token_after_close = -1;

    *values++ = 0;
    

    //Update parent to reflect that it has 1 more child
    

    //Now make this the super token
    //-----------------------------
    //super_token = token;
    super_token_size = 0;
    super_token_is_string = false;
    super_token_index = current_token_index;
    //super_token_type = JSMN_OBJECT;
    
    //Process the attribute
    //---------------------
    while (is_whitespace[js[++parser_position]]){  
    }

    switch (js[parser_position]){
        case '"':
            //Found first attribute
            goto parse_key;
        case '}':
            
            //TODO: We could throw some code in here ...
            
            //We have an empty object
            goto close_object;
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
        //mexPrintf("Array Position: %d\n",parser_position);

        refill_parser(parser,parser_position,current_token_index+1,super_token_index,0);
        return JSMN_ERROR_NOMEM;
    }
    ++current_token_index;
    //Initialization
    //-----------------------------
    //token = &tokens[++current_token_index];

    //token->end    = -1;
    //token->size   = 0; Handled below
    //token->token_after_close = -1;
    //token->type   = JSMN_ARRAY;
    //*types++ = JSMN_ARRAY;
    types[current_token_index] = JSMN_ARRAY;
    
    //token->start  = parser_position+1;
    starts[current_token_index] = parser_position+1;
    
    
    //token->parent = super_token_index;
    
    parents[current_token_index] = super_token_index;

    *values++ = 0;

    //Need to update the super token size
    

    //Now make this the super token
    //-------------------------------
    //super_token = token;
    super_token_size = 0;
    super_token_is_string = false;
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
    if (super_token_is_string){
       //Again, this may be useless since it should be 1
       //super_token->size = super_token_size; 
       sizes[super_token_index] = super_token_size;
       tokens_after_close[super_token_index] = current_token_index-1;
       
       //super_token->token_after_close = current_token_index-1;
       
       //Now move up to "{" character
       //super_token_index = super_token->parent;
       super_token_index = parents[super_token_index];
       
       //super_token = &tokens[super_token_index];
    } //else{ empty object }

    
    
    //Note, we don't handle size because size is only incremented
    //by attributes
    //super_token->size = super_token_size;
    
    //Now we should be at the '{', but we still
    //need to move up to its parent since we are closing
    //the object as well, not just the attribute
    //super_token->token_after_close = current_token_index+2;
    tokens_after_close[super_token_index] = current_token_index+2;
    
    //super_token->end = parser_position + 1;
    ends[super_token_index] = parser_position + 1;
    
    
    if(parents[super_token_index] == -1){
        goto process_end_of_file;
    }else{
        //super_token_index = super_token->parent;
        super_token_index = parents[super_token_index];
        
        //super_token = &tokens[super_token_index];
        //super_token_size = super_token->size;
        super_token_size      = sizes[super_token_index];
        super_token_is_string = types_array[super_token_index] == JSMN_STRING;

        goto process_end_of_token;
    } 
                
                
                
//=========================================================================
//                         Closing an Array   ']'   
//========================================================================= 
close_array:
    //super_token->token_after_close = current_token_index+2;
    tokens_after_close[super_token_index] = current_token_index+2;
    
    //super_token->end  = parser_position + 1;
    ends[super_token_index] = parser_position + 1;
    
    //super_token->size = super_token_size;
    sizes[super_token_index] = super_token_size;

    //mexPrintf("SuperTokenIndex: %d\n",super_token_index);
    
    if(parents[super_token_index] == -1){
        goto process_end_of_file;
    }else{
        //super_token_index     = super_token->parent;
        super_token_index = parents[super_token_index];
        
        //super_token           = &tokens[super_token_index];
        super_token_size      = sizes[super_token_index];
        //super_token_size      = super_token->size;
        
//         mexPrintf("Sup index: %d\n",super_token_index);
//         mexPrintf("Sup type: %d\n",types_array[super_token_index]);
        
        super_token_is_string = types_array[super_token_index] == JSMN_STRING;

//         mexPrintf("Sup is string: %d\n",super_token_is_string);
        
        goto process_end_of_token;

    }

//=========================================================================
//                        Parsing an attribute
//=========================================================================
parse_key:

    if (current_token_index >= num_tokens_minus_1) {
        //mexPrintf("Key Position: %d\n",parser_position);

        refill_parser(parser,parser_position,current_token_index+1,super_token_index,1);
        return JSMN_ERROR_NOMEM;
    }
    ++current_token_index;
    //token = &tokens[++current_token_index];
    //token->type   = JSMN_STRING; //Different type?
    
    //*types++ = JSMN_STRING;
    types[current_token_index] = JSMN_STRING;
    
    
    //token->start  = parser_position+2;
    starts[current_token_index] = parser_position+2;
    
    //token->end  <= defined in loop
    //token->size <= not currently defined ...
    //token->parent = super_token_index;
    parents[current_token_index] = super_token_index;
    
    //TODO: I'm not sure that this is right ...
    //We want to point to the 
    //token->token_after_close = current_token_index+2;
    //tokens_after_close[current_token_index] = current_token_index+2;

    *values++ = 0;

    parse_string_helper(js,&parser_position,len);  
    ends[current_token_index] = parser_position;
    
    while (is_whitespace[js[++parser_position]]){  
    }

    if (js[parser_position] == ':'){
        //Make the attribute string the super token
        //super_token->size = ++super_token_size;
        sizes[super_token_index] = ++super_token_size;
        
        //super_token = token;
        super_token_is_string = true;
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
        refill_parser(parser,parser_position,current_token_index+1,super_token_index,0);
        return JSMN_ERROR_NOMEM;
    }

    ++current_token_index;    
    //token = &tokens[++current_token_index];
    //*types++ = JSMN_STRING;
    types[current_token_index] = JSMN_STRING;
    
    
    //token->type   = JSMN_STRING;
    //token->start  = parser_position+2;
    
    starts[current_token_index] = parser_position+2;
    
    //token->end  <= defined in loop
    //token->size <= not currently defined ...
    //token->parent = super_token_index;
    parents[current_token_index] = super_token_index;
    
    //token->token_after_close = current_token_index+1;
    tokens_after_close[current_token_index] = current_token_index+1;
    
    *values++ = 0;

    parse_string_helper(js,&parser_position,len);
    //token->end = parser_position;
    ends[current_token_index] = parser_position;
    
    ++super_token_size;
    //The string is not an attribute, so we could
    // have another value, or close the current object/array
    goto process_end_of_token;
                     
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
    
    //mexPrintf("parsing comma: %d\n",parser_position);
    
    if (super_token_is_string){
        //This shouldn't be an interesting value ...
        //Should always be 1 (an attribute has 1 value)
        //Could harcode this and remove this line
        //Need better tests ...
        //super_token->size = super_token_size;
        
        sizes[super_token_index] = super_token_size;
        tokens_after_close[super_token_index] = current_token_index+2;
        
        
        
        //super_token_index = super_token->parent;
        super_token_index = parents[super_token_index];
        
        //super_token       = &tokens[super_token_index];
        //super_token_size  = super_token->size;
        super_token_size  = sizes[super_token_index];
        super_token_is_string = false;
        
        if (js[parser_position] == '"'){
            goto parse_key;    
        }else{
//             mexPrintf("Position: %d\n",parser_position);
//             mexPrintf("Char 1: %c\n",js[parser_position-3]);
//             mexPrintf("Char 1: %c\n",js[parser_position-2]);
//             mexPrintf("Char 1: %c\n",js[parser_position-1]);
//             mexPrintf("Char 1: %c\n",js[parser_position]);
//             mexPrintf("Char 1: %c\n",js[parser_position+1]);
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
                //mexPrintf("Current Position: %d\n",parser_position);
                mexErrMsgIdAndTxt("jsmn_mex:no_primitive","Primitive value was not found after the comma");
        }
    }
    
//=========================================================================
//                        Parsing a Number '-0123458789'
//=========================================================================
    

parse_number:
    if (current_token_index >= num_tokens_minus_1) {
//         mexPrintf("Number Position: %d\n",parser_position);
//         mexPrintf("Current TOken pos: %d\n",current_token_index);
//         mexPrintf("p diff: %d\n",types-types_array);
        refill_parser(parser,parser_position,current_token_index+1,super_token_index,0);
        return JSMN_ERROR_NOMEM;
    }
    super_token_size++;
    ++current_token_index;

    //mexPrintf("Current token index: %d\n",current_token_index);
    
    //token = &tokens[++current_token_index];
    //token->size   = 1;
    sizes[current_token_index] = 1;
    //token->type   = JSMN_NUMBER;

    
    
    //*types++ = JSMN_NUMBER;
    types[current_token_index] = JSMN_NUMBER;
    
    //token->start  = parser_position+1;
    
    starts[current_token_index] = parser_position+1;
    
    //token->parent = super_token_index;
    parents[current_token_index] = super_token_index;

    *values++ = string_to_double(js+parser_position,&pEndNumber);

    //TODO: Check pEndNumber
    //If pEndNumber = parser->pos then we have a bad char

    //Note, this is off by 1, which is good
    //when put into Matlab
    parser_position = (int)(pEndNumber - js);  
    ends[current_token_index] = parser_position;
    
    //token->token_after_close = current_token_index+2;
    tokens_after_close[current_token_index] = current_token_index+2;
    
    parser_position--; //backtrack so we terminate on the #

    goto process_end_of_token;


//=========================================================================
//                        Parsing a Null 'null'
//=========================================================================    
parse_null:
    if (current_token_index >= num_tokens_minus_1) {
        refill_parser(parser,parser_position,current_token_index+1,super_token_index,0);
        return JSMN_ERROR_NOMEM;
    }    
    super_token_size++;
    ++current_token_index;

    //token = &tokens[++current_token_index];
    //token->type   = JSMN_NUMBER;
    //*types++ = JSMN_NUMBER;
    
    types[current_token_index] = JSMN_NUMBER;
    
    //token->start  = parser_position+1;
    starts[current_token_index] = parser_position+1;
    
    
    //TODO: Error check null - make optional with compile flag
    parser_position  += 3; //advance to final 'l' in 'null'
    //1 based indexing ...
    //token->end    = parser_position+1;
    ends[current_token_index] = parser_position+1;
    
    //token->size   = 1;
    sizes[current_token_index] = 1;
    
    //token->parent = super_token_index;
    parents[current_token_index] = super_token_index;
    
    //token->token_after_close = current_token_index+2;
    tokens_after_close[current_token_index] = current_token_index+2;
    *values++ = MX_NAN;

    goto process_end_of_token;

//=========================================================================
//                        Parsing a True 'true'
//========================================================================    

parse_true:
    //------------  Start of True Token  --------------
    if (current_token_index >= num_tokens_minus_1) {
        refill_parser(parser,parser_position,current_token_index+1,super_token_index,0);
        return JSMN_ERROR_NOMEM;
    }
    super_token_size++;
    ++current_token_index;

    //token = &tokens[++current_token_index];
    //token->type   = JSMN_LOGICAL;
    
    //*types++ = JSMN_LOGICAL;
    types[current_token_index] = JSMN_LOGICAL;
    
    //token->start  = parser_position+1;
    
    starts[current_token_index] = parser_position+1;
    
    //TODO: Error check true - make optional with compile flag
    parser_position  += 3; //advance to final 'e' in 'true'
    //1 based indexing ...
    //token->end    = parser_position+1;
    ends[current_token_index] = parser_position+1;
    
    
    //token->size   = 1;
    sizes[current_token_index] = 1;
    
    //token->parent = super_token_index; 
    parents[current_token_index] = super_token_index;
    
    
    //token->token_after_close = current_token_index+2;
    tokens_after_close[current_token_index] = current_token_index+2;
    *values++ = 1;

    goto process_end_of_token;

//=========================================================================
//                        Parsing a False 'false'
//=========================================================================                 
parse_false:
    //------------  Start of False Token  --------------
    if (current_token_index >= num_tokens_minus_1) {
        refill_parser(parser,parser_position,current_token_index+1,super_token_index,0);
        return JSMN_ERROR_NOMEM;
    }
    super_token_size++;
    ++current_token_index;
    
    //token = &tokens[++current_token_index];
    //token->type   = JSMN_LOGICAL;
    
    //*types++ = JSMN_LOGICAL;
    types[current_token_index] = JSMN_LOGICAL;
    
    
    //token->start  = parser_position+1;
    starts[current_token_index] = parser_position+1;
    
    //TODO: Error check true - make optional with compile flag
    parser_position  += 4; //advance to final 'e' in 'false'
    //1 based indexing ...
    //token->end    = parser_position+1;
    
    ends[current_token_index] = parser_position+1;
    
    //token->size   = 1;
    sizes[current_token_index] = 1;
    
    //token->parent = super_token_index;
    parents[current_token_index] = super_token_index;
    
    //token->token_after_close = current_token_index+2;
    tokens_after_close[current_token_index] = current_token_index+2;
    *values++  = 0;

    goto process_end_of_token;
                                
//=============================================================
//                  End of Token Handling    
//=============================================================
//
//TODO: process_end_of_value would be a better name
//
process_end_of_token:
        while (is_whitespace[js[++parser_position]]){
        }

        switch(js[parser_position]){
            case ',':
               goto parse_comma; 
            case '}':
               if (super_token_is_string){
                  goto close_object; 
               }else{
                  mexErrMsgIdAndTxt("jsmn_mex:invalid_close","close object '}' is not matched with an open object");   
               }
            case ']':
               //TODO: Check that we've opened an array
               if (types_array[super_token_index] == JSMN_ARRAY){
                  goto close_array;
               }else{
//             mexPrintf("Types: %d\n",types_array[0]);
//             mexPrintf("Types: %d\n",types_array[1]);
//             mexPrintf("Types: %d\n",types_array[2]);
//             mexPrintf("Types: %d\n",types_array[3]);
//             mexPrintf("Types: %d\n",types_array[4]);
//             mexPrintf("Types: %d\n",types_array[5]);
//             mexPrintf("Types: %d\n",types[0]);
//             mexPrintf("Types: %d\n",types[1]);
//             mexPrintf("Types: %d\n",types[2]);
//             mexPrintf("Types: %d\n",types[3]);
//             mexPrintf("Types: %d\n",types[4]);
//             mexPrintf("Types: %d\n",types[5]);
//             
//             mexPrintf("STI: %d\n",super_token_index);       
//             mexPrintf("Position: %d\n",parser_position);
//             mexPrintf("Char 1: %c\n",js[parser_position-3]);
//             mexPrintf("Char 1: %c\n",js[parser_position-2]);
//             mexPrintf("Char 1: %c\n",js[parser_position-1]);
//             mexPrintf("Char 1: %c\n",js[parser_position]);
//             mexPrintf("Char 1: %c\n",js[parser_position+1]);
                  
                  mexErrMsgIdAndTxt("jsmn_mex:invalid_close","close array ']' is not matched with an open array"); 
               }
            default :  
                
//                                         mexPrintf("STI: %d\n",super_token_index);       
//             mexPrintf("Position: %d\n",parser_position);
//             mexPrintf("Char 1: %c\n",js[parser_position-3]);
//             mexPrintf("Char 1: %c\n",js[parser_position-2]);
//             mexPrintf("Char 1: %c\n",js[parser_position-1]);
//             mexPrintf("Char 1: %c\n",js[parser_position]);
//             mexPrintf("Char 1: %c\n",js[parser_position+1]);
                
                
                mexErrMsgIdAndTxt("jsmn_mex:invalid_token","String value must be followed by a comma or a closing token");
        }

 process_end_of_file:
     while (is_whitespace[js[++parser_position]]){  
        }

        if (!(js[parser_position] == '\0')){
            
//                         mexPrintf("STI: %d\n",super_token_index);       
//             mexPrintf("Position: %d\n",parser_position);
//             mexPrintf("Char 1: %c\n",js[parser_position-3]);
//             mexPrintf("Char 1: %c\n",js[parser_position-2]);
//             mexPrintf("Char 1: %c\n",js[parser_position-1]);
//             mexPrintf("Char 1: %c\n",js[parser_position]);
//             mexPrintf("Char 1: %c\n",js[parser_position+1]);
            
            
            mexErrMsgIdAndTxt("jsmn_mex:invalid_end","non-whitespace characters found after end of root token close");
        }
        return current_token_index+1;
                    
                
// 		}
// 	}

    mexErrMsgIdAndTxt("jsmn_mex:failed_to_close","Opening object or array was not closed before end of the string");
    
    /* ERROR CHECKING ONLY
     //TODO: Replace with open counts for { and [
    //I think this is just a check at the end, can we remove
    //this or just replace it with a counter or some depth measurement?
    for (i = next_token_index - 1; i >= 0; i--) {
        if (tokens[i].start != -1 && tokens[i].end == -1) {
            return JSMN_ERROR_PART;
        }
    }
     */
    
    //TODO: Put in an error
    
    //0 based indexing, next token 1 means a count of 1 token
	
    
}

