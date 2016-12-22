function numeric_array_tests()

%Order?????
%http://stackoverflow.com/questions/40833647/proper-translation-of-multi-dimensional-array-to-json-and-back/

data = reshape(1:24,[4,3,2]);
js = mexEncodeJSON(data);

data2 = zeros(4,1,2);
data3 = {data data2};
js = mexEncodeJSON(data3);


data = 1:10;
js = mexEncodeJSON(data);

%JSON writers
%------------

%1)
%https://github.com/christianpanton/matlab-json
%- only supports 2d
%- writes array by memory order ...
%- s = tojson(data)

%2) 
%Matlab's JSON implementation
%- js = mexEncodeJSON(data)
%- writes so that innermost is highest dimension

end