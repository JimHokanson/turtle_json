classdef tokens
    %
    %   Class:
    %   json.tokens
    %
    %   See Also:
    %   ---------
    %   json.fileToTokens
    %   json.stringToTokens
    
    properties
        file_string %string of the file
        %We might not hold onto this ...
        
        TYPE_DEFS = {'object','array','key','string','number','null','true','false'};
        
        types
        d1
        d2
        
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
        
        
        numeric_data
        key_data
        key_starts
        key_ends
        string_data
        string_starts
        string_ends
        strings
        
        mex
        
        d_extra_info = '-------------------------------'
        data_to_string_ratio
        toc_total_time
        toc_non_read_time
        toc_file_read
        toc_parse
        toc_post_process
        ns_per_char
        
        %This is approximate because we double the tokens for keys
        chars_per_token
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
            
            
            
            
            
            in.chars_per_token = sl.in.NULL;
            in.n_tokens = sl.in.NULL;
            in.raw_string = sl.in.NULL;
            in.raw_is_padded = false;
            in = sl.in.processVarargin(in,varargin,'remove_null',true);
            
            t0 = tic;
            result = jsmn_mex(file_path);
            
            obj.mex = result;
            
            obj.file_string = result.json_string;
            
            obj.types = result.types;
            obj.d1 = result.d1;
            obj.d2 = result.d2;
            obj.numeric_data = result.numeric_p;
            %obj.numeric_data = result.numeric_data;
            
            obj.key_data = result.key_data;
            obj.key_starts = result.key_start_indices;
            obj.key_ends = result.key_end_indices;
            
            obj.string_data = result.string_data;
            obj.string_starts = result.string_start_indices;
            obj.string_ends = result.string_end_indices;
            
            local_string_data = result.string_data;
            local_string_starts = result.string_start_indices;
            local_string_ends = result.string_end_indices;
            n_strings = length(result.string_end_indices);
            temp_strings = cell(1,n_strings);
            for iString = 1:n_strings
                temp_strings{iString} = local_string_data(local_string_starts(iString):local_string_ends(iString));
            end
            
            obj.strings = temp_strings;
            
            
            
            
            obj.data_to_string_ratio = length(result.d1)/length(result.json_string);
            obj.toc_total_time = toc(t0);
            
            obj.toc_file_read = result.elapsed_read_time;
            obj.toc_non_read_time = obj.toc_total_time - obj.toc_file_read;
            obj.toc_parse = result.elapsed_parse_time;
            obj.toc_post_process = result.elapsed_pp_time;
            obj.ns_per_char = 1e9*obj.toc_parse/length(result.json_string);
            obj.chars_per_token = length(obj.file_string)/length(obj.d1);
            
            %TODO: Provide estimate of memory consumption
            %types + 4*d1 + 4*d2 + 8*numeric_data
            %- also need string_p, key_p, numeric_p
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
        
        %TODO: This needs to be redone
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

