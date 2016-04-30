%{
1) DONE Move all values to next valid values e.g. for an array, don't allow a ':'
2) DONE Make sure reallocations work
3) DONE Remove the structure
4) DONE Change parser variable names
5) Remove pointless value setting (i.e. initialization to -1)
6) DONE create key type
7) multiline macro refill check
8) TRIED - NO GO remove extra posiition variable in # parsing
9) Finish test suite
10) Improve error printing
11) make pointers to numeric and strings
12) parent array with max depth

%Ugly hack to speed things up:
%- DONE create separate pathways for objects and array processing








super_tokens
1) start
2) end
3) next
4) parent

type
index

strings:
--------
value

numerics:
---------
value

%}