classdef tokens < handle
    %
    %   Class:
    %   json.tokens
    %
    %   See Also:
    %   ---------
    %   json.fileToTokens
    %   json.stringToTokens
    
    %cell array for each object
    %Each key would need to know its parent :/
    %but, at the end of parsing each object knows its children, so we
    %could post process
    %
    %   1) keep track of n_objects
    %   2) Initialize a cell array for each object
    %   3) for each object, create a struct that has indices which point
    %   to the children of each key
    %   
    %
    %   Alternative:
    %   ---------------
    %   structure array - would need the # of unique keys to be low
    %   
    %   for each key:
    %       check if in the structure
    %
    
    %TYPE_DEFS = {'object','array','key','string','number','null','true','false'};
    
    properties
        json_string %string of the file/string as bytes (without end padding)
        
        object_info
        array_info
        key_info
        
        
        %Data entries per token
        %----------------------------------------------------------
        types 
        data_indices
        
        %TODO: rename to numbers
        numbers
        strings
        
        mex %structure that is populated from mex data
    end
    
    properties (Dependent)
       parse_info 
    end
    
    methods
        function value = get.parse_info(obj)
           value = obj.h_parsed_info;
           if isempty(value)
              value = json.token_info.parsing_info(obj, obj.mex);
              obj.h_parsed_info = value;
           end
        end
    end
    
    properties (Hidden)
        toc_total_time
        h_parsed_info
    end
    
    methods
        function obj = tokens(file_path,varargin)
            %
            %   obj = json.tokens(file_path,varargin)
            %
            %   See Also:
            %   ---------
            %   json.stringToTokens
            %   json.fileToTokens
            
            %These still need to be reimplemented ...
            
            %Option Processing
            %-----------------
            in.chars_per_token = json.sl.in.NULL;
            in.n_tokens = json.sl.in.NULL;
            in.n_strings = json.sl.in.NULL;
            in.n_keys = json.sl.in.NULL;
            in.n_numbers = json.sl.in.NULL;
            in.raw_string = json.sl.in.NULL;
            in.raw_is_padded = false;
            in = json.sl.in.processVarargin(in,varargin,'remove_null',true);
            
            t0 = tic;
            t1 = cputime;
            %The main call
            result = turtle_json_mex(file_path,in);
            obj.toc_total_time = toc(t0);
            
            obj.mex = result;
            
            obj.json_string = result.json_string;
            
            obj.types = result.types;
            obj.data_indices = result.d1;
            
            
            obj.numbers = result.numeric_p;            
            obj.strings = result.strings;
            
        end
        function data = getParsedData(obj)
            %Call the mex code
            data = json_info_to_data(0,obj.mex,1);
        end
        function root = getRootInfo(obj)
            switch obj.types(1)
                case 1
                    %name,full_name,index,parse_object
                    root = json.token_info.object_token_info('root','root',1,obj.mex);
                case 2
                    root = json.token_info.array_token_info('root','root',1,obj.mex);
                    %error('Not yet implemented')
                    %output = parse_array(str,j,1,numeric_data,in);
                otherwise
                    error('Unexpected parent object')
            end
        end
    end
    
end

