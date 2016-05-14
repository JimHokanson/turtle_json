function string_speed_testing

strings = repmat({'a' 'b' 'c'},1,1e5);

uint8_data = cellfun(@uint8,strings,'un',0);

index = zeros(1,length(strings));

m = containers.Map('KeyType', 'char', 'ValueType', 'int32');
cur_count = 0;
tic
for i = 1:length(strings)
   cur_key = strings{i};
   try
   index(i) = m(cur_key);
   catch
      cur_count = cur_count+1;
      m(cur_key) = cur_count;
   end
end
toc

s = struct;

tic
for i = 1:length(strings)
   cur_key = strings{i};
   try
   index(i) = s.(cur_key);
   catch
      cur_count = cur_count+1;
      s.(cur_key) = cur_count;
   end
end
toc
disp('arg')

tic
for i = 1:length(strings)
   cur_key = strings{i};
   if isfield(s,cur_key)
   	  index(i) = s.(cur_key);
   else
      cur_count = cur_count+1;
      s.(cur_key) = cur_count;
   end
end
toc



tic; wtf = char(uint8(43*ones(1,1e7))); toc;

disp('uint8')
N = 1e6;
s_length = randi(20,1,N);
output = cell(1,N);
tic
for iN = 1:N
   output{iN} = ones(1,s_length(iN),'uint8'); 
end
toc

disp('to char')
tic
for iN = 1:N
   output{iN} = char(output{iN}); 
end
toc

disp('uint16')
tic
for iN = 1:N
   output{iN} = ones(1,s_length(iN),'uint16'); 
end
toc

disp('to string')
tic
% for iN = 1:N
   output = cellfun(@char,output,'un',0);
% end
toc

% disp('to string')
% tic
% for iN = 1:N
%    output{iN} = char(output{iN}); 
% end
% toc

disp('double')
tic
for iN = 1:N
   output{iN} = ones(1,s_length(iN),'double'); 
end
toc

disp('double without 3rd input')
tic
for iN = 1:N
   output{iN} = ones(1,s_length(iN)); 
end
toc

N = 1e6;
tic; wtf = repmat({'abcdefghijk'},1,N); toc;
tic
for iN = 1:N
   wtf{iN}(end) = 'j'; 
end
toc



% tic; wtf = cellstr(char(ones(1e6,5))); toc;


end