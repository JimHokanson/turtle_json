This code parses JSON files/strings using C (mex) code. Writing is not yet supported.

# Why This Code?

I needed JSON parsing for a project. Loading JSON files using existing parsers was painfully slow. This slowness was due to 1) JSON parsing being done in Matlab or 2) parsing using C/C++ or Java, but with inefficient memory structures. This code started off by wrapping an already written C JSON tokenizer, and then post-processing in Matlab. After some nit-picking here and there, I found myself writing an entire parser in C, from scratch.

# Advantages and Disadvantages

## Advantages
* C parser written specifically for Matlab, rather than wrapping an existing parser
* multi-step parsing option (tokens, then data) for finer (complete?) control over output format
* just like every other "fast" JSON parser, this one is fast as well

## Disadvantages
* I used a non-native compiler for both Windows and Mac (GCC)
* Currently favors speed over memory (this can be improved)
* Currently requires newer computers due to use of SIMD (I believe I'm only using AVX so ~2011 or newer processor required). Ideally a switch would allow an option not to use SIMD.

# Status

* Parser design is stable.
* Needs more unit tests (in progress).
* Lots of small issues, although nothing critical.

# Usage

Parsing can be done in one of two ways. Parsing can either be done to a set of tokens or alternatively, to a complete representation of the structure in Matlab. Parsing to tokens provides finer control over the parsing process.

## Parsing to a Complete Matlab Data Structure

The simplest approach is to parse directly to a Matlab data structure.

```matlab
data = json.load(file_path);
%OR
data = json.parse(json_string);
```

## Parsing to Tokens

For those that want a bit more control over the parsing process, one can parse to tokens, and then to data. Tokens include objects, arrays, numbers, etc. in the file. 

The following is an example.

```matlab
%This returns a tokenized representation of all of the data
root = json.tokens.load(file_path);
%OR
root = json.tokens.parse(json_string);

%Let's assume we got an object root, let's get the 'x_data' property.
x_data_token = root.getToken('x_data');

%Let's assume 'x_data' is an array, then 'x_data_token' contains information
%about that array, but it does not contain the actual data.

%Assuming 'x_data' should contain a cell array of 1d arrays
%e.g. x_data = {[1,2,3],[4],[5,6,7,8,9]}
x_data = x_data_token.getArrayOf1dNumericArrays();

%If 'x_data' is a 2d array (matrix)
x_data = x_data_token.get2dNumericArray();

%If 'x_data' is a 1d array
x_data = x_data_token.get1dNumericArray();

%If 'x_data' contains a cell array of strings
x_data = x_data_token.getCellstr();
```

# Contributing

Feel free to send me an email if you have an idea or want to discuss contributing. Please also open issues for questions, comments, or suggestions.



