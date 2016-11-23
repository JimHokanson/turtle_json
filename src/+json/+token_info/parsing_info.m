classdef parsing_info
    %
    %   Class:
    %   json.token_info.parsing_info
    
    properties
       ns_per_char
       dtime = '---------  elapsed time properties  ---------'
       %ordered by 
       total_parsing_time
       file_read_time
       non_read_time
       initial_parsing_time
       total_post_procesing_time
       string_parsing_time
       key_parsing_time
    end
    
    methods
        function obj = parsing_info(parent_obj, result, total_time)
            
            
            %Elapsed time
            %--------------------------------------------------------
            obj.total_parsing_time = total_time;
            obj.file_read_time = result.elapsed_read_time;
            obj.non_read_time = obj.total_parsing_time - obj.file_read_time;
            obj.initial_parsing_time = result.elapsed_parse_time;
            obj.total_post_procesing_time = result.elapsed_pp_time;
            obj.string_parsing_time = result.string_parsing_time;
            obj.key_parsing_time = result.key_parsing_time;
            
            obj.ns_per_char = 1e9*obj.non_read_time/length(result.json_string);

            
%             obj.data_to_string_ratio = length(result.d1)/length(result.json_string);
%             
%             
% 
%             obj.ns_per_char = 
%             obj.chars_per_token = length(obj.file_string)/length(obj.d1);
            
            %TODO: Provide estimate of memory consumption
            %types + 4*d1 + 4*d2 + 8*numeric_data
            %- also need string_p, key_p, numeric_p 
        end
    end
    
end

