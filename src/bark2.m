function jt = bark2()
file_path = 'C:\D\example_JSON\1.json';
N = 10;
d1_all = zeros(1,N);
t3 = tic;
for i = 1:N
    jt = json.tokens(file_path);
	%jt = json.tokens(file_path,'chars_per_token',10);
    d1_all(i) = jt.parse_info.non_read_time;
    %sobj = jt.get_parsed_data();
end
fprintf('%g, %g, %g\n',toc(t3)/N,min(d1_all),sum(d1_all))

end