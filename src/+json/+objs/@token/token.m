classdef token < handle
    %
    %   Class
    %   json.objs.token
    %
    %   See Also
    %   ---------
    %   json.objs.token.array
    %   json.objs.token.object
    
    %{
        //2d array - TODO: Move this to a test
        s = '[[1,2,3],[4,5,6]]';
        r = json.tokens.parse(s);
        wtf1 = r.getParsedData('stripe_array_low',true)
        wtf2 = r.getParsedData('stripe_array_low',false)
        wtf3 = r.getParsedData('stripe_array_low',false,'max_numeric_collapse_depth',1)
    %}
    
    properties
        md_index %Main Data Index
        mex %strucure returned from mex parsing
        %This structure is not specific to any instance
    end
    
    properties (Dependent)
       parse_info %json.objs.token.parsing_info
       %
       %    Information about parsing performance:
       %    - timing
       %    - memory usage
    end
    
    methods
        function value = get.parse_info(obj)
           value = json.objs.token.parsing_info(obj.mex);
        end
    end
    
    methods
        function data = getParsedData(obj,varargin)
            
            if isempty(varargin)
                data = json_info_to_data(0,obj.mex,obj.md_index);
            else
                in.max_numeric_collapse_depth = [];
                in.max_string_collape_depth = [];
                in.max_bool_collapse_depth = [];
                in.stripe_array_low = [];
                in.collapse_objects = [];
                in = json.sl.in.processVarargin(in,varargin);
                data = json_info_to_data(7,obj.mex,obj.md_index,in);
            end
        end
    end
    
    methods (Hidden)
      function lh = addlistener(varargin)
         lh = addlistener@handle(varargin{:});
      end
      function notify(varargin)
         notify@handle(varargin{:});
      end
      function delete(varargin)
          try
              delete@handle(varargin{:});
          catch ME
          %This appears to occur after the class has been edited
          %while in debug mode.
              
%               formattedWarning('WHAT THE HECK')
%               keyboard
          end
      end
      function Hmatch = findobj(varargin)
         Hmatch = findobj@handle(varargin{:});
      end
      function p = findprop(varargin)
         p = findprop@handle(varargin{:});
      end
      function TF = eq(varargin)
         TF = eq@handle(varargin{:});
      end
      function TF = ne(varargin)
         TF = ne@handle(varargin{:});
      end
      function TF = lt(varargin)
         TF = lt@handle(varargin{:});
      end
      function TF = le(varargin)
         TF = le@handle(varargin{:});
      end
      function TF = gt(varargin)
         TF = gt@handle(varargin{:});
      end
      function TF = ge(varargin)
         TF = ge@handle(varargin{:});
      end
    end
    
end

