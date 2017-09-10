function data = load(file_path,token_options,data_options)
%x Load json data from disk ...
%
%   s = json.load(file_path,token_options,data_options);
%
%   Inputs
%   ------
%   file_path
%   token_options : cell
%   convert_options : cell
%
%   
%
%   Token Options
%   --------------
%   
%
%   Data Options
%   -------------
%   max_numeric_collapse_depth: default -1
%   max_string_collape_depth : default -1
%   max_bool_collapse_depth : default -1
%   column_major : default true
%       If true, nd-arrays are read in column-major order,
%       otherwise as row major order. Note that Matlab uses column-major
%       ordering which means that parsing of the data is slightly more
%       efficient when column-major is used.
%
%       For example, consider the following 
%       JSON: [[1,2,3],[4,5,6]]
%
%       column-major: [1, 4; 
%                      2, 5; 
%                      3, 6]
%       row-major: [1,2,3; 
%                   4,5,6]
%
%   collapse_objects : default true
%       If true, objects with the same properties (in the 
%       same order) will be collapsed into a structure
%       array, otherwise all object arrays will be returned
%       as cell arrays of structures
%   For example, consider the following 
%   JSON: {'a':1,'b':2} TODO: Finish
%
%
%   Examples
%   --------
%   data = json.load(file_path,{},{'column_major',false});
%
%   See Also
%   --------
%   json.parse

%Retrieve tokens
root = json.tokens.load(file_path,token_options{:});

%root is either:
%   json.objs.token.array
%   json.objs.token.object
%
%which are both a subclass of:
%   json.objs.token

%Parse the resulting data
data = root.getParsedData(data_options{:});


end