This code tokenizes JSON and can also be used to parse JSON.

# Status

With the exception of not yet escaping strings or handling unicode, the code works. Currently strings will be returned with the escape character
in place.

In addition there are still some optimizations I would like to make and the interface may change.

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

TODO ...

# Why another JSON parser?

## The Problem
One of the biggest differentiators between different parsers is their interface. To this end a lot of time can be wasted by trying to convert between a JSON structure that is parsed in another language (e.g. Java, c++) and Matlab. Probably the format with the biggest overhead is a JSON array. Consider the following:

``` JSON
[1,2,4,5,6]
```

In Matlab this may be equivalent to:

``` matlab
1:6
```

However, a JSON array is really functionally equivalent to a cell array in Matlab. For example, the following is a valid JSON array:

```
[1,'test',[1,2,3],4]
```

Thus a generic JSON parser would need to parse the above as:

``` matlab
{1,'test',{1,2,3},4}
```

Typically arrays are not of mixed types, so the problem really comes in with higher order arrays. Consider the following datasets in JSON:

``` JSON
[[1,2][3,4]]
```

``` JSON
[[1,2,3],[4,5]]
``

In the first example, the data can be accurately represented as a matrix in Matlab:

``` matlab
[1,2; 3,4]
```

However, the 2nd example can't since the inner arrays are of a different size. Instead a cell array is needed:
``` matlab
{[1 2 3],[4 5]}
```

The problem comes when 99% of the time your data looks like the 2nd case, so a cell array is returned, but every once in a while the stars align and you get equal sized arrays and now a fancy parser returns a matrix instead of a cell array of arrays. Downstream when you attempt to dereference the first cell (i.e. data{1}) you get an error because this time your data is actually a matrix. This problem is solveable with user input but makes it very hard to create a generic parser that can simplify (saving time) only when actually appropriate. Additionally, it is really slow to initially create a cell array of individual numbers, only to later convert it to a numerical array.


## A Matlab Based Solution

Surprisingly, there were actually many Matlab based solutions when I started. Most of these suffered from bridging Matlab with another language as mentioned above. The most well known of these was jsonlab (http://www.mathworks.com/matlabcentral/fileexchange/33381-jsonlab--a-toolbox-to-encode-decode-json-files). When testing jsonlab on my initial 75 MB file the parsing took 30s. Upgrading to 2015b reduced this time to 18s (http://blogs.mathworks.com/loren/2016/02/12/run-code-faster-with-the-new-matlab-execution-engine/). For reference someone else working with this file was supposedly parsing this file in ~0.5s using Scala. To make matters worse, at the time there was a known silent bug in the jsonlab code that returned the wrong result without any error (https://github.com/fangq/jsonlab/issues/5). I VERY MUCH DISLIKE SILENT BUGS.

What I really wanted was a tokenizer so that I could parse the code in Matlab, knowing ahead of time where all of the objects, arrays, strings, and numbers began in the file.  To this end I found JSMN, a JSON tokenizer written in C. The code was surprisingly simple although I saw (to my surprise) numerous places for improvement as well as numerous bugs.

The resulting code, initially based on JSMN, ended up being written as a state-machine. The state machine operates by writing specific code depending on what state the tokenizer is in. If the tokenizer is at an opening object token '{', then it needs to proceed past whitespace to a key '"' or a closing object token '}', anything else is an error.

The parsing is currently done in Matlab (post-tokenizing) although I'm considering writing a generic parser in C (that uses the tokens I've created).

# Speed

TODO ...