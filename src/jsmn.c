#include "jsmn.h"
#include "stdio.h"
#include <stdlib.h>
#include "mex.h"
#include <math.h>

//From:
//------
//http://tinodidriksen.com/uploads/code/cpp/speed-string-to-double.cpp
//https://tinodidriksen.com/2011/05/28/cpp-convert-string-to-double-speed/

const bool is_number_array[256] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,true,true,true,true,true,true,true,true,true,true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};
const double p1e0[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9};
const double p1e1[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,20,30,40,50,60,70,80,90};
const double p1e2[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,200,300,400,500,600,700,800,900};
const double p1e3[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000,2000,3000,4000,5000,6000,7000,8000,9000};
const double p1e4[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10000,20000,30000,40000,50000,60000,70000,80000,90000};
const double p1e5[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100000,200000,300000,400000,500000,600000,700000,800000,900000};
const double p1e_1[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
const double p1e_2[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09};
const double p1e_3[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000,0.001,0.002,0.003,0.004,0.005,0.006,0.007,0.008,0.009};
const double p1e_4[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000,0.0001,0.0002,0.0003,0.0004,0.0005,0.0006,0.0007,0.0008,0.0009};
const double p1e_5[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000,0.00001,0.00002,0.00003,0.00004,0.00005,0.00006,0.00007,0.00008,0.00009};
const double p1e_6[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000,0.000001,0.000002,0.000003,0.000004,0.000005,0.000006,0.000007,0.000008,0.000009};

//TODO: Pass in pointer to double
//Return error value ...
double string_to_double(const char *p,char **char_offset) {

    
    double r;
    
    bool neg = false;
    double f = 0.0;
    int n = 0;
    char *start;
    //double integer_scalars = [1 10 100 1000 10000];
    
    if (*p == '-') {
        neg = true;
        ++p;
    }else if (*p == '+') {
        ++p;
    }
    
    start = p;
    
    while (is_number_array[*p++]) {  
    }
    --p;
    
    //? Subtraction vs lookup
    //
    // TODO: Just do fall through for now
    
    r = 0;
    n = p-start;
    p = start;
    switch (n) {
        case 6:
           r += p1e5[*p++];
        case 5:
           r += p1e4[*p++];
        case 4:
           r += p1e3[*p++];
        case 3:
           r += p1e2[*p++];
        case 2:
           r += p1e1[*p++];
        case 1:
           r += p1e0[*p++];
           break;
        default:
            mexPrintf("Too many #s: %d\n",n);
        mexErrMsgIdAndTxt("wtf:wtf2","Too many #s");
    }  
//     switch (p-start) {
//         case 1:
//                r = p1e0[*(p-1)]; 
//             break;
//         case 2:
//             r = p1e0[*(p-1)]+
//                 p1e1[*(p-2)]; 
//             break;
//         case 3:
//             r = p1e0[*(p-1)]+
//                 p1e1[*(p-2)]+
//                 p1e2[*(p-3)];
//             break;
//         case 4:
//                r = p1e0[*(p-1)]+
//                    p1e1[*(p-2)]+
//                    p1e2[*(p-3)]+
//                    p1e3[*(p-4)];
//             break;
//         case 5:
//                 r = (*(p-1) - '0') + 
//                         (*(p-2) - '0')*10 + 
//                         (*(p-3) - '0')*100 + 
//                         (*(p-4) - '0')*1000 + 
//                         (*(p-5) - '0')*10000;
//             break;
//         case 6:
//                 r = (*(p-1) - '0') + 
//                         (*(p-2) - '0')*10 + 
//                         (*(p-3) - '0')*100 + 
//                         (*(p-4) - '0')*1000 + 
//                         (*(p-5) - '0')*10000 + 
//                         (*(p-6) - '0')*100000;    
//         default:
//             mexErrMsgIdAndTxt("wtf:wtf2","Too many #s");
//     }    
    
    if (*p == '.') {
        ++p;
        start = p;
        
        while (is_number_array[*p++]) { 
        }
         --p;
                
        
        switch (p-start) {
            case 1:
                   r += p1e_1[*(p-1)];
                break;
            case 2:
                   r += p1e_2[*(p-1)] +
                        p1e_1[*(p-2)];
                break;
            case 3:
                r +=    p1e_3[*(p-1)] + 
                        p1e_2[*(p-2)] + 
                        p1e_1[*(p-3)];
                break;
            case 4:
                r +=    p1e_4[*(p-1)] + 
                        p1e_3[*(p-2)] + 
                        p1e_2[*(p-3)] +
                        p1e_1[*(p-4)] ;
                break;
            case 5:
                    r += (*(p-1) - '0')*0.00001 + 
                            (*(p-2) - '0')*0.0001 + 
                            (*(p-3) - '0')*0.001 + 
                            (*(p-4) - '0')*0.01 + 
                            (*(p-5) - '0')*0.1;
                break;
                
            case 6:
                    r +=    (*(p-1) - '0')*0.000001 + 
                            (*(p-2) - '0')*0.00001 + 
                            (*(p-3) - '0')*0.0001 + 
                            (*(p-4) - '0')*0.001 + 
                            (*(p-5) - '0')*0.01 + 
                            (*(p-6) - '0')*0.1;
                break;                
            case 7:
                    r +=    (*(p-1) - '0')*0.0000001 + 
                            (*(p-2) - '0')*0.000001 + 
                            (*(p-3) - '0')*0.00001 + 
                            (*(p-4) - '0')*0.0001 + 
                            (*(p-5) - '0')*0.001 +
                            (*(p-6) - '0')*0.01 +
                            (*(p-7) - '0')*0.1;
               case 8:
                    r +=    (*(p-1) - '0')*0.00000001 + 
                            (*(p-2) - '0')*0.0000001 + 
                            (*(p-3) - '0')*0.000001 + 
                            (*(p-4) - '0')*0.00001 + 
                            (*(p-5) - '0')*0.0001 +
                            (*(p-6) - '0')*0.001 +
                            (*(p-7) - '0')*0.01 +
                            (*(p-8) - '0')*0.1;
                case 9:
                    r +=    (*(p-1) - '0')*0.000000001 + 
                            (*(p-2) - '0')*0.00000001 + 
                            (*(p-3) - '0')*0.0000001 + 
                            (*(p-4) - '0')*0.000001 + 
                            (*(p-5) - '0')*0.00001 +
                            (*(p-6) - '0')*0.0001 +
                            (*(p-7) - '0')*0.001 +
                            (*(p-8) - '0')*0.01 +
                            (*(p-9) - '0')*0.1;
                break;                 
                
            default:
                mexPrintf("%d\n",p-start);
                mexErrMsgIdAndTxt("wtf:wtf3","Too many #s fractions");
        }

    }
    if (neg) {
        r = -r;
    }
    
    //This code hasn't been touched
    if (*p == 'E' || *p == 'e') {
        ++p;
        if (*p == '-'){
            neg = true;
            ++p;
        }else if (*p == '+') {
            neg = false;
            ++p;
        }
        f = 0.0;
        while (is_number_array[*p]) {
            f = (f*10.0) + (*p - '0');
            ++p;
        }
        if (neg){
          f = -f;  
        }
        r *= pow(10.0, f);
    }
    
    *char_offset = p;
    

    return r;
}
    
// // // //     double r = 0.0;
// // // //     bool neg = false;
// // // //     double f = 0.0;
// // // //     int n = 0;
// // // //     
// // // //     if (*p == '-') {
// // // //         neg = true;
// // // //         ++p;
// // // //     }else if (*p == '+') {
// // // //         neg = false;
// // // //         ++p;
// // // //     }
// // // //     
// // // //     while (*p >= '0' && *p <= '9') {
// // // //         r = (r*10.0) + (*p - '0');
// // // //         ++p;
// // // //     }
// // // //     if (*p == '.') {
// // // //         ++p;
// // // //         while (*p >= '0' && *p <= '9') {
// // // //             f = (f*10.0) + (*p - '0');
// // // //             ++p;
// // // //             --n;
// // // //         }
// // // //         r += f * pow(10.0, n);
// // // //     }
// // // //     if (neg) {
// // // //         r = -r;
// // // //     }
// // // //     
// // // //     if (*p == 'E' || *p == 'e') {
// // // //         ++p;
// // // //         if (*p == '-'){
// // // //             neg = true;
// // // //             ++p;
// // // //         }else if (*p == '+') {
// // // //             neg = false;
// // // //             ++p;
// // // //         }
// // // //         f = 0.0;
// // // //         while (*p >= '0' && *p <= '9') {
// // // //             f = (f*10.0) + (*p - '0');
// // // //             ++p;
// // // //         }
// // // //         if (neg){
// // // //           f = -f;  
// // // //         }
// // // //         r *= pow(10.0, f);
// // // //         //mexErrMsgTxt("Error, unhandled E in code");
// // // //     }
// // // //     
// // // //     *char_offset = p;
// // // //     
// // // // 
// // // //     return r;
// // // // }


//Parser initialization
//----------------------------------
void jsmn_init(jsmn_parser *parser) {
	parser->pos = 0;
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
    
    //
    //  Inputs
    //  ------
    //  parser : 
    //      Initialized parser from jsmn_init()
    //  js : string
    //      The JSON string to parse
    
    const double MX_NAN = mxGetNaN();
    
    //Parser local variables
    //--------------------------------------
    unsigned int parser_position;
    unsigned int next_token_index;
    int super_token_index;
    
    //Temp local variables
    //----------------------------------------------------
    bool string_parsed; // 
    
    
    
    bool super_token_is_string; //Happens for attributes
	int r;
    int start;
	int i;
    
	char c;
    char *pEndNumber;
    //Neither of these worked for me with MSVC++ 2012
    //double NAN = 0.0/0.0;
	//double NAN = strtod("NaN", NULL);
    
    jsmntype_t type;
    jsmntok_t null_super[1];
    jsmntok_t *token;
    jsmntok_t *super_token;
    
    //parser back to local variables
    parser_position = parser->pos;
    next_token_index = parser->toknext;
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
    
    //We shouldn't ever allocate to this in the loop
    //otherwise we have a parsing error
    //TODO: Build in checks in the loop that are better than those I took 
    //out
    null_super->size = 0;
    null_super->type = 0;
    null_super->parent = -1;

    //-------------  THE MAIN LOOP   --------------------------------------    
    parser_position--;
    while((c = js[++parser_position])){

		switch (c) {
			case '{':                
            case '[':
                if (next_token_index >= num_tokens) {
                    refill_parser(parser,parser_position,next_token_index,super_token_index);
                    return JSMN_ERROR_NOMEM;
                }
                super_token->size++;  
                
                //Initialization
                //-----------------------------
                token = &tokens[next_token_index++];
                
                //These are optional ...
                token->end    = -1;
                token->size   = 0;
#ifdef JSMN_TOKEN_AFTER_CLOSE  
                token->token_after_close = -1;
#endif
                //This should really be set
                //token->value = 0;
                *values++ = 0;
                
                //Population of final attributes
                //------------------------------
                token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
				token->start = parser_position;
                token->parent = super_token_index;

                //Now make this the super token
                super_token = token;
                super_token_is_string = false;
				super_token_index = next_token_index - 1;
				break;
                //---------------- End of Open Array ----------------------
			case '}': 
                //---------------- Start of Close Object ------------------
                //
                //{ "test" : 1  }
                //1  2       3  4  
                //     p        x   <= p is parent/super_token
                        
                // If the super token is a string (i.e. attribute)
                // This should happen the most often
                if (super_token_is_string){
                   //Specify next token for the string
                   super_token->token_after_close = next_token_index;
                   
                   //Now move up to "{" character
                   super_token_index = super_token->parent;
                   super_token = &tokens[super_token_index];
                }
                
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
                    //TODO: Verify that not updating the toksuper is ok
                    super_token=null_super;
                    super_token_is_string = false;
                }else{
                    super_token_index = super_token->parent;
                    super_token = &tokens[super_token_index];
                    super_token_is_string = super_token->type == JSMN_STRING;
                }                   
                break;
                //--------       End of Close Object       ---------------
            case ']':                
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
                
                //TODO: If this is ever -1, then we should break to the end
                //and make sure nothing follows except whitepsace
                if(super_token->parent == -1){
                    super_token=null_super;
                    super_token_is_string = false;
                }else{
                    super_token_index = super_token->parent;
                    super_token = &tokens[super_token_index];
                    super_token_is_string = super_token->type == JSMN_STRING;
                }
				break;
                //-----------   End of Close Array     --------------------
			case '\"':
                //-----------  Start parsing of String --------------------
                if (next_token_index >= num_tokens) {
                    return JSMN_ERROR_NOMEM;
                }
                
                start = parser_position;
                
                token = &tokens[next_token_index++];
        	    token->type   = JSMN_STRING;
         	    //TODO: For Matlab we could add 2 instead of 1
           	    token->start  = start+1;
                //token->end  <= defined in loop
                //token->size <= not currently defined ...
         	    token->parent = super_token_index;
#ifdef JSMN_TOKEN_AFTER_CLOSE                        
                token->token_after_close = next_token_index;
#endif
                *values++ = 0;
                
                //We need this in case parsing fails ...
                
                
                //Move past the starting quote
                parser_position++;
                
                //Looping over the string ...
                //---------------------------------------------------------
                string_parsed = false;
                for (; parser_position < len; parser_position++) {
                    c = js[parser_position];

                    /* Quote: end of string */
                    if (c == '\"') {
                        //Again, this is Matlab based, add flag and
                        //handle accordingly ...
                        //We're pointing to the " in c
                        //which in Matlab points to the character before
                        //c
                        token->end    = parser_position;
                        string_parsed = true;
                        break;
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
                                        parser_position = start;
                                        
                                        mexPrintf("Invalid Char: %c\n",js[parser_position]);
                                        mexPrintf("Location: %d\n",parser_position);
                                        
                                        return JSMN_ERROR_INVAL;
                                    }
                                    parser_position++;
                                }
                                parser_position--;
                                break;
                            /* Unexpected symbol */
                            default:
                                mexPrintf("Invalid Char: %c\n",js[parser_position]);
                                mexPrintf("Location: %d\n",parser_position);
                                parser_position = start;
                                return JSMN_ERROR_INVAL;
                        }
                    }
                }
                
                if(!string_parsed){
                    //We got to the end of the input string without a '"'
                    parser_position = start;
                    return JSMN_ERROR_PART;
                }
                //mexPrintf("token->end : %d\n",token->end);
                super_token->size++; 
				break;
            //--------      Start of Parsing Whitespace    ---------------
            case ' '  :
            case '\n' :
			case '\t' : 
            case '\r' :
				break;
            //--------      End of Parsing Whitespace    ---------------
			case ':':
                //Here we are moving from a key to the attribute
                //We make the attribute the parent for future things
                super_token = &tokens[next_token_index - 1];
                super_token_is_string = true;
				super_token_index = next_token_index - 1;
				break;
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
                    super_token_index = super_token->parent;
                    super_token = &tokens[super_token_index];
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
            	if (next_token_index >= num_tokens) {
                    return JSMN_ERROR_NOMEM;
                }
                super_token->size++;
                
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
#ifdef JSMN_TOKEN_AFTER_CLOSE                        
                token->token_after_close = next_token_index;
#endif
                parser_position--; //backtrack so we terminate on the #
                
				break;
                
            //-------        End of Number Parsing       --------------
            case 'n' :
                super_token->size++;
                
                token = &tokens[next_token_index++];
                token->type   = JSMN_NUMBER;
                token->start  = parser_position;
                //TODO: Error check null - make optional with compile flag
                parser_position  += 3; //advance to final 'l' in 'null'
                //1 based indexing ...
                token->end    = parser_position+1;
                token->size   = 1;
                token->parent = super_token_index;
                #ifdef JSMN_TOKEN_AFTER_CLOSE                        
                    token->token_after_close = next_token_index;
                #endif
                *values++ = MX_NAN;
                mexPrintf("Values: %f",*values);
                break;
            //-------        End of Null Parsing       --------------
			case 't':
                //------------  Start of True Token  --------------
                if (next_token_index >= num_tokens) {
                    return JSMN_ERROR_NOMEM;
                }
                super_token->size++;
                
                token = &tokens[next_token_index++];
                token->type   = JSMN_LOGICAL;
                token->start  = parser_position;
                //TODO: Error check true - make optional with compile flag
                parser_position  += 3; //advance to final 'e' in 'true'
                //1 based indexing ...
                token->end    = parser_position+1;
                token->size   = 1;
                token->parent = super_token_index;
                #ifdef JSMN_TOKEN_AFTER_CLOSE                        
                    token->token_after_close = next_token_index;
                #endif
                *values++ = 1;
                break;
            case 'f':
                //------------  Start of False Token  --------------
                if (next_token_index >= num_tokens) {
                    return JSMN_ERROR_NOMEM;
                }
                super_token->size++;
                
                token = &tokens[next_token_index++];
                token->type   = JSMN_LOGICAL;
                token->start  = parser_position;
                //TODO: Error check true - make optional with compile flag
                parser_position  += 4; //advance to final 'e' in 'false'
                //1 based indexing ...
                token->end    = parser_position+1;
                token->size   = 1;
                token->parent = super_token_index;
                #ifdef JSMN_TOKEN_AFTER_CLOSE                        
                    token->token_after_close = next_token_index;
                #endif
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
    
    //0 based indexing, next token 1 means a count of 1 token
	return next_token_index;
    
}

