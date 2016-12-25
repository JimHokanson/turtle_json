function logical_array_tests()

%1d array testing
data = 1:10 > 3;
js = mexEncodeJSON(data);

wtf = json.parse(js);

%nd-array testing
data = reshape(data,5,2);
js = mexEncodeJSON(data);
wtf = json.parse(js);




end