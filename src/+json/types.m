function output = TYPES(indices)
%
%   json.types

output = {'Object','Array','String','Numeric','Logical','Key'};

if nargin
   output = output(indices); 
end

end