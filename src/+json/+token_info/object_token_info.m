classdef object_token_info < handle
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
    
    properties (Dependent)
        key_indices
        key_names
        attribute_indices
        attribute_types
    end
    
    methods
        function value = get.key_names(obj)
            value = obj.p__key_names;
            if isempty(value)
                lp = obj.p;
                obj_data_index = lp.d1(obj.md_index)+1;
                obj_info = lp.object_info;
                ref_object = obj_info.objects{obj_info.object_ids(obj_data_index)};
                value = fieldnames(ref_object);
                obj.p__key_names = value;
            end
        end
    end
    
    properties (Hidden)
        p__key_names
        p
    end
    
    methods
        function output = getParsedData(obj,varargin)
            if ~isempty(varargin)
                error('vargin not yet handled')
            end
            output = json_info_to_data(0,s.mex,obj.md_index);
%             %TODO: rename getParsedData to getParsedData
%             error('This needs to be fixed')
%             %Involves passing in the index ...
%             output = obj.p.getParsedData(varargin{:});
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
            [key_value_type,key_value_md_index] = h__getKeyValueInfo(obj,name);
            lp = obj.p;

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
            %v3
        	[~,key_value_md_index] = h__getKeyValueInfo(obj,name);
            output = json_info_to_data(0,s.mex,key_value_md_index);
        end
        function output = getArrayToken(obj,name)
            %v3
            [key_value_type,key_value_md_index] = h__getKeyValueInfo(obj,name);
            if key_value_type ~= 2
               error('Requested key: "%s" is not an array',name); 
            end
            local_full_name = [obj.full_name '.' name];
            output = json.token_info.array_token_info(name,local_full_name,key_value_md_index,obj.p);
        end
        function output = getNumericToken(obj,name)
            %v3
            [key_value_type,key_value_md_index] = h__getKeyValueInfo(obj,name);
            if ~(key_value_type == 5 || key_value_type == 6)
               error('Requested key: "%s" is not a number',name); 
            end
            output = obj.p.numeric_p(key_value_md_index);
        end
        function output = getTokenString(obj,name)
            %v3
            %    Use this to retrieve a string token.
            [key_value_type,key_value_md_index] = h__getKeyValueInfo(obj,name);
            if key_value_type ~= 4
               error('Requested key: "%s" is not a string',name); 
            end
            output = obj.p.strings{key_value_md_index};
        end
        function output = getStringOrCellstr(obj,name)
            [key_value_type,key_value_md_index] = h__getKeyValueInfo(obj,name);
            
            lp = obj.p;
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
key_index = json_info_to_data(1,obj.p,obj.md_index,name);
[key_value_type,key_value_md_index] = json_info_to_data(2,obj.p,obj.md_index,key_index);
end