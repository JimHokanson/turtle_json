%{

Oops, see better info (I think) in docs folder

Entry:

turtle_json_mex.c - this file handles the parsing
    - mexFunction()
        - parse_json() - turtle_json_main.c
        - post_process() - turtle_json_post_process.c

json_info_to_data.c - this file converts our parse "tree" to the expected
                      MATLAB format

%}