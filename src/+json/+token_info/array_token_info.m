classdef array_token_info
    %
    %   Class:
    %   json.token_info.array_token_info
    
    %{
        s = '[[1,2,3],[4,5,6]]'
        s2 = json.stringToTokens(s)
        r = s2.getRootInfo
        d = r.get2dNumericArray
    %}
    
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
            
            %TODO: Add on optional error check ...
            lp = obj.p;
            n_values = lp.d1;
            local_index = obj.index;
            if n_values(local_index)
                string_pointer = lp.d1;
                tac = lp.d2;
                start_index = string_pointer(local_index+1);
                end_index = string_pointer(tac(local_index)-1);

                output      = obj.p.strings(start_index:end_index);
            else
                output = {};
            end
        end
        function output = get1dNumericArray(obj)
            
            lp = obj.p;
            n_values = lp.d1;
            local_index = obj.index;
            if (n_values(local_index))
                tac = lp.d2;
                numeric_pointer = lp.d1;
                start_numeric_I = numeric_pointer(local_index+1);
                end_numeric_I = numeric_pointer(tac(local_index)-1);
                output = lp.numeric_data(start_numeric_I:end_numeric_I);
            else
                output = [];
            end
            
        end
        function output = get2dNumericArray(obj)
            
            %            lp = obj.p;
            %            local_index = obj.index;
            %            start_index = local_index + 2;
            %            end_index   = lp.d2(local_index)-1;
            %            start_numeric_index = lp.d1(start_index);
            %            end_numeric_index = lp.d1(end_index);
            %            d1_size = lp.d1(local_index+1);
            %            d2_size = (end_numeric_index-start_numeric_index+1)/d1_size)
            
            lp = obj.p;
            local_index = obj.index;
            
            next_sibling_element = lp.token_after_close;
            child_count = lp.child_count;
            
            value_indices = lp.value_index;
            
            
            %Retrieval of indices
            %--------------------------------------------------------------
            % [    [     #
            % 0    +1   +2
            first_data_value_index = value_indices(local_index + 2);
            
            %note, array closings don't take up space, so the final #
            %is positioned at 1 less than next sibling element 
            %
            %This could fail if the 2d array assumption is invalid
            %since we haven't checked the type of n_s_e(local_index)-1
            %so value_indices of this value might be 0
            last_data_value_index  = value_indices(next_sibling_element(local_index)-1);
            
            %Basic verification for now ...
            %--------------------------------------------------------------
            %This could still be wrong if the arrays are unevenly shaped
            %but somehow counterbalance each other
            %e.g. 3 1d arrays of sizes 2,1,3
            %   {[1,2],[3],[4,5,6]} => interpreted as [1,2;3,4;5,6]
            %
            %A more proper error check would go through the subarrays
            %and verify that they are all of arrays and that their sizes
            %are consistent
            
            n_1d_arrays = child_count(local_index);
            
            %This could be different for each child, we'll just
            %check that 2d is possible given the first entry
            n_elements_per_1d_array = child_count(local_index+1);
            
            if (last_data_value_index - first_data_value_index + 1) ... 
                    == n_1d_arrays*n_elements_per_1d_array
               output = reshape(lp.numeric_data(first_data_value_index:last_data_value_index),[n_elements_per_1d_array n_1d_arrays]);
            else
                %TODO: Show numbers that failed ...
               error('Current array object is not a proper 2d array') 
            end

        end
        function output = getArrayOf1dNumericArrays(obj)
            %
            %   
            %
            
            output = cell(1,obj.n_elements);
            
            
            %TODO: Change names, don't use d1 or d2
            
            lp = obj.p;
            tac = lp.d2;
            n_values = lp.d1;
            numeric_pointer = lp.d1;
            
            numeric_data = lp.numeric_data;
            
            cur_array_index = obj.index + 1;
            
            local_n = obj.n_elements;
            
            for iCell = 1:local_n
                if n_values(cur_array_index)
                    start_numeric_I = numeric_pointer(cur_array_index+1);
                    cur_array_index = tac(cur_array_index);
                    end_numeric_I = numeric_pointer(cur_array_index-1); %Get last value before the start of the next array
                    output{iCell} = numeric_data(start_numeric_I:end_numeric_I);
                else
                    cur_array_index = cur_array_index + 1;
                end
            end
            
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
