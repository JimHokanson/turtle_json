classdef tokens
    %
    %   Class:
    %   json.tokens
    %
    
    properties
       file_string
       info
       %1) Type
       %2) Start
       %3) End
       %4) Size
       %5) Parent Index - 0 based 
       %6) Next Object - 0 based
       
       numeric_data
       d0
       d1
       d2
       chars_per_token
       ns_per_token
    end
    
    %{
    
    Errors
    ------
    1) No opening bracket
    str = '1,2,3,4';
    jt = json.tokens('','raw_string',str);
    
    2) Trailing whitespace
    str = '[1,2,3,4]    '
    
    
    3) multiple top level tokens
    str = '[1,2,3][4,5,6]'
    
    4) No terminiating quotes on string
    str = '["This is a test';
    
    5) Double :
    str = {"test"::1}
    
    
    
    str = '[1,null,2,3]';
    jt = json.tokens('','raw_string',str);
    obj = parse_json(str,jt.info,jt.numeric_data)
    
    str = '{}'
    jt = json_tokens('','raw_string',str);
    
    str = '[1,null,true,false,"my_string",2.0123456789012345]'
    jt = json.tokens('','raw_string',str);
    obj = parse_json(str,jt.info,jt.numeric_data)
    
    %}
    
    
    methods
        function obj = tokens(file_path,varargin)
            %
            %
            %
            %   See Also:
            %   ---------
            %   json.stringToTokens
            %   json.fileToTokens
            
            in.n_tokens = 0;
            in.raw_string = -1;
            in = sl.in.processVarargin(in,varargin);
            
            if in.raw_string == -1
                t0 = tic;
                uint8_data = read_to_uint8(file_path);
                obj.file_string = char(uint8_data);
                obj.d0 = toc(t0);
                t1 = tic;
                if in.n_tokens
                    result = jsmn_mex(uint8_data,in.n_tokens);
                else
                    result = jsmn_mex(uint8_data);
                end
                obj.d1 = toc(t1);
            else
                t0 = tic;
                obj.file_string = in.raw_string;
                obj.d0 = toc(t0);
                t1 = tic;
                if in.n_tokens
                    result = jsmn_mex(in.raw_string,in.n_tokens);
                else
                    result = jsmn_mex(in.raw_string);
                end
                obj.d1 = toc(t1);
            end
            
% % % %             token_info = result.info;
% % % %             num = result.values;
            
            %[token_info,num]
            
% %             t2 = tic;
% %             %NOTE: We're not changing the parent which is currently 0 based
% %             %Ideally we could pass this into the parser ...
% %             %2) start
% %             token_info(2,:) = token_info(2,:)+1; %off by 1, might change code
% %             %6) 
% %             token_info(6,:) = token_info(6,:)+1; %same here, this could change
% %             %temp = token_info(7:8,:);
% %             
% %             obj.d2 = toc(t2);
            
            obj.numeric_data = result.values;
            obj.info = result.info;
            obj.chars_per_token = length(obj.file_string)/length(obj.numeric_data);
            obj.ns_per_token = 1e9*obj.d1/length(obj.numeric_data);
        end
        function root = get_root_token(obj)
            j = obj.info;
            if j(1,1) == 1
                root = json.object_token('root','root',1,obj);
            elseif j(1,1) == 2
                error('Not yet implemented')
                %output = parse_array(str,j,1,numeric_data,in);
            else
                error('Unexpected parent object')
            end 
        end
    end
    
end

