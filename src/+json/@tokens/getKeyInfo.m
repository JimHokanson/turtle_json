function [local_key_names,local_key_indices] = getKeyInfo(p,object_index)
%Keys are a bit messy, I was hoping to be able to do something
%nice with them rather than allocating a new string for each
%object. In the mean time I'm going to keep this clunky
%function


%I moved this to a different file to avoid the, are you sure you didn't
%want to reference a property warning, since unfortunately you can't
%disable warnings on a function level

d2 = p.d2;

%We might change this approach later on ...

key_data = p.key_data;
key_starts = p.key_starts;
key_ends = p.key_ends;

d1 = p.d1;
n_attributes = p.d1(object_index);
key_start_I  = object_index + 1;

local_key_indices = zeros(1,n_attributes,'int32');
local_key_names = cell(1,n_attributes);

for iItem = 1:n_attributes
    
    local_key_indices(iItem) = key_start_I;
    
    cur_I = d1(key_start_I);
    local_key_names{iItem} = key_data(key_starts(cur_I):key_ends(cur_I));
    key_start_I = d2(key_start_I);
end

end