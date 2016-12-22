function logical_array_tests()

data = 1:10 > 3;
js = mexEncodeJSON(data);

wtf = json.parse(js);

end