function output = getMexC(mex_struct,use_ms_time)
%X Retrieve mex c-struct from turtle_json as Matlab structure
%
%   output = json.utils.getMexC(mex_struct,*use_ms_time)
%
%   TODO: Describe why we are doing this ...
%
%   See Also
%   --------
%
%
%   TODO: rename tj_get_log_struct_as_mx
%
%   

output = tj_get_log_struct_as_mx(mex_struct.slog);

if nargin < 2
    use_ms_time = false;
end

if use_ms_time
    fn = fieldnames(output);
    mask = strncmp(fn,'time__',6);
    time_fn = fn(mask);
    for i = 1:length(time_fn)
        cur_name = time_fn{i};
        output.(cur_name) = 1000*output.(cur_name);
    end
end

end