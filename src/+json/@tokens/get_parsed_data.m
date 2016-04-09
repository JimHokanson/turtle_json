function output = get_parsed_data(obj,varargin)

in.collapse_array = true;
in = sl.in.processVarargin(in,varargin);

s = struct;
s.types = obj.types;
s.starts = obj.starts;
s.ends = obj.ends;
s.sizes = obj.sizes;
s.parents = obj.parents;
s.tokens_after_close = obj.tokens_after_close;
s.numeric_data = obj.numeric_data;
s.strings = obj.strings;


if s.types(1) == 1
    output = parse_object(s,in);
elseif s.types(1) == 2
    output = parse_array(s,in);
else
    error('Unexpected parent object')
end

end

function value = parse_primitive(str,j,index,numeric_data)

start_I = j(2,index);
switch(str(start_I))
    case 't'
        value = true;
    case 'f'
        value = false;
    case 'n'
        value = NaN;
    otherwise
        value = numeric_data(index);
end

end

function obj = parse_object(str,j,index,numeric_data,in)

obj = dict;

%3 - string
%2 - array
cur_name_I = index+1;

for iItem = 1:j(4,index)
    cur_value_I =  cur_name_I + 1;
    key = parse_string(str,j,cur_name_I);
    switch(j(1,cur_value_I))
        case 1
            value = parse_object(str,j,cur_value_I,numeric_data,in);
        case 2
            value = parse_array(str,j,cur_value_I,numeric_data,in);
        case 3
            value = parse_string(str,j,cur_value_I);
        case 4
            value = parse_primitive(str,j,cur_value_I,numeric_data);
        otherwise
            error('Unexpected value')
    end
    obj.(key) = value;
    cur_name_I = j(6,cur_value_I);
end
end

function array = parse_array(str,j,index,numeric_data,in)
n_items = j(4,index);
if n_items == 0
   %TODO: This needs an options check
   array = [];
   return
end

next_type = j(1,index+1);
if next_type == 1
    %This let's avoid a lot of work up front
    
    %full_parse = true;
elseif j(6,index)-index == n_items + 1
   %Then we are not nested, only need to check for
   %same value type
   types = str(j(2,index+1:index+n_items));
   
   %TODO: build in type check
   %array = numeric_data(j(2,index)+1:j(3,index)-1);
   array = numeric_data(index+1:j(6,index)-1);
   %temp = textscan(str(j(2,index)+1:j(3,index)-1),'%f','Delimiter',',');
   %array = temp{1}';
   return
else
    
    %We are nested, objects or arrays present ...
    %-----------------------------------------------
    
    %TODO: Validate that we have [[ %2d array
    %any objects - need full parse
    %TODO: Need to check data types
   %TODO: Build in multi-array support, this only works for 2d
   I = (index+1):(j(6,index)-1);
   if I(1) == 2
       
       
   end
   
   array_mask = j(1,I) == 2;
   if ~any(array_mask)
       
   end
   %array_I = find(j(1,index+1:j(6,index)-1)==2)+(double(index));
   array_sizes = j(4,I(array_mask));
   n_arrays = length(array_sizes);
   
   
%        temp = textscan(str(j(2,index)+1:j(3,index)-1),'%f',...
%            'Delimiter',{',','[',']'},'MultipleDelimsAsOne',1);
%        raw_array_data = temp{1}';
   
   raw_array_data = numeric_data(I(~array_mask));

   %raw_array_data = numeric_data(j(2,index)+1:j(3,index)-1);    
       
   if all(array_sizes == array_sizes(1))
       array = reshape(raw_array_data,array_sizes(1),n_arrays);
   else
       array = cell(1,n_arrays);
       end_I = 0;
       for iArray = 1:n_arrays
          start_I = end_I + 1;
          end_I = start_I + array_sizes(iArray)-1;
          array{iArray} = raw_array_data(start_I:end_I);
       end
   end
   return
end

cur_value_I = index + 1;
array = cell(1,n_items);
for iItem = 1:n_items
    switch(j(1,cur_value_I))
        case 1
            value = parse_object(str,j,cur_value_I,numeric_data,in);
        case 2
            value = parse_array(str,j,cur_value_I,numeric_data,in);
        case 3
            value = parse_string(str,j,cur_value_I,in);
        case 4
            value = parse_primitive(str,j,cur_value_I,numeric_data,in);
        otherwise
            error('Unexpected value')
    end
    array{iItem} = value;
    cur_value_I = j(6,cur_value_I);
end

end

function output_string = parse_string(str,j,index)
%TODO: I'm thinking of keeping this in mex
%Currently
output_string = str(j(2,index):j(3,index));
end
