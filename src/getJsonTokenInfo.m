function [token_info,numeric_data] = getJsonTokenInfo(str)
%
%   [token_info,numeric_data] = getJsonTokenInfo(str)
%
    
token_info = jsmn_mex(str);
numeric_data = 1;
%jsmn_mex(str);
%toc(t3);


% % % % % %Let's make some fixes ...
% % % % % token_info(2,:) = token_info(2,:)+1; %off by 1, might change code
% % % % % token_info(6,:) = token_info(6,:)+1; %this won't change since it is nice in code
% % % % % temp = token_info(7:8,:);
% % % % % numeric_data = typecast(temp(:),'double');

end