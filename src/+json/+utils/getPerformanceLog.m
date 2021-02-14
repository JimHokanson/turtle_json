function output = getPerformanceLog(mex_struct,use_ms_time)
%X Retrieve mex c-struct from turtle_json as Matlab structure
%
%   output = json.utils.getPerformanceLog(mex_struct,*use_ms_time)
%
%   Note, this is only for debugging and is not optimized for running in a
%   loop when reading multiple files. Note, if used in a loop when
%   debugging that is OK as the timing information needed to assess
%   performance is contained in this structure.
%
%   See Also
%   --------
%
%
%   TODO: rename tj_get_log_struct_as_mx
%
%   Improvements
%   ------------
%   1) TODO: Add on fields to delineate each section ...

%This is the mex call ... (see private folder)
output = tj_get_log_struct_as_mx(mex_struct.slog);

if nargin < 2
    use_ms_time = false;
end

if output.qpc_freq ~= 0
	scale_factor = 1/(1e6*output.qpc_freq);
else
    scale_factor = 1;
end
    
if use_ms_time
	scale_factor = 1000*scale_factor;
end

%Scaling any time fields ...
if scale_factor ~= 1
    fn = fieldnames(output);
    mask = strncmp(fn,'time__',6);
    time_fn = fn(mask);
    for i = 1:length(time_fn)
        cur_name = time_fn{i};
        output.(cur_name) = scale_factor*output.(cur_name);
    end
end

end