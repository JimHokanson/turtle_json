classdef token < handle
    %
    %   Class
    %   json.objs.token
    
    properties
        md_index
        mex
    end
    
    properties (Dependent)
       parse_info 
    end
    
    methods
        function value = get.parse_info(obj)
           value = json.objs.token.parsing_info(obj.mex);
        end
    end
    
    methods
        function data = getParsedData(obj)
            %Call the mex code
            data = json_info_to_data(0,obj.mex,obj.md_index);
        end
    end
    
end

