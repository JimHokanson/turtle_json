
%scratch page

file_path = json.utils.examples.getFilePath('1.json');
file_path = json.utils.examples.getFilePath('big.json');

tic
n = 10;
for i = 1:n
data = json.load(file_path);
end
toc/n

f = json.tokens.load(file_path);
s = f.getLogStruct()
data = f.getParsedData()
