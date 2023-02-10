function mex_turtle_json(file_id,varargin)
%x Code to compile turtle_json
%
%   mex_turtle_json(*file_id,varargin)
%
%   This code has been designed with OpenMP and pointers to labels. I've
%   used GCC for both mac and windows.
%
%   Inputs
%   ------
%   file_id : default compile all
%       1 - turtle_json_mex.c
%       2 - json_info_to_data.c
%
%   Optional Inputs
%   ---------------
%   verbose : default false
%       If true compile commands will be shown 
%   allow_ref_count : default false
%       If true, allocations can be made by simply increasing the reference
%       count. If not, all allocations that are made that could be done
%       with just reference counts are done via deep copies.
%
%   Implementation Notes
%   --------------------
%   Compilation relies on:
%       https://github.com/JimHokanson/mex_maker
%
%   The C code relies on GCC.
%
%       mac : used homebrew to install GCC
%   windows : used mingw
%     linux : not tested, something like the following should work.
%
%       email me if you want Linux and can't figure it out ...
%
%       mex CFLAGS="$CFLAGS -std=c11 -mavx2 -fopenmp" LDFLAGS="$LDFLAGS -fopenmp" turtle_json_mex.c turtle_json_main.c
%
%   Issues
%   ------------------------------------------------------------------
%   Windows: -----------------------
%   Problem: .o file, no such file or directory
%       GCC relies on libwinpthread-1.dll. I'm not sure how gcc has this
%       included when called, but if you are getting the above error
%       you may try including the bin folder on your Windows Path.
%       - path is something like:
%       C:\Program Files\mingw-w64\x86_64...\mingw64\bin\
%
%   
%
%
%   Examples
%   --------
%   mex_turtle_json()
%   mex_turtle_json([],'allow_ref_count',true)

%{

- copied terminal app to desktop
- get info - open with Rosetta
- /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

THIS DOESN'T WORK, RUNNING ZSH
SEE VIA: echo $SHELL
- cd ~/
- touch .bash_profile
- open -e .bash_profile

open ~/.zshrc

ADD THESE LINES:
alias ibrew='arch -x86_64 /usr/local/bin/brew'
alias mbrew='arch -arm64e /opt/homebrew/bin/brew'

M1 mac targeting arch x86_64
- need brew to be for Rosetta 2

Install brew for Rosetta 2:
https://stackoverflow.com/a/64883440/764365

How to update .bash_profile
https://stackoverflow.com/a/8967864/764365

Setup aliases:
https://stackoverflow.com/a/65424574/764365


%}



%{
%After compiling testing:
data = json.utils.examples.speedDataTest('1.json',1);
data = json.utils.examples.speedDataTest('big.json',10);
data = json.utils.examples.speedDataTest('XJ30',10);

%----------------------------------
s = struct('id',num2cell(1:1e6),'flag',true,'fflag',false,'null',NaN);
tic
json_str = jsonencode(s);
toc

tic
f = json.tokens.parse(json_str);
toc
s2 = f.getLogStruct();
tic
data = f.getParsedData();
toc
%data verification ...
isequalwithequalnans(s,data)

%----------------------------------
file_path = json.utils.examples.getFilePath('XJ3');
f = json.tokens.load(file_path);
s = f.getLogStruct();
data = f.getParsedData();
%}


%   OLD
%   log_timing : default true
%       Whether to log timing. I made some improvements so this should
%       really just be left true.
%   log_alloc : default true
%       Whether to log allocations made. I made some improvements so
%       this should really just be left true.


p = inputParser;
addOptional(p,'verbose',false);
addOptional(p,'log_timing',true);
addOptional(p,'log_alloc',true);
addOptional(p,'allow_ref_count',false);
parse(p,varargin{:});

in = p.Results;

if nargin == 0
    file_id = [];
end



%TODO: Ideally I would include the final code so that someone
%   could recompile by modifying the long form of the code
%   rather than using my experimental compile code

%Compiling of turtle_json_mex.c and associated files
%-------------------------------------------------------
if isempty(file_id) || file_id == 1
    fprintf('Compiling turtle_json_mex.c\n');
    
    %TODO: mex maker should do this ...
    clear turtle_json_mex
    %c = mex.compilers.gcc('./turtle_json_mex.c');
    % c = mex.compilers.gcc('./turtle_json_mex.c',...
    %     'files',{...
    %     './turtle_json_main.c', ...
    %     './turtle_json_mex_helpers.c'});
    %
    c = mex.compilers.gcc('./turtle_json_mex.c',...
        'files',{...
        './turtle_json_main.c', ...
        './turtle_json_post_process.c', ...
        './turtle_json_mex_helpers.c', ...
        './turtle_json_pp_objects.c', ...
        './turtle_json_number_parsing.c'},'verbose',in.verbose);
    c.addLib('openmp');
    
    %Only if not m1
    %
    %   TODO: At some point run explicit test
    
    if ismac()
        [~,result] = system('uname -v');
        is_m1_mac = any(strfind(result,'ARM64'));
    else
        is_m1_mac = false;
    end
    
    if is_m1_mac
        c.addCompileDefines({'IS_M1_MAC'});
        c.addCompileFlags('-march=native');
    else
        c.addCompileFlags('-mavx');
    end
    
    if in.log_timing
        c.addCompileDefines({'LOG_TIME'});
    end
    if in.log_alloc
        c.addCompileDefines({'LOG_ALLOC'});
    end
    
    build_spec = c.getBuildSpec();
    compile_statements = build_spec.getCompileStatments();
    linker_statement = build_spec.getLinkerStatement();
    %   -> can be examined for the compile statements
    
    c.build();
end

%Compiling of json_info_to_data.c and associated files
%--------------------------------------------------------
if isempty(file_id) || file_id == 2
    fprintf('Compiling json_info_to_data.c\n');
    clear json_info_to_data
    
    c = mex.compilers.gcc('./json_info_to_data.c',...
        'files',{...
        './json_info_to_data__arrays.c', ...
        './json_info_to_data__objects.c', ...
        './json_info_to_data__utils.c', ...
        './json_info_to_data__option_handling.c'},'verbose',in.verbose);
    if in.allow_ref_count
        c.addCompileDefines({'ALLOW_REF_COUNT'});
    else
        %nothing
    end
    
    c.build();
end

end
