This code parses JSON files/strings. Writing is not yet supported.

#Why This Code?

This parser was written to have high performance relative to other Matlab JSON parsers. 


# Usage

Parsing can be done in one of two ways. Parsing can either be done to a set of tokens or alternatively, to a complete representation of the structure in Matlab. Parsing to tokens provides finer control over the parsing process.

## Parsing to Tokens

TODO: Not thrilled with the token name, abstract syntax tree?

```matlab
%This returns a tokenized representation of all of the data
tokens = json.fileToTokens(file_path);
%OR
tokens = json.stringToTokens(json_string);

%Returns information on the root array or object
root = tokens.getRootInfo(); 

%Let's assume we got an object root, let's get the 'x_data' property.
x_data_token = root.getToken('x_data');

%Let's assume 'x_data' is an array, then 'x_data_token' contains information
%about that array, but it does not contain a Matlab representation of the data

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

## Parsing to a Complete Matlab Data Structure

```matlab
data = json.load(file_path);
%OR
data = json.parse(json_string);
```


# What's a token?

A token is a character that has special meaning. Tokens in JSON include (not exhaustive): '{' , '"', ':', '}', '[', and ']'

Consider the following JSON:

``` JSON
{"test":3,"key":[1,3,4]}
```

The '{' character specifies that an object starts. The '"' character specifies the start (or end) of a key/string unless it is escaped in a string. Once you know the location of these special characters, it becomes much easier to parse a JSON file.

