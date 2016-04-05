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
            value = obj.parse_object.info(1,obj.attribute_indices);
        end
        function value = get.attribute_sizes(obj)
            value = obj.parse_object.info(4,obj.attribute_indices);
        end
    end
    
    properties (Hidden)
        map
        parse_object
    end
    
    methods
        function obj = object_token(name,full_name,index,parse_object)
            obj.name = name;
            obj.full_name = full_name;
            obj.index = index;
            obj.parse_object = parse_object;
            
            raw_string = parse_object.file_string;
            info = parse_object.info;
            
            n_attributes = info(4,index);
            cur_name_I = index+1;
            a_indices = zeros(1,n_attributes,'int32');
            a_names   = cell(1,n_attributes);
            
            
            
            
            %Value: integer
            %TODO: initialize map
            map = containers.Map;
            for iItem = 1:info(4,index)
                cur_value_I =  cur_name_I + 1;
                a_indices(iItem) = cur_value_I;
                temp_name = h__parse_string(raw_string,info,cur_name_I);
                map(temp_name) = iItem;
                a_names{iItem} = temp_name;
                cur_name_I = info(6,cur_value_I);
            end
            obj.map = map;
            obj.attribute_indices = a_indices;
            obj.attribute_names = a_names;
        end
        function output = get_token(obj,name)
            I = obj.map(name);
            full_name = [obj.full_name '.' name];
            index = obj.attribute_indices(I);
            
            p = obj.parse_object;
            
            type = p.info(1,index);
            
            switch type
                case 1
                    error('Not yet implemented');
                case 2
                    output = json.array_token(name,full_name,index,p);
                case 3
                    output = h__parse_string(p.file_string,p.info,index);
                otherwise
                    error('Not yet implemented')
            end
            
        end
    end
    
end

function output_string = h__parse_string(str,j,index)
%TODO: I'm thinking of keeping this in mex
%Currently
output_string = str(j(2,index):j(3,index));
end

