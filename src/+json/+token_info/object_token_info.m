classdef object_token_info
    %
    %   Class:
    %   json.token_info.object_token_info
    %
    %   Consumption methods
    %   -------------------
    %   1) getParsedObjectData(s_or_obj,index,fields_to_ignore)
    
    properties
        type = 'object'
        name
        full_name
        md_index
    end
    
    properties
        key_indices
        key_names
        attribute_indices
        attribute_types
    end
    
    properties (Hidden)
        p
    end
    
    methods
        function output = getParsedData(obj,varargin)
            %TODO: rename getParsedData to getParsedData
            error('This needs to be fixed')
            %Involves passing in the index ...
           output = obj.p.getParsedData(varargin{:}); 
        end
        function obj = object_token_info(name,full_name,md_index,parse_object)
            obj.name = name;
            obj.full_name = full_name;
            obj.md_index = md_index;
            obj.p = parse_object;
        end
		function getNumericArray(obj,name)
			error('Code not yet implemented')
		end
        function output = getToken(obj,name)
            %v3
            %
            %  
            
            key_index = json_info_to_data(1,obj.p,obj.md_index,name);
            
            if ~key_index
               error('Key not a member of the specified object') 
            end
            
            [key_value_type,key_value_md_index] = json_info_to_data(2,obj.p,obj.md_index,key_index);
                        
            lp = obj.p;
            
            
            % #define TYPE_OBJECT 1
            % #define TYPE_ARRAY  2
            % #define TYPE_KEY    3
            % #define TYPE_STRING 4
            % #define TYPE_NUMBER 5
            % #define TYPE_NULL   6
            % #define TYPE_TRUE   7
            % #define TYPE_FALSE  8
            
            
            switch key_value_type
                case 1
                    local_full_name = [obj.full_name '.' name];
                    output = json.token_info.object_token_info(name,local_full_name,key_value_md_index,lp);
                case 2
                    local_full_name = [obj.full_name '.' name];
                    output = json.token_info.array_token_info(name,local_full_name,key_value_md_index,lp);
                case 3
                    error('Unexpected value type of key')
                case 4
                    output = lp.strings{lp.d1(key_value_md_index)};
                case 5
                    %TODO: Support scalars
                    output = lp.numeric_p(lp.d1(key_value_md_index));
                case 6
                    output = NaN;
                case 7
                    output = true;
                case 8
                    output = false;
                otherwise
                    error('Unrecognized token type: %d',key_value_type)
            end
        end
        function output = getParsedToken(obj,name)
            %TODO: I think I want a method that explicitly merges
            %these two lines, getTokenIndex
            I = h__getMapIndex(obj,name);
            local_index = obj.attribute_indices(I);
            
            output = obj.p.getParsedData('index',local_index);
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