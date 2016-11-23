function output = get_parsed_data(obj,varargin)
%
%   json.tokens.get_parsed_data
%
%   This should return a generic representation for data.
%
%   Optional Inputs
%   ---------------
%   index : integer
%       The current index to parse
%
%   TODO: Pass in custom function handles ...
%
%   TODO: Ideally this portion would be written in C
%
%   TODO: Rename based on my style getParsedData

% #define TYPE_OBJECT 1
% #define TYPE_ARRAY  2
% #define TYPE_KEY    3
% #define TYPE_STRING 4
% #define TYPE_NUMBER 5
% #define TYPE_NULL   6
% #define TYPE_TRUE   7
% #define TYPE_FALSE  8

in.index = 1;
in.collapse_array = true;
in = json.sl.in.processVarargin(in,varargin);

s = obj;

switch s.types(in.index)
    case 1
        output = parse_object(in.index,s,in);
    case 2
        output = parse_array(in.index,s,in);
    case 3
        %We parse the value from the key
        in.index = in.index + 1;
        output = obj.get_parsed_data(in);
    case 4
        string_I = s.d1(in.index);
        output = s.strings(string_I);
    case 5
        number_I = s.d1(in.index);
        output = s.strings(number_I);
    case 6
        output = NaN;
    case 7
        output = true;
    case 8
        output = false;
    otherwise
        error('Unexpected type')
end

end

function obj = parse_object(index,s,in)

obj = sl.obj.dict;

%3 - string
%2 - array

n_keys = s.child_count;
value_indices = s.value_index;

types = s.types;

[local_key_names,local_key_indices] = s.getKeyInfo(index);

% #define TYPE_OBJECT 1
% #define TYPE_ARRAY  2
% #define TYPE_KEY    3
% #define TYPE_STRING 4
% #define TYPE_NUMBER 5
% #define TYPE_NULL   6
% #define TYPE_TRUE   7
% #define TYPE_FALSE  8

attribute_indices = local_key_indices + 1;


for iItem = 1:n_keys(index)
    cur_key = local_key_names{iItem};
    cur_value_I = attribute_indices(iItem);
    
    switch types(cur_value_I)
        case 1
            value = parse_object(cur_value_I,s,in);
        case 2
            value = parse_array(cur_value_I,s,in);
        case 3
            %This should never happen because the JSON parser won't allow
            %it
            error('A key should not contain a key')
        case 4
            value = s.strings{value_indices(cur_value_I)};
        case 5
            value = s.numeric_data(value_indices(cur_value_I));
        case 6
            value = NaN;
        case 7
            value = true;
        case 8
            value = false;
        otherwise
            error('Unexpected type')
    end
    obj.(cur_key) = value;
end
end

function output = parse_array(index,s,in)

n_items = s.child_count(index);

token_after_close = s.token_after_close;

if n_items == 0
    output = [];
    return
end

types = s.types;
value_indices = s.value_index;

%1) object- go to full loop
%2) array - go to full loop - for now
%3) value - same - single array
%         - diff - go to full loop


% #define TYPE_OBJECT 1
% #define TYPE_ARRAY  2
% #define TYPE_KEY    3
% #define TYPE_STRING 4
% #define TYPE_NUMBER 5
% #define TYPE_NULL   6
% #define TYPE_TRUE   7
% #define TYPE_FALSE  8

switch types(index+1)
    case 1 %object
        %pass
    case 2 %array
        %pass - for now
    case 3 %key
        error('Unexpected type: key')
    case 4 %string
        %cell array of strings?
        %Requires:
        %1) first and last entries of the array are a string
        %2) that there indices are such that all inbetween values must be
        %strings ..
        if types(index + n_items) == 4
            
            I1 = value_indices(index + 1);
            I2 = value_indices(index + n_items);
            
            if (I2 - I1 + 1) == n_items
                output = s.strings(I1:I2);
                return
            end
        end
    case {5,6} %number or null
        if types(index+n_items) == 5
            I1 = value_indices(index + 1);
            I2 = value_indices(index + n_items);
            if (I2 - I1 + 1) == n_items
                output = s.numeric_data(I1:I2);
                return
            end
        end
    case {7,8} %logical
        %note that tac(index) points 1 past the array
        %index also points before the array
        %start value1 value2 value3 next
        %index                      tac(index)
        %1       2     3      4      5
        %
        %5 - 1 = n_items (3) + 1
        if token_after_close(index)-index == n_items + 1
            %Unlike the other types, we don't keep track of an index
            %for these values to know if all values inbetween the first 
            %and the last element are all logicals
            if all(types(index+1:index+n_items) == 7 | types(index+1:index+n_items) == 8);
                output = types(index+1:index+n_items) == 7;
                return
            end
        end
    otherwise
        error('unexpected type')
end

output = cell(1,n_items);
cur_I  = index+1;
tokens_after_close = token_after_close;
for iItem = 1:n_items
    switch s.types(cur_I)
        case 1
            output{iItem} = parse_object(cur_I,s,in);
            cur_I = tokens_after_close(cur_I);
        case 2
            output{iItem} = parse_array(cur_I,s,in);
            cur_I = tokens_after_close(cur_I);
        case 3
            error('Key type should not be an element of an array')
        case 4
            output{iItem} = s.strings{value_indices(cur_I)};
            cur_I = cur_I + 1;
        case {5,6}
            output{iItem} = s.numeric_data(value_indices(cur_I));
            cur_I = cur_I + 1;
        case 7
            output{iItem} = true;
            cur_I = cur_I + 1;
        case 8
            output{iItem} = false;
            cur_I = cur_I + 1;
        otherwise
            error('Unexpected value')
    end
end

end
