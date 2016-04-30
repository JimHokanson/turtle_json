classdef tokens
    %
    %   Class:
    %   json.tokens
    %
    %   See Also:
    %   ---------
    %   json.fileToTokens
    %   json.stringToTokens
    
    %{
    
    Working on new layout to save memory and time
    
    object
    - token after close
    - n_keys
    - parent
    
    array
    - token after close
    - n_elements
    - parent
    
    key
    - token_after_close
    - parent
    - start/pointer
    - end
    
    string
    - start/pointer
    - end
    
    number/null/true/false
    - pointer
    
    
    
    %}
    
    properties
       file_string %string of the file
       %We might not hold onto this ...
       
       types
       %1 - object
       %2 - array
       %3 - string
       %4 - numeric
       %5 - logical
       %6 - key (of an object, string)
       
%        starts
%        ends
       sizes
       parents
       tokens_after_close
       %1) Type
       %2) Start
       %3) End
       %4) Size
       %5) Parent Index - 0 based 
       %6) Next Object - 0 based
       
       mex
       
       numeric_data
       strings
       d0
       d1
       chars_per_token
       ns_per_token
    end
    
    %{
    
    Errors
    ------
    1) No opening bracket
    str = '1,2,3,4';
    jt = json.tokens('','raw_string',str);
    
    2) Trailing whitespace
    str = '[1,2,3,4]    '
    
    
    3) multiple top level tokens
    str = '[1,2,3][4,5,6]'
    
    4) No terminiating quotes on string
    str = '["This is a test';
    
    5) Double :
    str = {"test"::1}
    
    
    
    str = '[1,null,2,3]';
    jt = json.tokens('','raw_string',str);
    obj = parse_json(str,jt.info,jt.numeric_data)
    
    str = '{}'
    jt = json_tokens('','raw_string',str);
    
    str = '[1,null,true,false,"my_string",2.0123456789012345]'
    jt = json.tokens('','raw_string',str);
    obj = parse_json(str,jt.info,jt.numeric_data)
    
    %}
    
    
    methods
        function obj = tokens(file_path,varargin)
            %
            %   obj = json.tokens(file_path,varargin)
            %
            %   See Also:
            %   ---------
            %   json.stringToTokens
            %   json.fileToTokens
            
            in.chars_per_token = 0;
            in.n_tokens = 0;
            in.raw_string = -1;
            in = sl.in.processVarargin(in,varargin);
            
            if in.raw_string == -1
                t0 = tic;
                uint8_data = read_to_uint8(file_path);
                obj.file_string = char(uint8_data);
                obj.d0 = toc(t0);
                t1 = tic;
                if in.n_tokens
                    result = jsmn_mex(uint8_data,in.n_tokens);
                elseif in.chars_per_token
                    result = jsmn_mex(uint8_data,length(obj.file_string)/in.chars_per_token);
                else
                    result = jsmn_mex(uint8_data);
                end
                obj.d1 = toc(t1);
            else
                t0 = tic;
                obj.file_string = in.raw_string;
                obj.d0 = toc(t0);
                t1 = tic;
                if in.n_tokens
                    result = jsmn_mex(in.raw_string,in.n_tokens);
                elseif in.chars_per_token
                    result = jsmn_mex(in.raw_string,length(obj.file_string)/in.chars_per_token);
                else
                    result = jsmn_mex(in.raw_string);
                end
                obj.d1 = toc(t1);
            end
            
                       
            obj.mex = result;
            
            obj.types = result.types;
            obj.sizes = result.sizes;
            obj.parents = result.parents;
            obj.tokens_after_close = result.tokens_after_close;
            obj.numeric_data = result.values;
            obj.strings = result.strings;
            
            obj.chars_per_token = length(obj.file_string)/length(obj.numeric_data);
            obj.ns_per_token    = 1e9*obj.d1/length(obj.numeric_data);
        end
        function root = getRootInfo(obj)
            switch obj.types(1)
                case 1
                    %name,full_name,index,parse_object
                    root = json.token_info.object_token_info('root','root',1,obj);
            	case 2
                    error('Not yet implemented')
                %output = parse_array(str,j,1,numeric_data,in);
                otherwise
                    error('Unexpected parent object')
            end 
        end
        function output = viewOldInfo(obj,indices)
           output = [num2cell(indices);
               json.TYPES(obj.types(indices));
               num2cell(obj.starts(indices));
               num2cell(obj.ends(indices));
               num2cell(obj.sizes(indices));
               num2cell(obj.parents(indices));
               num2cell(obj.tokens_after_close(indices));
               num2cell(obj.numeric_data(indices));
               obj.strings(indices)];
           output = [{'indices','type','start','end','size','parent','token_after_close','value','string'}' output];
        end
    end
    
end

