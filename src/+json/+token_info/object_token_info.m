classdef object_token_info
    %
    %   Class:
    %   json.token_info.object_token_info
    
    properties
        type = 'object'
        name
        full_name
        index
        key_indices
        key_names
        attribute_indices
        attribute_types
        

    end
    
%     properties (Dependent)
%         attribute_sizes
%     end
%     
%     methods
%         function value = get.attribute_sizes(obj)
%             value = obj.p.sizes(obj.attribute_indices);
%         end
%     end
    
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
            
%     object: 1) type  2) n_values        3) tac
%     array:  1) type  2) n_values        3) tac
%     key:    1) type  2) start_pointer   3) tac
%             
%     string: 1) type  2) start_pointer   3) end of string
%     number: 1) type  2) start_pointer
%     null:   1) type  2) start_pointer
%     tf      1) type
    
            d2 = p.d2;
            d1 = p.d1;
            
            %We might change this approach later on ...
            
           key_data = p.key_data;
           key_starts = p.key_starts;
           key_ends = p.key_ends;
            
            
            n_attributes = p.d1(index);
            key_start_I  = index + 1;
            
            local_key_indices = zeros(1,n_attributes,'int32');
            local_key_names = cell(1,n_attributes);
            
            for iItem = 1:n_attributes
               
               local_key_indices(iItem) = key_start_I;
               
               cur_I = d1(key_start_I);
               local_key_names{iItem} = key_data(key_starts(cur_I):key_ends(cur_I));
               key_start_I = d2(key_start_I);
            end
            
            obj.key_indices = local_key_indices;
            obj.key_names = local_key_names;
            obj.attribute_indices = local_key_indices + 1;
            obj.attribute_types = p.types(obj.attribute_indices);
            
            obj.map = containers.Map(obj.key_names,1:n_attributes);
            
        end
        function output = getToken(obj,name)
            
            keyboard
            
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
           
           keyboard
            
           I = h__getMapIndex(obj,name); 
           local_index = obj.attribute_indices(I);
           %TODO: Check type
           output = obj.p.numeric_data(local_index); 
        end
        function output = getTokenString(obj,name)
           %
           %    Use this to retrieve a string token. This function also
           %    checks that the name of the requested key is a string
           
           keyboard
           
           I = h__getMapIndex(obj,name); 
           local_index = obj.attribute_indices(I);
           %TODO: Check type
           output = obj.p.strings{local_index};
        end
        function output = getStringOrCellstr(obj,name)
            
           keyboard 
            
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