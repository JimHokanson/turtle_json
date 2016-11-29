function numeric_array_tests()

%Order?????
%http://stackoverflow.com/questions/40833647/proper-translation-of-multi-dimensional-array-to-json-and-back/

data = reshape(1:24,[4,3,2]);

%JSON writers
%------------

%1)
%https://github.com/christianpanton/matlab-json
%- only supports 2d
%- writes array by memory order ...
%- s = tojson(data)

%2) 
%Matlab's JSON implementation
%- s = mexEncodeJSON(data)
%- writes so that innermost is highest dimension

end