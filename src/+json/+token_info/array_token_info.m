classdef array_token_info
    %
    %   Class:
    %   json.token_info.array_token_info
    
    properties
        type = 'array'
        name
        full_name
        index
        n_elements
        %         attribute_names
        %         attribute_indices
    end
    
    properties
        array_depth
    end
    
    methods
        function value = get.array_depth(obj)
            cur_index = obj.index;
            n_arrays = 1;
            local_types = obj.p.types;
            for i = 1:obj.n_elements
                cur_index = cur_index + 1;
                if local_types(cur_index) == 2
                    n_arrays = n_arrays + 1;
                else
                    break
                end
            end
            value = n_arrays;
        end
    end
    
    properties (Hidden)
        p
    end
    
    methods
        function obj = array_token_info(name,full_name,index,p)
            obj.name = name;
            obj.full_name = full_name;
            obj.index = index;
            obj.p = p;
            
%     object: 1) type  2) n_values        3) tac
%     array:  1) type  2) n_values        3) tac
%     key:    1) type  2) start_pointer   3) tac
%             
%     string: 1) type  2) start_pointer   3) end of string
%     number: 1) type  2) start_pointer
%     null:   1) type  2) start_pointer
%     tf      1) type
            
            obj.n_elements = p.d1(index);
        end        
        function output = getCellstr(obj)
        
           keyboard
            
           %TODO: Add on optional error check ...  
           start_index = obj.index + 1;
           end_index   = obj.index + obj.n_elements;
           output      = obj.p.strings(start_index:end_index); 
        end
        function output = get1dNumericArray(obj)
            
           lp = obj.p;
           start_index = obj.index + 1;
           end_index   = obj.index + obj.n_elements;
           start_numeric_index = lp.d1(start_index);
           end_numeric_index = lp.d1(end_index);
           if (end_index-start_index) ~= (end_numeric_index-start_numeric_index)
              error('Array is not purely numeric') 
           end
           
           output = lp.numeric_data(start_numeric_index:end_numeric_index);

        end
        function output = get2dNumericArray(obj)
            
            keyboard
           error('Not yet implemented') 
        end
        function output = getArrayOf1dNumericArrays(obj)
            
            keyboard
           output = cell(1,obj.n_elements);
           array_I = obj.index + 1;

           local_numeric_data = obj.p.numeric_data;
           local_tokens_after_close = obj.p.tokens_after_close;
           for iCell = 1:obj.n_elements
              end_I = local_tokens_after_close(array_I)-1;
              output{iCell} = local_numeric_data(array_I+1:end_I);
              array_I = local_tokens_after_close(array_I);
           end
%            keyboard 
%            error('Not yet implemented')  
        end
        function output = getObjectArray(obj)
            %
            %   Use this when the array holds all objects
            
%     object: 1) type  2) n_values        3) tac
%     array:  1) type  2) n_values        3) tac
%     key:    1) type  2) start_pointer   3) tac
%             
%     string: 1) type  2) start_pointer   3) end of string
%     number: 1) type  2) start_pointer
%     null:   1) type  2) start_pointer
%     tf      1) type
            
            
            local_p = obj.p;
            tokens_after_close = local_p.d2;
            I = obj.index+1;
            n_objects = obj.n_elements;
            temp_output = cell(1,n_objects);
            for iObject = 1:n_objects
                %TODO: Add check on type
                local_name = sprintf('(%d)',iObject);
                local_full_name = [obj.full_name local_name];
                temp_output{iObject} = json.token_info.object_token_info(local_name,local_full_name,I,local_p);
                I = tokens_after_close(I);
            end
            
            output = [temp_output{:}];
        end
    end
    
end
