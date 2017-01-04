classdef time_example_file
    %
    %   Class:
    %   json_tests.time_example_file
    %
    %   This class may move to a 'support' folder
    
    properties
       average_elapsed_time
       method
       file_name
    end
    
    methods (Static)
        function all_results = testAll()
            %
            %   all_results = json_tests.time_example_file.testAll
            %   
            %   This might be better in a file that clearly indicates that
            %   it is a test
            
            file_names = json.utils.examples.FILE_LIST;
            n_files = length(file_names);
            all_results = cell(1,n_files);
            for iFile = 1:n_files
                fprintf('Running example %d of %d: %s\n',iFile,n_files,file_names{iFile});
                all_results{iFile} = json_tests.time_example_file(file_names{iFile});
            end
            all_results = [all_results{:}];
        end
    end
    
    methods
        function obj = time_example_file(name_or_index,varargin)
            %
            %   result = json_tests.time_example_file(name_or_index)
            %
            %   result = json_tests.time_example_file('big.json','n_runs',1)
            %
            %   Optional Inputs
            %   ---------------
            %   n_runs: scalar
            %       # of times to run the code
            %
            
            in.n_runs = 10;
            in.method = 1;
            in = json.sl.in.processVarargin(in,varargin);
            
            file_path = json.utils.examples.getFilePath(name_or_index);
            
            [~,obj.file_name] = fileparts(file_path);
            
            n_runs = in.n_runs;
            if in.method == 1
                t0 = tic;
                for iRun = 1:n_runs
                    token = json.tokens.load(file_path);
                end
                t1 = toc(t0)/n_runs;
                
                t2 = tic;
                for iRun = 1:n_runs
                    data = token.getParsedData();
                end
                t3 = toc(t2)/n_runs;
            elseif in.method == 2
                t0 = tic;
                if ismac
                    error('Not yet implemented')
                else
                    for iRun = 1:n_runs
                        temp = fileread(file_path);
                        data = mexDecodeJSON(temp, @makeArray, @makeStructure);
                    end
                end
                
                t3 = toc(t0)/n_runs;
                
                
            end
            
            obj.method = in.method;
            obj.average_elapsed_time = t3;

        end
    end
    
end

function data = makeStructure(names, values, areNamesUnique, areNamesValid)
% Create a structure from names and values cell arrays. Ensure the names
% are unique.

if ~(areNamesUnique && areNamesValid)
    names = matlab.lang.makeUniqueStrings(names);
    names = matlab.lang.makeValidName(names);
end
data = cell2struct(values, names, 1);
if isempty(data)
    data = struct;
end
end

%-------------------------------------------------------------------------%

function arr = makeArray(data, depth)
% Create an array from data cell array.

arr = reshape(data, [ones(1, depth - 1) numel(data) 1]);
try
    % do not try to convert a cell array to a matrix if it
    % contains characters
    if ~isempty(arr) && iscell(arr) && ~iscellstr(arr)
        arr = cell2mat(arr);
    elseif isempty(arr)
        arr = [];
    end
catch %#ok<CTCH>
end
if iscell(arr) && depth > 1
    % Remove singleton dimensions in each element of data which
    % can result from the earlier reshape.
    arr = cellfun(@squeeze, arr, 'UniformOutput', false);
end
end
