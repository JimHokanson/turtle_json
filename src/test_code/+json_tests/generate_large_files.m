function generate_large_files()

a = 1:1e7;

b = mat2str(a);

a = rand(1,1e6);
b = a./rand(1,1e6);

tic;
for i = 1:1e6
   sprintf('%0.10f',a(1));
   wtf = sprintfc('%0.10f', [3.144 234234.234234], false);
end
toc;

tic
   wtf = arrayfun(@(x) sprintf('%0.10f',x),a,'un',0);
toc

tic
   wtf = sprintfc('%0.10f', b);
toc

tic
   wtf = mat2str(b,10);
toc