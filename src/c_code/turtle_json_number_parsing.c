#include "turtle_json.h"

//This file contains code for parsing strings to numbers. There are 
//currently two different approaches implemented, but only 1 is used.
//
//Eventually I wanted to have explicit speed testing that compared various 
//approaches.

//Place to put various # parsing algorithms ...
//https://github.com/miloyip/rapidjson/blob/03a73910498d784a3a9429202a90d2fb67be910b/include/rapidjson/reader.h#L1118



//Values for Integer portion of number
//------------------------------------
//Note that I'm avoiding subtracting 0 which makes these arrays rather large ...
//TODO: test time difference on subtracting
//TODO: Integer math would probably be better, with double conversion ...
//
//This code allows us to do double_value = ple#[current_character]
//
//Subtracting 0 would mean we would do double_value = ple#[current_character - '0']
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
const double p1e_21[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000000000000000001,0.000000000000000000002,0.000000000000000000003,0.000000000000000000004,0.000000000000000000005,0.000000000000000000006,0.000000000000000000007,0.000000000000000000008,0.000000000000000000009};
const double p1e_22[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000000000000000001,0.0000000000000000000002,0.0000000000000000000003,0.0000000000000000000004,0.0000000000000000000005,0.0000000000000000000006,0.0000000000000000000007,0.0000000000000000000008,0.0000000000000000000009};
const double p1e_23[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000000000000000001,0.00000000000000000000002,0.00000000000000000000003,0.00000000000000000000004,0.00000000000000000000005,0.00000000000000000000006,0.00000000000000000000007,0.00000000000000000000008,0.00000000000000000000009};
const double p1e_24[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000000000000000000001,0.000000000000000000000002,0.000000000000000000000003,0.000000000000000000000004,0.000000000000000000000005,0.000000000000000000000006,0.000000000000000000000007,0.000000000000000000000008,0.000000000000000000000009};
const double p1e_25[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000000000000000000001,0.0000000000000000000000002,0.0000000000000000000000003,0.0000000000000000000000004,0.0000000000000000000000005,0.0000000000000000000000006,0.0000000000000000000000007,0.0000000000000000000000008,0.0000000000000000000000009};
const double p1e_26[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000000000000000000001,0.00000000000000000000000002,0.00000000000000000000000003,0.00000000000000000000000004,0.00000000000000000000000005,0.00000000000000000000000006,0.00000000000000000000000007,0.00000000000000000000000008,0.00000000000000000000000009};
const double p1e_27[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000000000000000000000001,0.000000000000000000000000002,0.000000000000000000000000003,0.000000000000000000000000004,0.000000000000000000000000005,0.000000000000000000000000006,0.000000000000000000000000007,0.000000000000000000000000008,0.000000000000000000000000009};
const double p1e_28[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000000000000000000000001,0.0000000000000000000000000002,0.0000000000000000000000000003,0.0000000000000000000000000004,0.0000000000000000000000000005,0.0000000000000000000000000006,0.0000000000000000000000000007,0.0000000000000000000000000008,0.0000000000000000000000000009};
const double p1e_29[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000000000000000000000001,0.00000000000000000000000000002,0.00000000000000000000000000003,0.00000000000000000000000000004,0.00000000000000000000000000005,0.00000000000000000000000000006,0.00000000000000000000000000007,0.00000000000000000000000000008,0.00000000000000000000000000009};
const double p1e_30[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000000000000000000000000001,0.000000000000000000000000000002,0.000000000000000000000000000003,0.000000000000000000000000000004,0.000000000000000000000000000005,0.000000000000000000000000000006,0.000000000000000000000000000007,0.000000000000000000000000000008,0.000000000000000000000000000009};




//*p++                  js[parser_position++]
//*p   ==========       js[parser_position]
//++p  ==========       ++parser_position

void string_to_double(double *value_p, char *p, int i, int *error_p, int *error_value) {
    
    /*
     *  I found atof to be the main bottleneck in this code. It was
     *  suggested to me by some internet searching that a custom algorithm
     *  would be close enough in 99.99 of the cases. I found a couple
     *  basic ones online that used loops and multiplation. This was my
     *  attempt to make something as fast possible. I welcome a faster
     *  approach!
     *
     *  I think that it would be worthwhile to investigate trying
     *  to populate integer and fraction arrays and then add them together
     *  using intrinsics
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
    while (isdigit(*p++)) {}
    
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
            
            *error_p = i+1;
            *error_value = 0;
            return;
            //TODO: This will likely cause a problem since we have multiple threads
            //mexErrMsgIdAndTxt("turtle_json:no_number","No numbers were found, at position: %d");
        default:
            *error_p = i+1;
            *error_value = 1;
            return;
            //mexErrMsgIdAndTxt("turtle_json:too_many_integers","The integer component of the number had too many digits");
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
        //TODO: Is no digit ok? => no, this needs to be handled
        if(isdigit(*p)){
            value += p1e_1[*p++];
            if(isdigit(*p)){
                value += p1e_2[*p++];
                if(isdigit(*p)){
                    value += p1e_3[*p++];
                    if(isdigit(*p)){
                        value += p1e_4[*p++];
                        if(isdigit(*p)){
                            value += p1e_5[*p++];
                            if(isdigit(*p)){
                                value += p1e_6[*p++];
                                if(isdigit(*p)){
                                    value += p1e_7[*p++];
                                    if(isdigit(*p)){
                                        value += p1e_8[*p++];
                                        if(isdigit(*p)){
                                            value += p1e_9[*p++];
                                            if(isdigit(*p)){
                                                value += p1e_10[*p++];
                                                if(isdigit(*p)){
                                                    value += p1e_11[*p++];
                                                    if(isdigit(*p)){
                                                        value += p1e_12[*p++];
                                                        if(isdigit(*p)){
                                                            value += p1e_13[*p++];
                                                            if(isdigit(*p)){
                                                                value += p1e_14[*p++];
                                                                if(isdigit(*p)){
                                                                    value += p1e_15[*p++];
                                                                    if(isdigit(*p)){
                                                                        value += p1e_16[*p++];
                                                                        if(isdigit(*p)){
                                                                            value += p1e_17[*p++];
                                                                            if(isdigit(*p)){
                                                                                value += p1e_18[*p++];
                                                                                if(isdigit(*p)){
                                                                                    value += p1e_19[*p++];
                                                                                    if(isdigit(*p)){
                                                                                        value += p1e_20[*p++];
                                                                                        if(isdigit(*p)){
                                                                                            value += p1e_21[*p++];
                                                                                            if(isdigit(*p)){
                                                                                                value += p1e_22[*p++];
                                                                                                if(isdigit(*p)){
                                                                                                    value += p1e_23[*p++];
                                                                                                    if(isdigit(*p)){
                                                                                                        value += p1e_24[*p++];
                                                                                                        if(isdigit(*p)){
                                                                                                            value += p1e_25[*p++];
                                                                                                            if(isdigit(*p)){
                                                                                                                *error_p = i+1;
                                                                                                                *error_value = 3;
                                                                                                                return;
                                                                                                                //mexErrMsgIdAndTxt("turtle_json:too_many_decimals","The fractional component of the number had too many digits");

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
                    }
                }
            }
        }else{
            //No number following the period
            *error_p = i+1;
            *error_value = 2;
            return;
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
        while (isdigit(*p++)) {}
        
        exponent_value = 0;
        n_numeric_chars_plus_1 = p-number_start;
        //reset p so that we can increment our way down
        p = number_start;
        switch (n_numeric_chars_plus_1) {
            case 4:
                exponent_value += p1e2[*p++];
            case 3:
                exponent_value += p1e1[*p++];
            case 2:
                exponent_value += p1e0[*p++];
                break;
            case 1:
                mexErrMsgIdAndTxt("turtle_json:empty_exponent","An exponent was given with no numeric value");
            default:
                //TODO: Give error location in string
                mexErrMsgIdAndTxt("turtle_json:large_exponent","There were more than 3 digits in a numeric exponent");
        }
        if (negate){
            exponent_value = -exponent_value;
        }
        value *= pow(10.0, exponent_value);
    }
    
    //TODO: I need to check for invalid values at the end of the string
    //With our string skipping we skip all parts of the number so
    //something like this would get by:
    //  1.2345E123E
    //  as would
    //  1.2345.123432
    //
    //  valid characters at the end would be:
    //  space } ] , 
    //  comma is probably the most likely
    // 
    //  Actually, we just need a lookup on # values
    //  in particular non-numeric parts of the number
    //  (so '.' , '-', 'e', and 'E') (numbers would just be parsed)
    //
    
    *value_p = value;
}

void string_to_double_v2(double *value_p, char *p, int i, int *error_p, int *error_value) {

    //errors
    //1 - no lead number

//                case 0:
//                     //I don't think this can run based on how our parser works ...
//                     //TODO: Change this to a code error?
//                     mexErrMsgIdAndTxt("turtle_json:no_integer_component", \
//                             "No integer component was found for a number (#%d in the file, at position %d)", \
//                             error_index+1,first_char_of_bad_number-js+1);
//                     break;
//                 case 1:
//                     mexErrMsgIdAndTxt("turtle_json:integer_component_too_large",
//                             "The integer component of the number had too many digits (#%d in the file, at position %d)",
//                             error_index+1,first_char_of_bad_number-js+1);
//                     break;
//                 case 2:
//                     mexErrMsgIdAndTxt("turtle_json:no_fractional_numbers","A number had a period, followed by no numbers (#%d in the file, at position %d)",error_index+1,first_char_of_bad_number-js+1);
//                 case 3:
//                     mexErrMsgIdAndTxt("turtle_json:fractional_component_too_large","The fractional component of a number had too many digits");
//                 case 4:
//                     mexErrMsgIdAndTxt("turtle_json:no_exponent_numbers","A number had an exponent symbol (e or E) followed by no digits");
//                 case 5:
//                     mexErrMsgIdAndTxt("turtle_json:exponent_component_too_large","The fractional component of the number had too many digits");
//                 default:
//                     mexErrMsgIdAndTxt("turtle_json:internal_code_error","Internal code error"); 
    
    
    double value = 0;
    double fraction = 0;
    double exponent_value;
    bool negate;
    
    if (*p == '-'){
        ++p;
        negate = true;
    }else{
        negate = false;
    }
    
    //Verify at least one number - technically we could put this in the '-'
    //case above, as otherwise this was triggered based on observing a #
    if (isdigit(*p)){
        value = 10*value + (double)(*p-'0');
        ++p;
    }else{
    	*error_p = i+1;
        *error_value = 0;
        return;
    }
    
    while (isdigit(*p)){
        value = 10*value + (double)(*p-'0');
        ++p;
    }
    
    //Fraction
    //--------------------------------------------------------
    if (*p == '.') {
        ++p;
        if (isdigit(*p)){
            value = value + 0.1*(*p-'0');
            fraction = 0.01;
            ++p;
        }else{
            *error_p = i+1;
            *error_value = 2;
            return;
        }
        
      	while (isdigit(*p)){
            value = value + fraction * (*p-'0');
            fraction *= 0.1;
            ++p;
        }
    }
    
    if (negate){
        value = -1*value;
    }
    
    //Exponent
    //----------------------
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
        
        exponent_value = 0;
        while (isdigit(*p)) {
            exponent_value = 10*exponent_value + (double)(*p-'0');
            ++p;
        }
        if (negate){
            exponent_value = -exponent_value;
        }
        value *= pow(10.0, exponent_value);
    }
    
    //TODO: We could still have:
    //(so '.' , '-', 'e', and 'E') (numbers would just be parsed)
    //- note, with no e or E, we only need to check '.' and '-'
    

    *value_p = value;
    
}

//=========================================================================
//=========================================================================
void parse_numbers(unsigned char *js,mxArray *plhs[]) {
    //
    //  numeric_p - this array starts as a set of pointers
    //  to locations in the json_string that contain numbers.
    //  For example, we might have numeric_p[0] point to the following
    //  location:
    //
    //      {"my_value": 1.2345}
    //                   ^   
    //
    //  Some of these pointers may be null, indicating that a "null"
    //  JSON value occurred at that index in the array.
    //
    //  I am currently assuming that a pointer is 64 bits, which means
    //  that I recycle the memory to store the array of doubles
    
    mxArray *temp = mxGetField(plhs[0],0,"numeric_p");
    
    //Casting for input handling
    unsigned char **numeric_p = (unsigned char **)mxGetData(temp);
    
    //Casting for output handling (recycling of memory)
    double *numeric_p_double = (double *)mxGetData(temp);
    
    int n_numbers = mxGetN(temp);
    
    int *error_locations;
    int *error_values;
    
    int n_threads = omp_get_max_threads();
    
    error_locations = mxMalloc(n_threads*sizeof(int));
    error_values    = mxMalloc(n_threads*sizeof(int));
    
    const double MX_NAN = mxGetNaN();
    
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int error_location = 0;
        int error_value;

        #pragma omp for
        for (int i = 0; i < n_numbers; i++){
            if (numeric_p[i]){
                string_to_double_v2(&numeric_p_double[i],numeric_p[i],i,&error_location,&error_value);
            }else{
                numeric_p_double[i] = MX_NAN;
            }
        }  
        
        *(error_locations + tid) = error_location;
        *(error_values + tid) = error_value;
    }
    
    //Error processing
    //--------------------------------------
    for (int i = 0; i < n_threads; i++){
        if (*error_locations){
            int error_index = *error_locations - 1;
            //Note that we hold onto the pointer in cases of an error
            //It is not overidden with a double
            unsigned char *first_char_of_bad_number = numeric_p[error_index];
            // p - js
            //
            // numeric_p[**=
            
            //TODO: This is a bit confusing since this pointer doesn't
            //move but the other one does ...
            //TODO: Ideally we would pass these error messages into
            //a handler that would handle the last bit of formatting
            //and also provide context in the string
            //We would need the string length ...
            switch (*(error_values + i))
            {
                case 0:
                    //I don't think this can run based on how our parser works ...
                    //TODO: Change this to a code error?
                    mexErrMsgIdAndTxt("turtle_json:no_integer_component", \
                            "No integer component was found for a number (#%d in the file, at position %d)", \
                            error_index+1,first_char_of_bad_number-js+1);
                    break;
                case 1:
                    mexErrMsgIdAndTxt("turtle_json:integer_component_too_large",
                            "The integer component of the number had too many digits (#%d in the file, at position %d)",
                            error_index+1,first_char_of_bad_number-js+1);
                    break;
                case 2:
                    mexErrMsgIdAndTxt("turtle_json:no_fractional_numbers","A number had a period, followed by no numbers (#%d in the file, at position %d)",error_index+1,first_char_of_bad_number-js+1);
                case 3:
                    mexErrMsgIdAndTxt("turtle_json:fractional_component_too_large","The fractional component of a number had too many digits");
                case 4:
                    mexErrMsgIdAndTxt("turtle_json:no_exponent_numbers","A number had an exponent symbol (e or E) followed by no digits");
                case 5:
                    mexErrMsgIdAndTxt("turtle_json:exponent_component_too_large","The fractional component of the number had too many digits");
                default:
                    mexErrMsgIdAndTxt("turtle_json:internal_code_error","Internal code error");   
            }
        }
        ++error_locations;
    } 
}
//=====================   END OF NUMBER PARSING  ==========================