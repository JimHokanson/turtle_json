
%scratch page

%Get files at:
%https://drive.google.com/drive/folders/0B7to9gBdZEyGMExwTFA0ZWh1OTA
%
%place in <repo root>/examples

file_path = json.utils.examples.getFilePath('1.json');
file_path = json.utils.examples.getFilePath('big.json');
file_path = json.utils.examples.getFilePath('XJ30_NaCl500mM4uL6h_10m45x10s40s_Ea.json');




tic
n = 10;
for i = 1:n
data = json.load(file_path);
end
toc/n

f = json.tokens.load(file_path);
s = f.getLogStruct()
data = f.getParsedData()
