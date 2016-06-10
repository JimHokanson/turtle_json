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
cur_name_I = index+1;

keyboard

for iItem = 1:s.sizes(index)
    cur_value_I =  cur_name_I + 1;
    key = s.strings{cur_name_I};
    switch s.types(cur_value_I)
        case 1
            value = parse_object(cur_value_I,s,in);
        case 2
            value = parse_array(cur_value_I,s,in);
        case 3
            value = s.strings{cur_value_I};
        case 4
            value = s.numeric_data(cur_value_I);
        case 5
            value = logical(s.numeric_data(cur_value_I));
        otherwise
            error('Unexpected value')
    end
    obj.(key) = value;
    cur_name_I = s.tokens_after_close(cur_name_I);
end
end

function output = parse_array(index,s,in)

n_items = s.sizes(index);

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

switch types(index+1)
    case 1 %object
        %pass
    case 2 %array
        %pass - for now
    case 3 %string
        if s.tokens_after_close(index)-index == n_items + 1 && ...
                all(s.types(index+1:index+n_items) == 3)
           output = s.strings(index+1:index+n_items);
           return
        end
    case 4
        if s.tokens_after_close(index)-index == n_items + 1 && ...
                all(s.types(index+1:index+n_items) == 4)
           output = s.numeric_data(index+1:index+n_items); 
           return
        end
    case 5
        if s.tokens_after_close(index)-index == n_items + 1 && ...
                all(s.types(index+1:index+n_items) == 5)
           output = logical(s.numeric_data(index+1:index+n_items)); 
           return
        end
    otherwise
        error('unexpected type')
end

output = cell(1,n_items);
cur_I  = index+1;
tokens_after_close = s.tokens_after_close;
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
        
%Old code related to matrix parsing ...            

% % % % next_type = j(1,index+1);
% % % % if next_type == 1
% % % %     %This let's avoid a lot of work up front
% % % %     
% % % %     %full_parse = true;
% % % % elseif j(6,index)-index == n_items + 1
% % % %    %Then we are not nested, only need to check for
% % % %    %same value type
% % % %    types = str(j(2,index+1:index+n_items));
% % % %    
% % % %    %TODO: build in type check
% % % %    %array = numeric_data(j(2,index)+1:j(3,index)-1);
% % % %    output = numeric_data(index+1:j(6,index)-1);
% % % %    %temp = textscan(str(j(2,index)+1:j(3,index)-1),'%f','Delimiter',',');
% % % %    %array = temp{1}';
% % % %    return
% % % % else
% % % %     
% % % %     %We are nested, objects or arrays present ...
% % % %     %-----------------------------------------------
% % % %     
% % % %     %TODO: Validate that we have [[ %2d array
% % % %     %any objects - need full parse
% % % %     %TODO: Need to check data types
% % % %    %TODO: Build in multi-array support, this only works for 2d
% % % %    I = (index+1):(j(6,index)-1);
% % % %    if I(1) == 2
% % % %        
% % % %        
% % % %    end
% % % %    
% % % %    array_mask = j(1,I) == 2;
% % % %    if ~any(array_mask)
% % % %        
% % % %    end
% % % %    %array_I = find(j(1,index+1:j(6,index)-1)==2)+(double(index));
% % % %    array_sizes = j(4,I(array_mask));
% % % %    n_arrays = length(array_sizes);
% % % %    
% % % %    
% % % % %        temp = textscan(str(j(2,index)+1:j(3,index)-1),'%f',...
% % % % %            'Delimiter',{',','[',']'},'MultipleDelimsAsOne',1);
% % % % %        raw_array_data = temp{1}';
% % % %    
% % % %    raw_array_data = numeric_data(I(~array_mask));
% % % % 
% % % %    %raw_array_data = numeric_data(j(2,index)+1:j(3,index)-1);    
% % % %        
% % % %    if all(array_sizes == array_sizes(1))
% % % %        output = reshape(raw_array_data,array_sizes(1),n_arrays);
% % % %    else
% % % %        output = cell(1,n_arrays);
% % % %        end_I = 0;
% % % %        for iArray = 1:n_arrays
% % % %           start_I = end_I + 1;
% % % %           end_I = start_I + array_sizes(iArray)-1;
% % % %           output{iArray} = raw_array_data(start_I:end_I);
% % % %        end
% % % %    end
% % % %    return
% % % % end



end
