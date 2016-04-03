#include "jsmn.h"
#include "stdio.h"
#include <stdlib.h>
#include "mex.h"
#include <math.h>

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
n_entries = 16;
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

void parse_string(const char *js, jsmntok_t *token, unsigned int *input_parser_position, size_t len){
    
    //TODO: This function needs work
    
    //  parse_string(js,token,parser_position)
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
            token->end = parser_position;
            *input_parser_position = parser_position;
            return;
            //goto parse_string_end;
        }
        
        //This is just error checking
        //---------------------------------------------
        /* Backslash: Quoted symbol expected */
        if (c == '\\' && parser_position + 1 < len) {
            parser_position++;
            switch (js[parser_position]) {
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
                    mexPrintf("Invalid Char: %c\n",js[parser_position]);
                    mexPrintf("Location: %d\n",parser_position);
                    mexErrMsgIdAndTxt("wtf:wtf6","Invalid escape character");
            }
        }
    }
    
    mexErrMsgIdAndTxt("wtf:wtf16","Unable to find ");
}


//Parser initialization
//----------------------------------
void jsmn_init(jsmn_parser *parser) {
	parser->pos = -1;
	parser->toknext = 0;
	parser->toksuper = -1;
}


//TODO: Implement this so that the parser is valid upon return
void refill_parser(jsmn_parser *parser,    
        unsigned int parser_position,
        unsigned int next_token_index,
        int super_token_index){
    
    
    //refill_parser(parser,parser_position,next_token_index,super_token_index)
    
    parser->pos = parser_position;
    parser->toknext = next_token_index;
    parser->toksuper = super_token_index;
}

//Parse JSON string and fill tokens.
//-------------------------------------
int jsmn_parse(jsmn_parser *parser, 
        const char *js, 
        size_t len, 
        jsmntok_t *tokens, 
        unsigned int num_tokens,
        double *values) {
    
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
    unsigned int next_token_index;
    int super_token_index;
    
    //Frequently accessed super token attributes
    //------------------------------------------
    //This is true when inside an attribute
    bool super_token_is_string; 
    //This was moved to a variable specifically for large arrays
    int super_token_size;
    
	char c;
    char *pEndNumber;
    
    jsmntype_t type;
    jsmntok_t null_super[1];
    jsmntok_t *token;
    jsmntok_t *super_token;
    
    //parser back to local variables
    parser_position   = parser->pos;
    next_token_index  = parser->toknext;
    super_token_index = parser->toksuper;
    
    
    //reinitialize super if we've reallocated memory for the parser
    if (super_token_index != -1){
        super_token = &tokens[super_token_index];
        super_token_is_string = super_token->type == JSMN_STRING;
    }else{
        //TODO: We should find the first character and make sure
        //that it is [ or {
        super_token = null_super;
        super_token_is_string = false;
    }

    //Initialization
    //---------------------------------------------------------------------
    if(parser_position == -1){
        while (is_whitespace[js[++parser_position]]) {  
        }

        switch (js[parser_position]) {
            case '{':
                goto parse_object;
                break;
            case '[':
                goto parse_array;
                break;
            default:
                mexErrMsgIdAndTxt("jsmn_mex:invalid_start","Starting token needs to be an opening object or array");
                    
        }
	}
    
    //This is for reexecuting code. I might remove it and throw in a switch ...
    //We know we have to be creating something to have exited
    //-------------  THE MAIN LOOP   --------------------------------------    
    parser_position--;
    while((c = js[++parser_position])){

		switch (c) {
			case '{':
            parse_object:
                if (next_token_index >= num_tokens) {
                    refill_parser(parser,parser_position,next_token_index,super_token_index);
                    return JSMN_ERROR_NOMEM;
                }
                
                //Initialization
                //-----------------------------
                token = &tokens[next_token_index++];
                
                token->type   = JSMN_OBJECT;
                token->start  = parser_position;
                token->end    = -1;
                //token->size //Handled below
                token->parent = super_token_index;
                token->token_after_close = -1;
                
                *values++ = 0;
                
                //Update parent to reflect that it has 1 more child
                super_token->size = ++super_token_size;
                
                
                super_token_index = next_token_index-1;
                super_token_is_string = false;
                
                //Store size on old super
                super_token->size = super_token_size;

                //Now make this the super token
                super_token = token;
                super_token_size = 0;
                super_token_is_string = false;
				super_token_index = next_token_index - 1;

                while (is_whitespace[js[++parser_position]]){  
                }
                
                switch (js[parser_position]){
                    case '"':
                        goto parse_string_label;
                        break;
                    case '}':
                        goto close_object;
                        break;
                    default:
                        mexErrMsgIdAndTxt("jsmn_mex:invalid_start","Starting token needs to be an opening object or array");
                }
                
				break;
                //---------------- End of Open Object ---------------------                
            case '[':
            parse_array:
                if (next_token_index >= num_tokens) {
                    refill_parser(parser,parser_position,next_token_index,super_token_index);
                    return JSMN_ERROR_NOMEM;
                }
                  
                super_token_size++;
                
                //Initialization
                //-----------------------------
                token = &tokens[next_token_index++];
                
                token->end    = -1;
                //token->size   = 0; Handled below
                token->token_after_close = -1;
                token->type = JSMN_ARRAY;
				token->start = parser_position;
                token->parent = super_token_index;
                
                *values++ = 0;
                
                //Need to update the super token size
                super_token->size = super_token_size;

                //Now make this the super token
                super_token = token;
                super_token_size = 0;
                super_token_is_string = false;
				super_token_index = next_token_index - 1;
				break;
                //---------------- End of Open Array ----------------------
			case '}':
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
                   super_token->size = super_token_size; 
                   super_token->token_after_close = next_token_index;
                   //Now move up to "{" character
                   super_token_index = super_token->parent;
                   super_token = &tokens[super_token_index];
                }
                //TODO: otherwise, error
                
                
                //Now we should be at the '{', but we still
                //need to move up to its parent since we are closing
                //the object
                super_token->token_after_close = next_token_index;
                super_token->end = parser_position + 1;
                
                //Since we're closing the object, we need to
                //move the super to the parent
                //TODO: If this is ever -1, then we should break to the end
                //and make sure nothing follows except whitepsace
                if(super_token->parent == -1){
                    
                    super_token->size = super_token_size;
                    
                    while (is_whitespace[js[++parser_position]]){  
                    }
                    
                    if (!(js[parser_position] == '\0')){
                        mexErrMsgIdAndTxt("jsmn_mex:invalid_end","non-whitespace characters found after end of root token close");
                    }
                    return next_token_index;
                }else{
                    super_token_index = super_token->parent;
                    super_token = &tokens[super_token_index];
                    super_token_size = super_token->size;
                    super_token_is_string = super_token->type == JSMN_STRING;
                }                   
                break;
                //--------       End of Close Object       ---------------
            case ']':
            close_array:
                //----------   Start of Close Array  ----------------------
                //Grabs the last token, why aren't we grabbing the parent
                //token?
                //[ ]
                //p x
                //
                //[ 7, 8, 9 ]
                //1 2  3  4 
                //p       x
                //
                //[ [ 7, 8, 9] ] // Note in this case we wouldn't have 
                //a token for the first close
                //1 2 3  4  5

                super_token->token_after_close = next_token_index;
                
                //This is Matlab based, we should add on a flag for this ...
                super_token->end = parser_position + 1;
                
                
                if(super_token->parent == -1){
                    
                    super_token->size = super_token_size;
                    
                    while (is_whitespace[js[++parser_position]]){  
                    }
                    
                    if (!(js[parser_position] == '\0')){
                        mexErrMsgIdAndTxt("jsmn_mex:invalid_end","non-whitespace characters found after end of root token close");
                    }
                    return next_token_index;
                }else{
                    super_token->size = super_token_size;
                    
                    super_token_index = super_token->parent;
                    super_token = &tokens[super_token_index];
                    super_token_size = super_token->size;
                    super_token_is_string = super_token->type == JSMN_STRING;
                }
				break;
                //-----------   End of Close Array     --------------------
			case '\"':
            parse_string_label:
                //---------------------------------------------------------
                //-----------  Start parsing of String --------------------
                //---------------------------------------------------------
                if (next_token_index >= num_tokens) {
                    refill_parser(parser,parser_position,next_token_index,super_token_index);
                    return JSMN_ERROR_NOMEM;
                }
                
                token = &tokens[next_token_index++];
        	    token->type   = JSMN_STRING;
         	    //TODO: For Matlab we could add 2 instead of 1
           	    token->start  = parser_position+1;
                //token->end  <= defined in loop
                //token->size <= not currently defined ...
         	    token->parent = super_token_index;
                token->token_after_close = next_token_index;

                *values++ = 0;
                
                parse_string(js,token,&parser_position,len);
                
                super_token_size++;
                
                //TODO: We may wish to make this a variable as well
                if (super_token->type == JSMN_OBJECT){
                    
                    super_token->size = super_token_size;
                    
//                     if(super_token_index == 0){
//                         mexPrintf("Size: %d\n",super_token_size);
//                     }
                    
                    //TODO: now go onto attributes
                    //----------------------------------------------
                    while (is_whitespace[js[++parser_position]]){  
                    }
                    
                    if (js[parser_position] == ':'){
                        super_token = token;
                        super_token_is_string = true;
                        super_token_size = 0;
                        super_token_index = next_token_index - 1; 
                        
                    }else{
                        //TODO: provide more info
                        mexErrMsgIdAndTxt("jsmn_mex:attribute_missing_colon","Object attribute not followed by a colon");
                    }         
                }
                
				break;
            //--------      Start of Parsing Whitespace    ---------------
            case ' '  :
            case '\n' :
			case '\t' : 
            case '\r' :
				break;
            //--------      End of Parsing Whitespace    ---------------
            //We've changed so that we explicitly get the colon after
            //any attribute
// // // // // 			case ':':
// // // // //                 //Here we are moving from a key to the attribute
// // // // //                 //We make the attribute the parent for future things
// // // // //                 
// // // // //                 //TODO: Store size with parent
// // // // //                 
// // // // //                 
// // // // //                 super_token = &tokens[next_token_index - 1];
// // // // //                 super_token_is_string = true;
// // // // //                 super_token_size = 0;
// // // // // 				super_token_index = next_token_index - 1;
// // // // // 				break;
			case ',':
                //When we get a comma inside an object we need to set
                //the super from the attribute to the parent
                //{ a : 1 , b : 2}
                //  p     x 
                //p   //Need to move p here so that the parent of 'b'
                // is the opening '{', not 'a'
                //
                //mexPrintf(",\n");
                if (super_token_is_string){
                    //This shouldn't be an interesting value ...
                    //Should always be 1
                    super_token->size = super_token_size;
                    
                    super_token_index = super_token->parent;
                    super_token = &tokens[super_token_index];
                    super_token_size = super_token->size;
                }
                
                while (is_whitespace[js[++parser_position]]){  
                }
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
                        goto parse_string_label;
                    case '{':
                        goto parse_object;
                    case '[':
                        goto parse_array;
                    default:
                        mexPrintf("Current Position: %d\n",parser_position);
                        mexErrMsgIdAndTxt("jsmn_mex:no_primitive","Primitive value was not found after the comma"); 
                    
                }
                
                
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
            parse_number:
            	if (next_token_index >= num_tokens) {
                    refill_parser(parser,parser_position,next_token_index,super_token_index);
                    return JSMN_ERROR_NOMEM;
                }
                super_token_size++;
                
                token = &tokens[next_token_index++];
                token->size   = 1;
                token->type   = JSMN_NUMBER;
                token->start  = parser_position;
                token->parent = super_token_index;

                *values++ = string_to_double(js+parser_position,&pEndNumber);

                //TODO: Check pEndNumber
                //If pEndNumber = parser->pos then we have a bad char

                //Note, this is off by 1, which is good
                //when put into Matlab
                parser_position = token->end = (int)(pEndNumber - js);                      
                token->token_after_close = next_token_index;
                parser_position--; //backtrack so we terminate on the #
                
				break;
                
            //-------        End of Number Parsing       --------------
            case 'n' :
            parse_null:
                super_token_size++;
                
                token = &tokens[next_token_index++];
                token->type   = JSMN_NUMBER;
                token->start  = parser_position;
                //TODO: Error check null - make optional with compile flag
                parser_position  += 3; //advance to final 'l' in 'null'
                //1 based indexing ...
                token->end    = parser_position+1;
                token->size   = 1;
                token->parent = super_token_index;                     
                token->token_after_close = next_token_index;
                *values++ = MX_NAN;
                //mexPrintf("Values: %f",*values);
                break;
            //-------        End of Null Parsing       --------------
			case 't':
            parse_true:
                //------------  Start of True Token  --------------
                if (next_token_index >= num_tokens) {
                    refill_parser(parser,parser_position,next_token_index,super_token_index);
                    return JSMN_ERROR_NOMEM;
                }
                super_token_size++;
                
                token = &tokens[next_token_index++];
                token->type   = JSMN_LOGICAL;
                token->start  = parser_position;
                //TODO: Error check true - make optional with compile flag
                parser_position  += 3; //advance to final 'e' in 'true'
                //1 based indexing ...
                token->end    = parser_position+1;
                token->size   = 1;
                token->parent = super_token_index;                    
                token->token_after_close = next_token_index;
                *values++ = 1;
                
//                 while (is_whitespace[js[++parser_position]]){  
//                 }
                
                //, => ok
                //} => might be ok
                
                break;
            case 'f':
            parse_false:
                //------------  Start of False Token  --------------
                if (next_token_index >= num_tokens) {
                    refill_parser(parser,parser_position,next_token_index,super_token_index);
                    return JSMN_ERROR_NOMEM;
                }
                super_token_size++;
                
                token = &tokens[next_token_index++];
                token->type   = JSMN_LOGICAL;
                token->start  = parser_position;
                //TODO: Error check true - make optional with compile flag
                parser_position  += 4; //advance to final 'e' in 'false'
                //1 based indexing ...
                token->end    = parser_position+1;
                token->size   = 1;
                token->parent = super_token_index;
                token->token_after_close = next_token_index;
                *values++  = 0;
                break;
                //-------        End of Logical Parsing      --------------


			//---------  Unexpected Character  ---------
			default:

                mexPrintf("Invalid Char: %c\n",js[parser_position]);
            	mexPrintf("Location: %d\n",parser_position);
                
				return JSMN_ERROR_INVALID_TOKEN_CHAR;
		}
	}

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

