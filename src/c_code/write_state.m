classdef write_state < handle
    %UNTITLED3 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        I
        n
        
        %arrays
        buffer  %data
        depth   %depth
        next
        type
        index
        %arrays
        
        out
        out_I
        indent = true;
    end
 
    
    methods
        function obj = write_state()
          	obj.I = 0;
            obj.n = 1;
            obj.buffer = cell(1,100);
            obj.depth = zeros(1,100,'uint8');
            obj.next = zeros(1,100);
            obj.type = zeros(1,100,'uint8');
            obj.index = zeros(1,100,'uint8');
            obj.out = zeros(1,1000,'uint8');
            obj.out_I = 0;
        end
    end
    
    methods
        function initArray(obj)
            %cur_depth = obj.depth(obj.I);            
            %pretty print ...
        	if obj.indent
                cur_depth = obj.depth(obj.I);            
                str_to_add = uint8([repmat(32,1,cur_depth*5) 91 10]);
                h__addString(obj,str_to_add);
            else
                str_to_add = uint8([91 10]);
                h__addString(obj,str_to_add);
            end 
            obj.indent = true;
        end
        function addNextObjectArrayElement(obj)

            cur_data = obj.buffer{obj.I};
            current_index = obj.index(obj.I);
            if current_index == length(cur_data)
                keyboard
            end
            
            next_index = current_index + 1;
            obj.index(obj.I) = next_index;
            
            s = obj.buffer{obj.I}(next_index);
            addStructToBuffer(obj,s)
        end
        function addArrayElement(obj)
            
        end
        function initObject(obj)
            if obj.indent
                cur_depth = obj.depth(obj.I);            
                str_to_add = uint8([repmat(32,1,cur_depth*5) 123 10]);
                h__addString(obj,str_to_add);
            else
                str_to_add = uint8([123 10]);
                h__addString(obj,str_to_add);
            end
            obj.indent = true;
        end
      	function addNextKey(obj)
            %
            %   - at parent
            
            s = obj.buffer{obj.I};
            fn = fieldnames(s);
            
            current_index = obj.index(obj.I);
            if current_index == length(fn)
                %
                %   - close object
                keyboard
            end
            next_index = current_index + 1;
            obj.index(obj.I) = next_index;
            cur_name = fn{next_index};
            
            cur_depth = obj.depth(obj.I);
            
            %Add
            str_to_add = uint8([repmat(32,1,(cur_depth+1)*5) 34 ...
                uint8(cur_name) 34 58 32]);
            h__addString(obj,str_to_add);
            obj.indent = false;
            
            
            %If simple, just convert
            %- string
            %- logical
            %- numeric
            
            next_value = s.(cur_name);
            
            if isstruct(next_value)
                obj.addStructToBuffer(next_value)
            elseif iscell(next_value)
                keyboard
            else
                if isnumeric(next_value)
                    h__addNumericToString(obj,next_value)
                else
                    keyboard
                end
                if next_index == length(fn)
                    %close
                    %
                    %JAH: At this point
                    %- 
                    keyboard
                else
                    %add comma
                    h__addString(obj,uint8([44 10]))
                    
                end
            end
        end
        function addStructToBuffer(obj,s)
            next_I = obj.I + 1;
            obj.n = next_I;
            obj.buffer{next_I} = s;
            
            obj.depth(next_I) = obj.depth(obj.I)+1;
            obj.next(next_I) = obj.I;
            if length(s) > 1
                obj.type(next_I) = 0;           
            else
                obj.type(next_I) = 1;
            end
            obj.index(next_I) = 0;
            obj.I = obj.I + 1;
        end
        function addCellToBuffer(obj)
            
        end
    end
    
end

function h__addNumericToString(obj,numeric_array)
    h__addString(obj,uint8('NUMBER GOES HERE'));
    return

    %NOT YET IMPLEMENTED
    if isscalar(numeric_array)
        keyboard
    elseif isvector(numeric_array)
        keyboard
    elseif ismatrix(numeric_array)
        %row or column major ...
        keyboard
    else
        keyboard
    end
end

function h__addString(obj,str_to_add)
    end_I = obj.out_I + length(str_to_add);
    if length(obj.out) < end_I
        temp = obj.out;
        obj.out = zeros(1,2*length(temp));
        obj.out(1:length(temp)) = temp;
    end
    obj.out(obj.out_I+1:end_I) = str_to_add;
    obj.out_I = end_I;
end
