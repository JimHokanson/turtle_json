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
    
    
    
    
    object: 1) type  2) n_values        3) tac
    array:  1) type  2) n_values        3) tac
    key:    1) type  2) start_pointer   3) tac
    string: 1) type  2) start_pointer
    number: 1) type  2) start_pointer
    null:   1) type  2) start_pointer
    tf      1) type
    
    
    
    array:  1) type  2) parent      3) tac     4) n_values
    key:    1) type  2) parent      3) tac     4) start/p     5) end
    string: 1) type  2) start/p     3) end
    number: 1) type  2) start/p     3) info
    null:   1) type  2) <nothing>/p 3) <nothing>
    t/f:    1) type
    
    
    
    Working on new layout to save memory and time
    
    object: 1) type  2) parent      3) tac     4) n_values
    array:  1) type  2) parent      3) tac     4) n_values
    key:    1) type  2) parent      3) tac     4) start/p     5) end
    string: 1) type  2) start/p     3) end
    number: 1) type  2) start/p     3) info
    null:   1) type  2) <nothing>/p 3) <nothing>
    t/f:    1) type
    
    RULES:
    1) parent-1 => type
    2) parent+2 => n_values (for array and object)
    3
    
    
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
        d1
        d2
        %type-ids
        % #define TYPE_DATA_END 0
        % #define TYPE_OBJECT 1
        % #define TYPE_ARRAY  2
        % #define TYPE_KEY    3
        % #define TYPE_STRING 4
        % #define TYPE_NUMBER 5
        % #define TYPE_NULL   6
        % #define TYPE_TRUE   7
        % #define TYPE_FALSE  8
        
       
        
        numeric_data
        strings
        
        d_extra_info = '-------------------------------'
        mex
        data_to_string_ratio
        toc_total_time
        toc_file_read
        toc_parse
        toc_post_process
        ns_per_char
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
            
            t0 = tic;
            result = jsmn_mex(file_path);
            obj.mex = result;
            
            obj.file_string = result.json_string;
            obj.types = result.types;
            obj.d1 = result.d1;
            obj.d2 = result.d2;
            obj.numeric_data = result.numeric_data;
            
            obj.data_to_string_ratio = length(result.d1)/length(result.json_string);
            
            obj.toc_total_time = toc(t0);
            obj.toc_file_read = result.elapsed_read_time;
            obj.toc_parse = obj.toc_total_time - result.elapsed_read_time;
            obj.ns_per_char = 1e9*obj.toc_parse/length(result.json_string);
            
            %TODO: Build in an explicit timer on the parsing ...
%             obj.toc_parse = toc(t0)-result.elapsed_read_time;
%             obj.toc_file_read = result.elapsed_read_time;
%             obj.toc_post_process = result.elapsed_pp_time;
%             
            
            
%             if in.raw_string == -1
%                 t0 = tic;
%                 uint8_data = read_to_uint8(file_path);
%                 obj.file_string = char(uint8_data);
%                 obj.toc_file_read = toc(t0);
%                 t1 = tic;
%                 if in.n_tokens
%                     result = jsmn_mex(uint8_data,0,in.n_tokens);
%                 elseif in.chars_per_token
%                     result = jsmn_mex(uint8_data,0,length(obj.file_string)/in.chars_per_token);
%                 else
%                     result = jsmn_mex(uint8_data);
%                 end
%                 obj.toc_parse = toc(t1);
%             else
%                 t0 = tic;
%                 obj.file_string = in.raw_string;
%                 obj.toc_file_read = toc(t0);
%                 t1 = tic;
%                 if in.n_tokens
%                     result = jsmn_mex(in.raw_string,0,in.n_tokens);
%                 elseif in.chars_per_token
%                     result = jsmn_mex(in.raw_string,length(obj.file_string)/in.chars_per_token);
%                 else
%                     result = jsmn_mex(in.raw_string);
%                 end
%                 obj.toc_parse = toc(t1);
%             end
            
%             obj.file_string = result.json_string;
%             obj.mex = result;
%             obj.data_to_string_ratio = length(result.data)/length(result.json_string);
%             obj.data = result.data;
%             obj.numeric_data = result.numeric_data;
%             obj.ns_per_char = 1e9*obj.toc_parse/length(result.json_string);
            
            %These would take some work to get, would need to make these
            %dependent ...
            %             obj.chars_per_token = length(obj.file_string)/length(obj.numeric_data);
            %             obj.ns_per_token    = 1e9*obj.d1/length(obj.numeric_data);
        end
        function root = getRootInfo(obj)
            switch obj.data(1)
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
        %This no longer works since I changed the output format
        %from the parser ...
        %
        %What might be nice is something that flattens "data" into
        %a structure
        
        function c = getDataAsStruct(obj)
%     object: 1) type  2) parent      3) tac     4) n_values
%     array:  1) type  2) parent      3) tac     4) n_values
%     key:    1) type  2) parent      3) tac     4) start/p     5) end
%     string: 1) type  2) start/p     3) end
%     number: 1) type  2) start/p     3) info
%     null:   1) type  2) <nothing>/p 3) <nothing>
%     t/f:    1) type    

            cur_I = 1;
            d = obj.data;
            n_tokens = 0;
            %We don't know how much to allocate here, this is a poor guess
            %that should avoid resizing
            c = cell(1,ceil(length(d)/2));
            while(1)
                s = struct;
                switch d(cur_I)
                    case 0
                        break;
                    case 1
                        s.type   = 'object';
                        s.parent = d(cur_I+1);
                        s.tac    = d(cur_I+2);
                        s.n_keys = d(cur_I+3);
                        cur_I    = cur_I + 4;
                    case 2
                        s.type   = 'array';
                        s.parent = d(cur_I+1);
                        s.tac    = d(cur_I+2);
                        s.n_token_values = d(cur_I+3);
                        cur_I    = cur_I + 4; 
                    case 3
                        s.type = 'key';
                        s.parent = d(cur_I+1);
                        s.tac = d(cur_I+2);
                        s.start = d(cur_I+3);
                        s.end = d(cur_I+4);
                        cur_I    = cur_I + 5; 
                    case 4
                        s.type = 'string';
                        s.start_p = d(cur_I+1);
                        s.end = d(cur_I+2);
                        cur_I    = cur_I + 3; 
                    case 5
                         s.type = 'number';
                         s.start_p = d(cur_I+1);
                         s.info = d(cur_I+2);
                         cur_I    = cur_I + 3; 
                    case 6
                         s.type = 'null';
                         s.p = d(cur_I+1);
                         cur_I    = cur_I + 3; 
                    case 7
                         s.type = 'true';
                         cur_I    = cur_I + 1; 
                    case 8
                         s.type = 'false';
                         cur_I    = cur_I + 1; 
                    otherwise
                        error('Unexpected type')
                end
                n_tokens = n_tokens + 1;
                c{n_tokens} = s;
            end
            c = c(1:n_tokens);
        end
        
%         function output = viewOldInfo(obj,indices)
%             output = [num2cell(indices);
%                 json.TYPES(obj.types(indices));
%                 num2cell(obj.starts(indices));
%                 num2cell(obj.ends(indices));
%                 num2cell(obj.sizes(indices));
%                 num2cell(obj.parents(indices));
%                 num2cell(obj.tokens_after_close(indices));
%                 num2cell(obj.numeric_data(indices));
%                 obj.strings(indices)];
%             output = [{'indices','type','start','end','size','parent','token_after_close','value','string'}' output];
%         end
    end
    
end

