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
        function obj = array_token_info(name,full_name,index,parse_object)
            obj.name = name;
            obj.full_name = full_name;
            obj.index = index;
            obj.p = parse_object;
            
            obj.n_elements = parse_object.sizes(index);
        end
        % % % % %         function output = getIndex(obj,index)
        % % % % %
        % % % % %            p = obj.p;
        % % % % %            I = obj.index+1;
        % % % % %            %TODO: Make this a lazy attribute
        % % % % %            tokens_after_close = p.tokens_after_close;
        % % % % %            for iIndex = 2:index
        % % % % %               I = tokens_after_close(I);
        % % % % %            end
        % % % % %
        % % % % %            name = sprintf('(%d)',index);
        % % % % %            full_name = [obj.full_name name];
        % % % % %
        % % % % %            switch obj.p.types(1)
        % % % % %                case 1
        % % % % %                    output = json.token_info.object_token_info(name,full_name,I,p);
        % % % % %                case 2
        % % % % %                    error('NYI');
        % % % % %                case 3
        % % % % %                    error('NYI');
        % % % % %                case 4
        % % % % %                    error('NYI');
        % % % % %                otherwise
        % % % % %                    error('Not yet supported')
        % % % % %            end
        % % % % %         end
        function output = get1dNumericArray(obj)
           %TODO: Add on optional error check ... 
           start_index = obj.index + 1;
           end_index   = obj.index + obj.n_elements;
           output      = obj.p.numeric_data(start_index:end_index);
        end
        function get2dNumericArray(obj)
           error('Not yet implemented') 
        end
        function getArrayOf1dNumericArrays(obj)
           error('Not yet implemented')  
        end
        function output = getObjectArray(obj)
            %
            %   Use this when the array holds all objects
            %
            local_p = obj.p;
            I = obj.index+1;
            tokens_after_close = local_p.tokens_after_close;
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
% % % % %         function output = getNumericArray(obj,dims)
% % % % %             
% % % % %             if nargin == 0
% % % % %                 dims = 1;
% % % % %             end
% % % % %             
% % % % %             if dims == 1
% % % % %                 keyboard
% % % % %             end
% % % % %             output = [];
% % % % %         end
    end
    
end
