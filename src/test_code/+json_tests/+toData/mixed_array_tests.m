function mixed_array_tests()
%
%   json_tests.toData.mixed_array_tests

%{
    for i = 1:10000
    json_tests.toData.mixed_array_tests
    end
%}

%Tried nan, but this gets written as NaN by mexEncodeJSON
data = {'superstar' true false 15.3 ...
    1:10 {'asdf' 'anna' 'cbs'} [true false true false], ... %1d
    rand(5,2),{{'8 years','exactly'},{'Debbie','politics'}},false(10,2) ... %2d
    rand(3,4,2),{{{'young','cheese'},{'shape','need'}},{{'40','3'},{'20','mom'}}},false(4,5,2) ... %d
    }; 

expected_types = [-1 -1 -1 -1 ...
    0 1 2 ... %1d
    0 1 2 ... %2d
    0 1 2];

for iData = 1:length(data)
    %disp(iData);
    data2 = data(1:iData);
    js = tjSave(data2);
    %s = json.stringToTokens(js);
    temp = json.parse(js);
end


end