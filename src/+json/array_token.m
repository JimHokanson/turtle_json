classdef array_token
    %
    %   Class:
    %   json.array_token
    
    properties
        name
        full_name
        index
        n_elements
%         attribute_names
%         attribute_indices
    end
    
    properties (Hidden)
       parse_object
    end
    
    methods
        function obj = array_token(name,full_name,index,parse_object)
            obj.name = name;
            obj.full_name = full_name;
            obj.index = index;
            obj.parse_object = parse_object;
            
            obj.n_elements = parse_object.info(4,index);
        end
    end
    
end

function output_string = h__parse_string(str,j,index)
%TODO: I'm thinking of keeping this in mex
%Currently
output_string = str(j(2,index):j(3,index));
end

