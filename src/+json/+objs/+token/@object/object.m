classdef object < json.objs.token
    %
    %   Class:
    %   json.objs.token.object
    
    properties
        name
        full_name
    end
    
    properties (Dependent)
        %TODO: Implement these
        %key_indices
        key_names
        %attribute_indices
        %attribute_types
    end
    
    methods
        function value = get.key_names(obj)
            value = obj.p__key_names;
            if isempty(value)
                lp = obj.mex;
                %Stored indices are all 0 based
                obj_data_index = lp.d1(obj.md_index)+1;
                obj_info = lp.object_info;
                object_id = obj_info.object_ids(obj_data_index)+1;
                ref_object = obj_info.objects{object_id};
                value = fieldnames(ref_object);
                obj.p__key_names = value;
            end
        end
    end
    
    properties (Hidden)
        p__key_names
    end
    
    methods
        function obj = object(name,full_name,md_index,parse_object)
            obj.name = name;
            obj.full_name = full_name;
            obj.md_index = md_index;
            obj.mex = parse_object;
        end
        function [data,key_locations] = parseExcept(obj,keys_to_not_parse,keep_keys)
            %
            %   
            %   [data,key_locations] = parseExcept(obj,keys_to_not_parse,*keep_keys)
            %
            %   Inputs
            %   ------
            %   keys_to_not_parse:
            %   keep_keys:
            %
            %   Outputs
            %   -------
            %   data: 
            %   key_locations:
            %
            
            if nargin == 2
                keep_keys = true;
            else
                keep_keys = logical(keep_keys);
            end
            if ischar(keys_to_not_parse)
                keys_to_not_parse = {keys_to_not_parse};
            end
            [data,key_locations] = json_info_to_data(6,obj.mex,obj.md_index,keys_to_not_parse,keep_keys);
        end
        function nd_arrray = getNumericArray(obj,name)
            [key_value_type,key_value_md_index] = h__getKeyValueInfo(obj,name);
            %json_info_to_data(4,mex_struct,array_md_index,expected_array_type)
            nd_arrray = json_info_to_data(4,obj.mex,key_value_md_index,0);
        end
        function output = getToken(obj,name)
            %v3
            %
            %
            [key_value_type,key_value_md_index] = h__getKeyValueInfo(obj,name);
            lp = obj.mex;
            
            switch key_value_type
                case 1
                    local_full_name = [obj.full_name '.' name];
                    output = json.objs.token.object(name,local_full_name,key_value_md_index,lp);
                case 2
                    local_full_name = [obj.full_name '.' name];
                    output = json.objs.token.array(name,local_full_name,key_value_md_index,lp);
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
            %v3
            [~,key_value_md_index] = h__getKeyValueInfo(obj,name);
            output = json_info_to_data(0,s.mex,key_value_md_index);
        end
        %function output = getPartialToken(obj,name
        function output = getArrayToken(obj,name)
            %v3
            [key_value_type,key_value_md_index] = h__getKeyValueInfo(obj,name);
            if key_value_type ~= 2
                error('Requested key: "%s" is not an array',name);
            end
            local_full_name = [obj.full_name '.' name];
            output = json.token_info.array_token_info(name,local_full_name,key_value_md_index,obj.mex);
        end
        function output = getNumericToken(obj,name)
            %v3
            [key_value_type,key_value_md_index] = h__getKeyValueInfo(obj,name);
            if ~(key_value_type == 5 || key_value_type == 6)
                error('Requested key: "%s" is not a number',name);
            end
            output = obj.mex.numeric_p(key_value_md_index);
        end
        function output = getTokenString(obj,name)
            %v3
            %    Use this to retrieve a string token.
            [key_value_type,key_value_md_index] = h__getKeyValueInfo(obj,name);
            if key_value_type ~= 4
                error('Requested key: "%s" is not a string',name);
            end
            output = obj.mex.strings{key_value_md_index};
        end
        function output = getStringOrCellstr(obj,name)
            [key_value_type,key_value_md_index] = h__getKeyValueInfo(obj,name);
            
            lp = obj.mex;
            if key_value_type == 4
                string_pointer = lp.d1(key_value_md_index);
                output = lp.strings{string_pointer};
            elseif key_value_type == 2
                output = json_info_to_data(3,lp,key_value_md_index);
            else
                error('string or cellstr not found')
            end
        end
    end
    
end

function [key_value_type,key_value_md_index] = h__getKeyValueInfo(obj,name)
key_index = json_info_to_data(1,obj.mex,obj.md_index,name);
[key_value_type,key_value_md_index] = json_info_to_data(2,obj.mex,obj.md_index,key_index);
end