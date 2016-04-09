double string_to_double(const char *js, int *input_parser_position) {

    /*
     *  I found atof to be the main bottleneck in this code. It was 
     *  suggested to me by some internet searching that a custom algorithm
     *  would be close enough in 99.99 of the cases. I found a couple
     *  basic ones online that used loops and multiplation. This was my
     *  attempt to make something as fast possible. I welcome a faster
     *  approach!
     */
    int parser_position = *input_parser_position;
    double value = 0;
    double exponent_value;
    bool negate;
    int n_numeric_chars_plus_1;
    int number_start;
    
    if (js[parser_position] == '-'){
        ++parser_position;
        negate = true;    
    }else{
        negate = false;
    }

    number_start = parser_position;
    while (is_number_array[js[parser_position++]]) {  
    }
    
    n_numeric_chars_plus_1 = parser_position-number_start; //# of numeric characters (off by 1 due to ++)
    
    //reset p so that we can increment our way along the number
    parser_position = number_start;
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
           value += p1e15[js[parser_position++]];  
        case 16:
           value += p1e14[js[parser_position++]];  
        case 15:
           value += p1e13[js[parser_position++]];   
        case 14:
           value += p1e12[js[parser_position++]];   
        case 13:
           value += p1e11[js[parser_position++]];   
        case 12:
           value += p1e10[js[parser_position++]];       
        case 11:
           value += p1e9[js[parser_position++]];  
        case 10:
           value += p1e8[js[parser_position++]];  
        case 9:
           value += p1e7[js[parser_position++]];  
        case 8:
           value += p1e6[js[parser_position++]]; 
        case 7:
           value += p1e5[js[parser_position++]];
        case 6:
           value += p1e4[js[parser_position++]];
        case 5:
           value += p1e3[js[parser_position++]];
        case 4:
           value += p1e2[js[parser_position++]];
        case 3:
           value += p1e1[js[parser_position++]]; //1e1 == 10, 2 #s plus the off by 1
        case 2:
           value += p1e0[js[parser_position++]]; //1e0 == 1, an unfortunate mismatch of exponent and scalar
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
    
    if (js[parser_position] == '.') {
        ++parser_position;   
        //TODO: Is no digit ok?
        if(is_number_array[js[parser_position]]){
            value += p1e_1[js[parser_position++]];
            if(is_number_array[js[parser_position]]){
                value += p1e_2[js[parser_position++]];
                if(is_number_array[js[parser_position]]){
                    value += p1e_3[js[parser_position++]];
                    if(is_number_array[js[parser_position]]){
                        value += p1e_4[js[parser_position++]];
                        if(is_number_array[js[parser_position]]){
                            value += p1e_5[js[parser_position++]];
                            if(is_number_array[js[parser_position]]){
                                value += p1e_6[js[parser_position++]];
                                if(is_number_array[js[parser_position]]){
                                    value += p1e_7[js[parser_position++]];
                                    if(is_number_array[js[parser_position]]){
                                        value += p1e_8[js[parser_position++]];
                                        if(is_number_array[js[parser_position]]){
                                            value += p1e_9[js[parser_position++]];
                                            if(is_number_array[js[parser_position]]){
                                                value += p1e_10[js[parser_position++]];
                                                if(is_number_array[js[parser_position]]){
                                                    value += p1e_11[js[parser_position++]];
                                                    if(is_number_array[js[parser_position]]){
                                                        value += p1e_12[js[parser_position++]];
                                                        if(is_number_array[js[parser_position]]){
                                                            value += p1e_13[js[parser_position++]];
                                                            if(is_number_array[js[parser_position]]){
                                                                value += p1e_14[js[parser_position++]];
                                                                if(is_number_array[js[parser_position]]){
                                                                    value += p1e_15[js[parser_position++]];
                                                                    if(is_number_array[js[parser_position]]){
                                                                        value += p1e_16[js[parser_position++]];
                                                                        if(is_number_array[js[parser_position]]){
                                                                            value += p1e_17[js[parser_position++]];
                                                                            if(is_number_array[js[parser_position]]){
                                                                                value += p1e_18[js[parser_position++]];
                                                                                if(is_number_array[js[parser_position]]){
                                                                                    value += p1e_19[js[parser_position++]];
                                                                                    if(is_number_array[js[parser_position]]){
                                                                                        value += p1e_20[js[parser_position++]];
                                                                                        if(is_number_array[js[parser_position]]){
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
    
    if (js[parser_position] == 'E' || js[parser_position] == 'e') {
        ++parser_position;
        switch (js[parser_position]){
            case '-':
                ++parser_position;
                negate = true;
                break;
            case '+':
                ++parser_position;
            default:
                negate = false;
        }
        
        number_start = parser_position;
        while (is_number_array[js[parser_position++]]) {  
        }

        exponent_value = 0;
        n_numeric_chars_plus_1 = parser_position-number_start;
        //reset p so that we can increment our way down
        parser_position = number_start;
        switch (n_numeric_chars_plus_1) {
            case 17:
               exponent_value += p1e15[js[parser_position++]];  
            case 16:
               exponent_value += p1e14[js[parser_position++]];  
            case 15:
               exponent_value += p1e13[js[parser_position++]];   
            case 14:
               exponent_value += p1e12[js[parser_position++]];   
            case 13:
               exponent_value += p1e11[js[parser_position++]];   
            case 12:
               exponent_value += p1e10[js[parser_position++]];       
            case 11:
               exponent_value += p1e9[js[parser_position++]];  
            case 10:
               exponent_value += p1e8[js[parser_position++]];  
            case 9:
               exponent_value += p1e7[js[parser_position++]];  
            case 8:
               exponent_value += p1e6[js[parser_position++]]; 
            case 7:
               exponent_value += p1e5[js[parser_position++]];
            case 6:
               exponent_value += p1e4[js[parser_position++]];
            case 5:
               exponent_value += p1e3[js[parser_position++]];
            case 4:
               exponent_value += p1e2[js[parser_position++]];
            case 3:
               exponent_value += p1e1[js[parser_position++]];
            case 2:
               exponent_value += p1e0[js[parser_position++]];
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
    
    //*char_offset = parser_position;
    
    *input_parser_position = parser_position;
    
    return value;
}