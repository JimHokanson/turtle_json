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

The following information is visible in **mex.object_info**

The following information is parsed in the initial pass through.

* child_count_object - int32[] (length = # of objects)
    *  \# of keys in the object
* next_sibling_index_object - int32[] (length = # of objects)
    * index into 'd1' of the token after the object closes
    * this can be used to navigate to the next token
    * out of all the 'next' pointers is probably the least used
* object_depths - uint8[]
    * depth at which the object occurs. The top most level is 1 (0 based indexing) (0 is not used)
    * TODO: I'm not sure why I hold onto this ...
* n_objects_at_depth - int32[]
    * The # of objects at each depth. 
    * TODO: I'm not sure why I hold onto this ...

**The following information is parsed in post-processing.**

* **max\_keys\_in\_object** - int32[]
    * The maximum # of keys present in any object
    * This is largely a temporary variable when needing to allocate key (field) names later on
* **unique\_object\_first\_md\_indices** - int32[] (length = # of unique objects)
    * The first object which has fieldnames of the given type
* **object\_ids** - int32[] (length = # of objects)
    * values specify which object the current object is like
    * e.g. object_ids[5] => 2, means that the fifth object (1 based) is the 3rd unique object type (0 based)
    * values are indices (0 based) into the 'objects' property
* **n\_unique\_objects** - int32
    * The # of unique objects, where being unique means a unique set of key names (not values)
    * In the current implementation, order matters, so changing the key order would create more unique objects
* **objects** - cell array of structures (length = # of unique objects)
    * Each structure is of size [1 0] but contains the parsed (i.e. non UTF-8) fieldnames in that object. This makes it easy to initialize an object as we simply copy this base object and resize.

### Arrays ###

### Keys ###

### Strings ###

### Numbers ###

### Logicals ###

### Timing Info ###

All reported timing values are in seconds.

* **elapsed\_read\_time** - how long it took to read the file from disk
* **parsed\_data\_logging\_time** - ????
* **elapsed\_parse\_time** - time to parse initial token locations
* **object\_parsing\_time** - time to determine unique objects
* **array\_parsing\_time** - time to identify array types (homogenous, non-homogenous, 1d arrays, nd arrays, etc.)
* **number\_parsing\_time** - time to convert strings into numbers
* **string\_memory\_allocation_time** - the amount of time required to allocate memory for all strings in the file
* **string\_parsing\_time** - time required to convert strings from bytes (UTF-8) into Matlab strings (UTF-16)
* **elapsed\_pp\_time** - total amount of time spent in post-processing (objects, arrays, numbers, strings)
* **total\_elapsed\_time_mex** - total amount of time spent in the mex code