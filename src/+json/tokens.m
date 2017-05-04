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
            %
            %   
            %
            root = json.tokens.getRootToken(file_path,varargin{:});
        end
        function root = parse(input_string,varargin)
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
            %
            %   See Also:
            %   ---------
            %   json.stringToTokens
            %   json.fileToTokens
                        
            %Option Processing
            %-----------------
            
            %TODO: This can be slow in a loop for many small files ...
            in.chars_per_token = json.sl.in.NULL;
            in.n_tokens = json.sl.in.NULL;
            in.n_strings = json.sl.in.NULL;
            in.n_keys = json.sl.in.NULL;
            in.n_numbers = json.sl.in.NULL;
            in.raw_string = json.sl.in.NULL;
            in = json.sl.in.processVarargin(in,varargin,'remove_null',true);
            
            %The main call
            
            %TODO: Starting token needs to be an opening object or array
            %If this happens, we might have a file path
            %encountered with json.parse(file_path) which is incorrect
            %- should be json.load(file_path)
            mex_result = turtle_json_mex(file_path__or__string,in);
            
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

