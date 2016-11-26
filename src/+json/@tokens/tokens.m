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
        file_string %string of the file as bytes (with end padding)
        %TODO: truncate the end padding by shortening the length of the 
        %mxArray
        
        %Data entries per token
        %----------------------------------------------------------
        types 

        %TODO: next_sibling_index is a better name
        token_after_close %Only valid for objects, arrays, and keys
        
        value_index %index of the value into array of that data type
        %numeric or null => 'numeric_data' property
        %string => 'strings' property
        %key => 'keys' property
        %
        %Value is not valid for other types
        child_count
        
        %************ Current Definitions ************
        %                       d1                  d2
        %     object: 1) type  2) n_values        3) tac
        %     array:  1) type  2) n_values        3) tac
        %     key:    1) type  2) start_pointer   3) tac
        %
        %     string: 1) type  2) start_pointer   3) end of string
        %     number: 1) type  2) start_pointer
        %     null:   1) type  2) start_pointer
        %     tf      1) type
        
        %TODO: rename to numbers
        numeric_data
        keys 
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
              value = json.token_info.parsing_info(obj, obj.mex, obj.toc_total_time);
              obj.h_parsed_info = value;
           end
        end
    end
    
    properties (Hidden)
        toc_total_time
        h_parsed_info
        d1
        d2
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
            %The main call
            result = turtle_json_mex(file_path,in);
            obj.toc_total_time = toc(t0);
            
            obj.mex = result;
            
            obj.file_string = result.json_string;
            
            obj.types = result.types;
            obj.d1 = result.d1;
            obj.d2 = result.d2;
            
            %Aliasing ------------------------
            obj.token_after_close = obj.d2;
            obj.value_index = obj.d1;
            obj.child_count = obj.d1;
            
            
            obj.numeric_data = result.numeric_p;            
            obj.strings = result.strings;
            %obj.keys = result.keys;
            
        end
        function root = getRootInfo(obj)
            switch obj.types(1)
                case 1
                    %name,full_name,index,parse_object
                    root = json.token_info.object_token_info('root','root',1,obj);
                case 2
                    root = json.token_info.array_token_info('root','root',1,obj);
                    %error('Not yet implemented')
                    %output = parse_array(str,j,1,numeric_data,in);
                otherwise
                    error('Unexpected parent object')
            end
        end
    end
    
end

