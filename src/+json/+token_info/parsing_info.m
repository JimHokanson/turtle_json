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
       d_pp = '---------- post-processing time  --------'
       total_post_procesing_time
       number_parsing_time
       string_memory_allocation_time
       string_parsing_time
       key_parsing_time
       
       d_content = '---------- content  --------'
       n_key_chars
       n_string_chars
       chars_per_token
       
       
       d_allocation = '---------- allocation info  --------'
       n_tokens_allocated
       n_tokens
       token_allocation_efficiency
       
       n_keys_allocated
       n_keys
       key_allocation_efficiency
       
       n_strings_allocated
       n_strings
       string_allocation_efficiency
       
       n_numbers_allocated
       n_numbers
       number_allocation_efficiency
       
       memory_allocation_estimate_mb
       memory_used_estimate_mb
       memory_allocated_to_file_size_ratio
       memory_used_to_file_size_ratio
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
            obj.string_memory_allocation_time = result.string_memory_allocation_time;
            obj.string_parsing_time = result.string_parsing_time;
            obj.number_parsing_time = result.number_parsing_time;
            
            %obj.key_parsing_time = result.key_parsing_time;
            
            obj.ns_per_char = 1e9*obj.non_read_time/length(result.json_string);

            
            %TODO: Reimplement memory estimates
            
            obj.n_key_chars = result.n_key_chars;
            obj.n_string_chars = result.n_string_chars;
            obj.chars_per_token = length(result.json_string)/length(result.d1);
            
            obj.n_tokens_allocated = double(result.n_tokens_allocated);
            obj.n_tokens = length(result.d1);
            obj.token_allocation_efficiency = obj.n_tokens/double(obj.n_tokens_allocated);
            
            obj.n_keys_allocated = double(result.n_key_allocations);
            obj.n_keys = length(result.key_p);
            obj.key_allocation_efficiency = obj.n_keys/obj.n_keys_allocated;
            
            obj.n_strings_allocated = double(result.n_string_allocations);
            obj.n_strings = length(result.string_p);
            obj.string_allocation_efficiency = obj.n_strings/obj.n_strings_allocated;
            
        	obj.n_numbers_allocated = double(result.n_numeric_allocations);
            obj.n_numbers = length(result.numeric_p);
            obj.number_allocation_efficiency = obj.n_numbers/obj.n_numbers_allocated;
            
            
            obj.memory_allocation_estimate_mb = ((1+4+4)*obj.n_tokens_allocated + ...
                (8 + 4)*obj.n_keys_allocated + ...
                (8 + 4)*obj.n_strings_allocated + ...
                (8)*obj.n_numbers_allocated)/1e6;
            
            obj.memory_used_estimate_mb = ((1+4+4)*obj.n_tokens + ...
                (8 + 4)*obj.n_keys + ...
                (8 + 4)*obj.n_strings + ...
                (8)*obj.n_numbers)/1e6;
            
            obj.memory_allocated_to_file_size_ratio = obj.memory_allocation_estimate_mb/(length(result.json_string)/1e6);
            obj.memory_used_to_file_size_ratio = obj.memory_used_estimate_mb/(length(result.json_string)/1e6);
            
            %TODO: Provide estimate of memory consumption
            %types + 4*d1 + 4*d2 + 8*numeric_data
            %- also need string_p, key_p, numeric_p 
        end
    end
    
end

