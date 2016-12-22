function string_array_tests()


data = {'hi' 'all'; 'this' 'test'};
js = mexEncodeJSON(data);
%Fail: ["hi","this","all","test"]

data = {{'hi' 'all'},{'this' 'test'}};
js = mexEncodeJSON(data);
%Good: [["hi","all"],["this","test"]]



end