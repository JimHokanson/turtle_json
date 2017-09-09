classdef tokens
    %
    %   Class:
    %   json.tokens
    %
    %   Public Functions
    %   ----------------
    %   json.tokens.load
    %   json.tokens.parse
    
    properties
    end
    
    methods (Static)
        function root = load(file_path,varargin)
            %x Load tokens from file
            %
            %   root = json.tokens.load(file_path,varargin)
            %
            %   Optional Inputs
            %   ---------------
            %   n_tokens
            %   n_keys
            %   n_strings
            %   n_numbers
            %   chars_per_token - NYI
            %   
            %   
            %
            %   See Also
            %   --------
            %   json.load
            
            root = json.tokens.getRootToken(file_path,varargin{:});
        end
        function root = parse(input_string,varargin)
            %x Parse tokens from file
            %
            %   root = json.tokens.parse(file_path,varargin)
            
            
            %TODO: Starting token needs to be an opening object or array
            %If this happens, we might have a file path
            %encountered with json.parse(file_path) which is incorrect
            %- should be json.load(file_path)
            %
            %   In other words, look for the first non-space and
            %   if it is not { or [, then most likely we need to call
            %   json.load(input) instead
            
            root = json.tokens.getRootToken(input_string,varargin{:},'raw_string',true);
        end
    end
    
    methods (Static, Hidden)
        function root = getRootToken(file_path__or__string,varargin)
            %
            %   obj = json.tokens(file_path,varargin)
            %
            %   Optional Inputs
            %   ---------------
            %   Documented in json.load
            %
            %   See Also:
            %   ---------
            %   json.stringToTokens
            %   json.fileToTokens
                        
            mex_result = turtle_json_mex(file_path__or__string,varargin{:});
            
            if mex_result.types(1) == 1
                root = json.objs.token.object('root','root',1,mex_result);
            elseif mex_result.types(1) == 2
                root = json.objs.token.array('root','root',1,mex_result);
            else
                error('Unexpected parent object')
            end                        
        end
        
    end
    
end

