# Data Model

Following is a description of the internal data model used during parsing.

Tokens include:
* Object  {
* Array   [
* Key     :
* String  "
* Number  1
* Null    null  - I represent null as NaN
* True    true
* False   false

## Things that exist for every token ##
There are two arrays that have elements for each token. The order of the entries is in the order in which they are encountered. Thus the first value corresponds to the first token. This order is often used to navigate between elements, particularly for arrays.

* types: [uint8]
..* types include object, array, key, string, etc. (see turtle_json.h)
* d1: [int32]
..* indices (0 based) of data indices (see next section)
..* in code, I've tried to refer to this as the **md_index (main data index)**
..* Since the indices are incremental, this can serve as an indicator of the # of values between two different locations, e.g. if the 100th index has a value of 30 and 105th index has a value of 35 (and is of the same type -> e.g. types[100] == types[105]), then we know that types[101:104] == types[100] as well)

## Data Type Specific Information ##

### Objects ###

The following information is visible in ** mex.object_info **

The following information is parsed in the initial pass through.

*child_count_object - int32[] (length = # of objects)
..* # of keys in the object
*next_sibling_index_object - int32[] (length = # of objects)
..* index into 'd1' of the token after the object closes
..* this can be used to navigate to the next token
..* out of all the 'next' pointers is probably the least used
*object_depths - uint8[]
..* depth at which the object occurs. The top most level is 1 (0 based indexing) (0 is not used)
..* TODO: I'm not sure why I hold onto this ...
*n_objects_at_depth - int32[]
..* The # of objects at each depth. 
..* TODO: I'm not sure why I hold onto this ...

The following information is parsed in post-processing

*max_keys_in_object - int32[]
..* The maximum # of keys present in any object
..* This is largely a temporary variable when needing to allocate key (field) names later on
*unique_object_first_md_indices - int32[] (length = # of unique objects)
*object_ids - int32[] (length = # of objects)
..* values specify which object the current object is like
..* e.g. object_ids[5] => 2, means that the fifth object (1 based) is the 3rd unique object type (0 based)
..* values are indices (0 based) into the 'objects' property
*n_unique_objects - int32
..* The # of unique objects, where being unique means a unique set of key names (not values)
..* In the current implementation, order matters, so changing the key order would create more unique objects
*objects - cell array of structures (length = # of unique objects)
..* Each structure is of size [1 0] but contains the parsed (i.e. non UTF-8) fieldnames in that object

### Arrays ###

### Keys ###

### Strings ###

### Numbers ###

### Logicals ###


Below is old and will be changed ...


Data Type Specific
------------------

    --------  Key Related -------
            keys : cellstr
                Parsed key strings
            key_p: array of unsigned char *
                Pointer to the first character in the key string (not the 
                opening double quotes)

        TODO: These might change since the processing approach changed
        ---------------------------
        A better approach now might be to keep track of the # of characters
        in the entry, since key_p is a sufficient pointer

key_start_indices: array of int
            Count of the # of key characters at the start of processing
            this key. This is used for placing all keys into a singular
            array.
  key_end_indices: array of int
            Count of the # of key characters (really bytes) after initial
            processing of the key. This gets updated in post-processing
            based on UTF8 processing (conversion to UTF16).
    
    -------- String Related -------
        strings: cellstr
            Parsed string values    
       string_p: array of unsigned char *
            Pointer to the first character in the key string (not the 
            opening double quotes)

TODO: See note in key section regarding these values. Since d2 is unused
it could possibly store the length.
string_end_indices:
string_start_indices:

    -------  Number Related  ------
      numeric_p: 1) Initially a pointer to the start of a number to process
                 2) Later this takes on the 

General Meta Data
----------------------
          n_key_chars: total # of bytes in the JSON string that are part 
            of keys (not including double quotes)
       n_string_chars: same as 'n_key_chars' but for string values
    n_key_allocations:
 n_string_allocations:
n_numeric_allocations:

