function data = load(file_path,varargin)
%x Load json data from disk ...
%
%   s = json.load(file_path,*data_options,*token_options);
%
%   In this case 
%   s = json.load(file_path,varargin)
%
%   Inputs
%   ------
%   file_path
%   token_options : cell or empty
%   data_options : cell or empty
%
%
%
%   Token Options
%   --------------
%   For the most part these are only necessary to try and speed up parsing
%   or to limit memory usage.
%
%   n_tokens : double (default )
%       # of tokens expected in the file. Default is ...
%   n_keys : double (default )
%       # of keys expected in the file. 
%   n_strings : double
%       # of strings expected in the file.
%   n_numbers : double
%       # of numbers expected in the file.
%   chars_per_token - NYI
%       # of tokens to initially allocate based on the
%       length of the json string.
%
%   Data Options
%   -----------------------------------------------------------------------
%   These options affect how the JSON representation is converted to a
%   Matlab representation.
%
%   max_numeric_collapse_depth: default -1
%       A value of -1 means that arrays should be collapsed where possible.
%       Otherwise arrays are only made into a nd-array if this option value
%       meets or exceeds the dimensionality of the resulting array. For
%       example value of max_numeric_collapse_depth = 1 means that 1D
%       arrays will be returned but that 2D or higher arrays will be
%       returned as cell arrays.
%
%       JSON_STR = '[[1,2,3],[4,5,6]]';
%       1 => {[1,2,3],[4,5,6]}
%       2 =>   [1, 4;
%               2, 5;
%               3, 6]
%
%   max_string_collape_depth : default -1
%
%   max_bool_collapse_depth : default -1
%
%   column_major : default true
%       If true, nd-arrays are read in column-major order,
%       otherwise as row major order. Note that Matlab uses column-major
%       ordering which means that parsing of the data is slightly more
%       efficient when column-major is used.
%
%       For example, consider the following
%       JSON_STR = '[[1,2,3],[4,5,6]]';
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
%
%       For example, consider the following
%       JSON_STR = '[{"a":1,"b":2},{"a":3,"b":5}]';
%
%       collapse_objects =>  1×2 struct array with fields
%       no collapse => 1×2 cell array
%
%
%   Examples
%   --------
%   data = json.load(file_path,'column_major',false);
%
%   See Also
%   --------
%   json.parse

data_options = {};
token_options = {};
if nargin > 1
    if ischar(varargin{1})
        data_options = varargin;
    else
       data_options = varargin{1};
       if isempty(data_options)
           data_options = {};
       end
       if nargin > 2
           token_options = varargin{2};
           if isempty(token_options)
              token_options = {};
           end
       end
    end 
end

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