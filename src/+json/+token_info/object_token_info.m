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
    
    properties (Hidden)
        map
        p
    end
    
    methods
        function output = getParsedData(obj,varargin)
            %TODO: rename get_parsed_data to getParsedData
           output = obj.p.get_parsed_data(varargin{:}); 
        end
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
            
            n_attributes = p.d1(index);
            
            [local_key_names,local_key_indices] = p.getKeyInfo(index);
            
            obj.key_indices = local_key_indices;
            obj.key_names = local_key_names;
            obj.attribute_indices = local_key_indices + 1;
            obj.attribute_types = p.types(obj.attribute_indices);
            
            obj.map = containers.Map(obj.key_names,1:n_attributes);
            
        end
		function getNumericArray(obj,name)
			error('Code not yet implemented')
		end
        function output = getToken(obj,name)

            I = h__getMapIndex(obj,name);

            local_full_name = [obj.full_name '.' name];
            local_index = obj.attribute_indices(I);
            
            lp = obj.p;
            
            
            % #define TYPE_OBJECT 1
            % #define TYPE_ARRAY  2
            % #define TYPE_KEY    3
            % #define TYPE_STRING 4
            % #define TYPE_NUMBER 5
            % #define TYPE_NULL   6
            % #define TYPE_TRUE   7
            % #define TYPE_FALSE  8
            
            
            switch lp.types(local_index)
                case 1
                    output = json.token_info.object_token_info(name,local_full_name,local_index,lp);
                case 2
                    output = json.token_info.array_token_info(name,local_full_name,local_index,lp);
                case 3
                    error('Unexpected value type of key')
                case 4
                    output = lp.strings{lp.d1(local_index)};
                case 5
                    output = lp.numeric_data(lp.d1(local_index));
                case 6
                    output = NaN;
                case 7
                    output = true;
                case 8
                    output = false;
                otherwise
                    error('Unrecognized token type: %d',lp.types(local_index))
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
                      
           lp = obj.p;
           numeric_pointer = lp.d1;
           
           I = h__getMapIndex(obj,name); 
           local_index = obj.attribute_indices(I);
           %TODO: Check type
           output = obj.p.numeric_data(numeric_pointer(local_index)); 
        end
        function output = getTokenString(obj,name)
           %
           %    Use this to retrieve a string token. This function also
           %    checks that the name of the requested key is a string

           lp = obj.p;
           I = h__getMapIndex(obj,name); 
           local_index = obj.attribute_indices(I);
           output = lp.strings{lp.d1(local_index)};
        end
        function output = getStringOrCellstr(obj,name)
           
           lp = obj.p;
           I = h__getMapIndex(obj,name); 
           local_index = obj.attribute_indices(I);
           if lp.types(local_index) == 2
               %array
               %TODO: We can avoid the object creation ...
               temp = json.token_info.array_token_info('','',local_index,obj.p);
               output = temp.getCellstr();
           else
               %TODO: Verify type is string
               string_pointer = lp.d1(local_index);
               output = lp.strings{string_pointer};
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