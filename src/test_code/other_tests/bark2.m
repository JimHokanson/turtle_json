function jt = bark2()
file_path = 'C:\D\example_JSON\1.json';
N = 10;
d1_all = zeros(1,N);
t3 = tic;
for i = 1:N
    jt = json.tokens(file_path,'n_tokens',15000005,'n_keys',6000002,...
        'n_strings',1000001,'n_numbers',4000000);
	%jt = json.tokens(file_path,'chars_per_token',10);
    d1_all(i) = jt.parse_info.non_read_time;
    %sobj = jt.get_parsed_data();
end

fprintf('avg total, min parse, avg parse, sum 10 parses\n');
fprintf('%0.3g,     %0.3g,     %0.3g,     %0.3g\n',toc(t3)/N,min(d1_all),mean(d1_all),sum(d1_all))

end