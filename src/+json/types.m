function output = TYPES(indices)
%
%   json.types

output = {'Object','Array','String','Numeric','Logical'};

if nargin
   output = output(indices); 
end

end