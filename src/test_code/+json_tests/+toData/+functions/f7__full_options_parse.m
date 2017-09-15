function f7__full_options_parse()

%Invalid option testing
%-----------------------------------------------------------------------
%Numeric not boolean ...
JSON_STR = '[[1,2,3],[4,5,6]]';
try
data = json.parse(JSON_STR,'max_numeric_collapse_depth',true);
%Throw error here
catch ME
    %Throw error if not correct
end

%Column Majors
%-----------------------------------------------------------------------
JSON_STR = '[1,2,3]';
data = json.parse(JSON_STR,'column_major',false);
if ~isequal(data,1:3)
    error('Failed to properly parse 1d-array using row-major order');
end

JSON_STR = '[1,2,3]';
data = json.parse(JSON_STR,'column_major',true);
if ~isequal(data,(1:3)')
    error('Failed to properly parse 1d-array using column-major order');
end

JSON_STR = '[[1,2,3],[4,5,6]]';
data = json.parse(JSON_STR,'column_major',false);
if ~isequal(data,[1,2,3;4,5,6])
    error('Failed to properly parse nd-array using row-major order');
end

JSON_STR = '[[1,2,3],[4,5,6]]';
data = json.parse(JSON_STR,'column_major',true);
if ~isequal(data,[1,4;2,5;3,6])
    error('Failed to properly parse nd-array using column-major order');
end

%Max numeric collapse depth
%------------------------------------------------------------------------
%NOT YET IMPLEMENTED
% JSON_STR = '[[1,2,3],[4,5,6]]';
% data = json.parse(JSON_STR,'max_numeric_collapse_depth',0);
% d2 = {[1;2;3],[4;5;6]};
% if ~isequal(data,d2)
%     error('Failed to properly parse nd-array using max_numeric_collapse_depth = 1');
% end

JSON_STR = '[[1,2,3],[4,5,6]]';
data = json.parse(JSON_STR,'max_numeric_collapse_depth',1);
d2 = {[1;2;3],[4;5;6]};
if ~isequal(data,d2)
    error('Failed to properly parse nd-array using max_numeric_collapse_depth = 1');
end

JSON_STR = '[[1,2,3],[4,5,6]]';
data = json.parse(JSON_STR,'max_numeric_collapse_depth',2);
d2 = [1,4;2,5;3,6];
if ~isequal(data,d2)
    error('Failed to properly parse nd-array using max_numeric_collapse_depth = 2');
end



%   max_numeric_collapse_depth: default -1
%
%   max_string_collape_depth : default -1
%
%   max_bool_collapse_depth : default -1
%
%
%   collapse_objects : default true






end