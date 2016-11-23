function [local_key_names,local_key_indices] = getKeyInfo(p,object_index)
%
%   Returns the name and token indices of the keys of an object
%   
%

%Keys are a bit messy, I was hoping to be able to do something
%nice with them rather than allocating a new string for each
%object. In the mean time I'm going to keep this clunky
%function


%I moved this to a different file to avoid the, "are you sure you didn't
%want to reference a property" warning, since unfortunately you can't
%disable warnings on a function level


n_keys = p.child_count(object_index);
local_key_indices = zeros(1,n_keys,'int32');
local_key_names = cell(1,n_keys);
value_indices = p.value_index;
next_key_indices = p.token_after_close;
all_key_names = p.keys;

key_start_I  = object_index + 1;

for iKey = 1:n_keys
    local_key_indices(iKey) = key_start_I;
    local_key_names{iKey} = all_key_names{value_indices(key_start_I)};
    key_start_I = next_key_indices(key_start_I);
end

end