classdef array_token_info
    %
    %   Class:
    %   json.token_info.array_token_info
    
    %{
        expected_data = [1,4; 2,5; 3,6];
        s = '[[1,2,3],[4,5,6]]';
        s2 = json.stringToTokens(s);
        r = s2.getRootInfo;
        d = r.get2dNumericArray;
        isequal(d,expected_data)
    %}
    
    properties
        type = 'array'
        name
        full_name
        index
        n_elements
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
            obj.n_elements = p.child_count(index);
        end
        function output = getCellstr(obj)
            %
            %   output = getCellstr(obj)
            %   
            %   cell array of strings => {'as' 'df'}
            
            %TODO: Add on optional error check ...
            lp = obj.p;
            n_values = lp.child_count;
            local_index = obj.index;
            if n_values(local_index)
                value_indices = lp.value_index;
                next_sibling_index = lp.token_after_close;
                
                start_index = value_indices(local_index+1);
                end_index = value_indices(next_sibling_index(local_index)-1);

                output = lp.strings(start_index:end_index);
            else
                output = {};
            end
        end
        function output = get1dNumericArray(obj)
            %
            %   output = get1dNumericArray(obj)
            %   
            %   1d numeric array => [1,2,3,4,5]
            
            lp = obj.p;
            n_values = lp.child_count;
            local_index = obj.index;
            if (n_values(local_index))
                value_indices = lp.value_index;
                next_sibling_index = lp.token_after_close;
                
                start_index = value_indices(local_index+1);
                end_index = value_indices(next_sibling_index(local_index)-1);
                
                output = lp.numeric_data(start_index:end_index);
            else
                output = [];
            end
            
        end
        function output = get2dNumericArray(obj)
            %
            %   output = get2dNumericArray(obj)
            %
            %   2d numeric array => [1,2,3;
            %                        4,5,6];
            
            lp = obj.p;
            local_index = obj.index;
            
            next_sibling_index = lp.token_after_close;
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
            last_data_value_index  = value_indices(next_sibling_index(local_index)-1);
            
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
            %   output = getArrayOf1dNumericArrays(obj)       
            %   
            %   array of 1d numeric arrays => {[1,2,3],[4,5],[6]}
            
            lp = obj.p;
            next_sibling_index = lp.token_after_close;
            child_count = lp.child_count;
            value_indices = lp.value_index;
            numeric_data = lp.numeric_data;
            local_n = obj.n_elements;

            output = cell(1,obj.n_elements);
            
            cur_array_index = obj.index + 1;
            for iCell = 1:local_n
                if child_count(cur_array_index)
                    data_start_I = value_indices(cur_array_index+1);
                    cur_array_index = next_sibling_index(cur_array_index);
                    %Get last value before the start of the next array
                    data_end_I = value_indices(cur_array_index-1); 
                    output{iCell} = numeric_data(data_start_I:data_end_I);
                else
                    cur_array_index = cur_array_index + 1;
                end
            end         
        end
        function output = getObjectArray(obj)
            %
            %   Use this when the array holds all objects
            
            lp = obj.p;
            next_sibling_index = lp.d2;
            I = obj.index+1;
            n_objects = obj.n_elements;
            temp_output = cell(1,n_objects);
            for iObject = 1:n_objects
                %TODO: Add check on type
                local_name = sprintf('(%d)',iObject);
                local_full_name = [obj.full_name local_name];
                temp_output{iObject} = json.token_info.object_token_info(local_name,local_full_name,I,lp);
                I = next_sibling_index(I);
            end
            
            output = [temp_output{:}];
        end
    end
    
end
