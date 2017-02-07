classdef token < handle
    %
    %   Class
    %   json.objs.token
    %
    %   See Also
    %   ---------
    %   json.objs.token.array
    %   json.objs.token.object
    
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
        function data = getParsedData(obj)
            data = json_info_to_data(0,obj.mex,obj.md_index);
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

