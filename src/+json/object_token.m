classdef object_token
    %
    %   Class:
    %   json.object_token
    
    properties
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
            %TODO: replace with strings
            temp  = obj.p.types(obj.attribute_indices);
            value = json.TYPES(temp);
            
            %value = obj.parse_object.info(1,obj.attribute_indices);
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
        function obj = object_token(name,full_name,index,parse_object)
            obj.name = name;
            obj.full_name = full_name;
            obj.index = index;
            obj.p = parse_object;
            
            p = parse_object;
            
            raw_string = p.file_string;
            
            n_attributes = p.sizes(index);
            cur_name_I = index+1;
            a_indices = zeros(1,n_attributes,'int32');
            a_names   = cell(1,n_attributes);
            
            %Value: integer
            %TODO: initialize map with string and indices
            map = containers.Map;
            for iItem = 1:n_attributes
                cur_value_I =  cur_name_I + 1;
                a_indices(iItem) = cur_value_I;
                temp_name = h__parse_string(raw_string,p,cur_name_I);
                map(temp_name) = iItem;
                a_names{iItem} = temp_name;
                cur_name_I = p.tokens_after_close(cur_value_I);
            end
            obj.map = map;
            obj.attribute_indices = a_indices;
            obj.attribute_names = a_names;
        end
        function output = getToken(obj,name)
            
            %TODO: Surround with try catch on invalid key name
            I = obj.map(name);
            
            full_name = [obj.full_name '.' name];
            index = obj.attribute_indices(I);
            
            p = obj.p;
            
            switch p.types(index);
                case 1
                    output = json.object_token(name,full_name,index,p);
                case 2
                    output = json.array_token(name,full_name,index,p);
                case 3
                    output = h__parse_string(p.file_string,index);
                otherwise
                    error('Not yet implemented')
            end
            
        end
    end
    
end

function output_string = h__parse_string(str,p,index)
%TODO: I'm thinking of keeping this in mex
%Currently
output_string = str(p.starts(index):p.ends(index));
end

