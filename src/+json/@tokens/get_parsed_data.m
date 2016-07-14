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
in = sl.in.processVarargin(in,varargin);

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
        keyboard
    case 5
        keyboard
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

n_keys = s.d1;

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
    cur_key = local_key_names(iItem);
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
            value = s.strings(cur_value_I);
        case 5
            value = s.numeric_data(cur_value_I);
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

n_items = s.d1(index);
tac = s.d2;

if n_items == 0
    output = [];
    return
end

% output = [];
% return;

types = s.types;

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
        keyboard
        if tac(index)-index == n_items + 1 && ...
                all(types(index+1:index+n_items) == 3)

                string_pointer = s.d1;
                tac = lp.d2;
                start_index = string_pointer(index+1);
                end_index = string_pointer(tac(index)-1);

                output      = s.strings(start_index:end_index);
            
            return
        end
    case {5,6}
        if tac(index)-index == n_items + 1 && types(index+n_items) == 5
            
            numeric_pointer = s.d1;
            start_numeric_I = numeric_pointer(index+1);
            end_numeric_I = numeric_pointer(tac(index)-1);
            
            if end_numeric_I - start_numeric_I == n_items - 1
                output = s.numeric_data(start_numeric_I:end_numeric_I);
            end
            
            return
        end
        
    case {7,8} %logical
        if tac(index)-index == n_items + 1
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
tokens_after_close = tac;
for iItem = 1:n_items
    switch s.types(cur_I)
        case 1
            output{iItem} = parse_object(cur_I,s,in);
        case 2
            output{iItem} = parse_array(cur_I,s,in);
        case 3
            output{iItem} = s.strings{cur_I};
        case 4
            output{iItem} = s.numeric_data(cur_I);
        case 5
            output{iItem} = logical(s.numeric_data(cur_I));
        otherwise
            error('Unexpected value')
    end
    cur_I = tokens_after_close(cur_I);
end

end
