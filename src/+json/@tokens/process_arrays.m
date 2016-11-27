function process_arrays(obj)



m = obj.mex;
types = m.types;
data_indices = obj.data_indices;
child_counts = obj.oa_child_counts;
key_ns_index = m.next_sibling_index_key;
oa_ns_index  = m.next_sibling_index_oa;

n_oa = length(m.child_count);


cur_stack_processing_index = 1;
stack_max_index = 1;
%TODO: Maybe include a min here ...
stack = zeros(1,ceil(n_oa/1000));
stack(1) = 1;
stack_size = length(stack);
done = false;
while ~done
   cur_data_index = stack(cur_stack_processing_index);
    
    if types(cur_data_index) == 1
       %Go through each element and add it to the stack
       obj_data_index = data_indices(cur_data_index);
       n_children = child_counts(obj_data_index);
       cur_data_index = cur_data_index + 1;
       for iChildren = 1:n_children
          cur_key_index = data_indices(cur_data_index);
          %+1 to get value of key
          if types(cur_data_index+1) <= 2
             stack_max_index = stack_max_index + 1;
             if stack_max_index > stack_size
                 %TODO: Resize the stack
             end
             stack(stack_max_index) = cur_data_index+1;
          end
          cur_data_index = key_ns_index(cur_key_index);
       end
    else
        keyboard
    end
    
    cur_stack_processing_index = cur_stack_processing_index + 1;
    done = cur_stack_processing_index > stack_max_index;
end

end