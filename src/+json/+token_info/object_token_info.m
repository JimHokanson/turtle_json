classdef object_token_info
    %
    %   Class:
    %   json.token_info.object_token_info
    
    properties
        type = 'object'
        name
        full_name
        index
        attribute_names
        attribute_indices
    end
    
    properties (Dependent)
        attribute_types
        attribute_sizes
    end
    
    methods
        function value = get.attribute_types(obj)
            temp  = obj.p.types(obj.attribute_indices);
            value = json.TYPES(temp);
        end
        function value = get.attribute_sizes(obj)
            value = obj.p.sizes(obj.attribute_indices);
        end
    end
    
    properties (Hidden)
        map
        p
    end
    
    methods
        function obj = object_token_info(name,full_name,index,parse_object)
            obj.name = name;
            obj.full_name = full_name;
            obj.index = index;
            obj.p = parse_object;
            
            p = parse_object;
            
%         object: 1) type  2) parent      3) tac     4) n_values
%         array:  1) type  2) parent      3) tac     4) n_values
%         key:    1) type  2) parent      3) tac     4) start/p     5) end
%         string: 1) type  2) start/p     3) end
%         number: 1) type  2) start/p     3) info
%         null:   1) type  2) <nothing>/p 3) <nothing>
%         t/f:    1) type
            
            n_attributes = p.data(index+3);
            cur_name_I   = index+4;
            
            a_indices = zeros(1,n_attributes,'int32');
            a_names   = cell(1,n_attributes);
                        
            %Value: integer
            %TODO: initialize map with string and indices
            map = containers.Map;
            for iItem = 1:n_attributes
                cur_value_I =  cur_name_I + 1;
                a_indices(iItem) = cur_value_I;
                temp_name = p.strings{cur_name_I};
                map(temp_name) = iItem;
                a_names{iItem} = temp_name;
                cur_name_I = p.tokens_after_close(cur_name_I);
            end
            obj.map = map;
            obj.attribute_indices = a_indices;
            obj.attribute_names = a_names;
        end
        function output = getToken(obj,name)
            
            I = h__getMapIndex(obj,name);
            
            local_full_name = [obj.full_name '.' name];
            local_index = obj.attribute_indices(I);
            
            local_p = obj.p;
            
            switch local_p.types(local_index)
                case 1
                    output = json.token_info.object_token_info(name,local_full_name,local_index,local_p);
                case 2
                    output = json.token_info.array_token_info(name,local_full_name,local_index,local_p);
                case 3
                    output = obj.p.strings{local_index};
                case 4
                    output = obj.p.numeric_data(local_index);
                case 5
                    output = logical(obj.p.numeric_data(local_index));
                otherwise
                    error('Unrecognized token type: %d',local_p.types(local_index))
            end
        end
        function output = getParsedToken(obj,name)
            %TODO: I think I want a method that explicitly merges
            %these two lines, getTokenIndex
            I = h__getMapIndex(obj,name);
            local_index = obj.attribute_indices(I);
            
            output = obj.p.get_parsed_data('index',local_index);
        end
        function output = getArrayToken(obj,name)
            I = h__getMapIndex(obj,name);
            
            local_full_name = [obj.full_name '.' name];
            local_index = obj.attribute_indices(I);
            
            %TODO: run check on type ...
            
            local_p = obj.p; 
            output = json.token_info.array_token_info(name,local_full_name,local_index,local_p);
        end
        function output = getNumericToken(obj,name)
                      I = h__getMapIndex(obj,name); 
           local_index = obj.attribute_indices(I);
           %TODO: Check type
           output = obj.p.numeric_data(local_index); 
        end
        function output = getTokenString(obj,name)
           %
           %    Use this to retrieve a string token. This function also
           %    checks that the name of the requested key is a string
           
           I = h__getMapIndex(obj,name); 
           local_index = obj.attribute_indices(I);
           %TODO: Check type
           output = obj.p.strings{local_index};
        end
        function output = getStringOrCellstr(obj,name)
           I = h__getMapIndex(obj,name); 
           local_index = obj.attribute_indices(I);
           if obj.p.types(local_index) == 2
               %array
               %TODO: We can avoid the object creation ...
               temp = json.token_info.array_token_info('','',local_index,obj.p);
               output = temp.getCellstr();
           else
               output = obj.p.strings{local_index};
           end
        end
    end
    
end

function I = h__getMapIndex(obj,name)
    %TODO: wrap with try/catch on name
    I = obj.map(name); 
end

%NYI
% function h__ensureType(obj,local_index,type)
% 
% end