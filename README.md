This code tokenizes JSON and can also be used to parse JSON.

# Status

With the exception of not yet escaping strings or handling unicode, the code works. Currently strings will be returned with the escape character
in place.

Writing to JSON is not yet supported.

I'll probably be working on documentation the most in the next few weeks (months) since the documentation is lacking.

# What's a token?

A token is a character that has special meaning. Tokens in JSON include (ntt exhaustive): '{' , '"', ':', '}', '[', and ']'

Consider the following JSON:

``` JSON
{"test":3,"key":[1,3,4]}
```

The '{' character specifies that an object starts. The '"' character specifies the start (or end) of a key/string unless it is escaped in a string. Once you know the location of these special characters, it becomes much easier to parse a JSON file.

# Usage

## Token Parsing

```matlab

%This returns a tokenized representation of all of the data
tokens = json.fileToTokens(file_path);

root = tokens.getRootInfo(); %Returns information on the root array or object

```

TODO: Jim at this point
- next, discuss parsing vs step by step object requesting
