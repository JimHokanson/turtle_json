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
        md_index
        n_elements
    end
    
    properties
%         array_depth
    end
    
    methods
%         function value = get.array_depth(obj)
%             value = obj.p.array_info
%             cur_index = obj.index;
%             n_arrays = 1;
%             local_types = obj.p.types;
%             for i = 1:obj.n_elements
%                 cur_index = cur_index + 1;
%                 if local_types(cur_index) == 2
%                     n_arrays = n_arrays + 1;
%                 else
%                     break
%                 end
%             end
%             value = n_arrays;
%         end
    end
    
    properties (Hidden)
        p
    end
    
    methods
        function obj = array_token_info(name,full_name,md_index,p)
            obj.name = name;
            obj.full_name = full_name;
            obj.md_index = md_index;
            obj.p = p;            
        end
        function output = getCellstr(obj)
            %
            %   output = getCellstr(obj)
            %   
            %   cell array of strings => {'as' 'df'}
                        
            output = json_info_to_data(3,obj.p,obj.md_index);
        end
        function output = get1dNumericArray(obj)
            %
            %   output = get1dNumericArray(obj)
            %   
            %   1d numeric array => [1,2,3,4,5]
            
            output = json_info_to_data(4,obj.p,obj.md_index,0);            
        end
        function output = get2dNumericArray(obj)
            %
            %   output = get2dNumericArray(obj)
            %
            %   2d numeric array => [1,2,3;
            %                        4,5,6];
            
            output = json_info_to_data(4,obj.p,obj.md_index,0);  
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
