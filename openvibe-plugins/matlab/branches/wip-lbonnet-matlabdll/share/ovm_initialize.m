function [errorCode] = ovm_initialize(settings, inputs, outputs)
% Warning : arguments are deleted after this function returns.
% If needed, copy their content before returning.

% settings is a cell-array of "setting cells"
% inputs is a cell-array of "input cells"
% outputs is a cell-array of "output cells"
global referenceInput
global inputHeaderReceived outputHeaderSent
global outSamplingRate outSamplesPerChunk

referenceInput = -1;
inputHeaderReceived = [false false];
outputHeaderSent = [false false];
outSamplingRate = 0;
outSamplesPerChunk = 0;

    disp('MATLAB DLL : demo');
    errorCode = 0;
        
    % check settings number
    [dim1,settingsSize] = size(settings);
    if( dim1 ~= 1 ),
        disp('ovm_initialize : settings have wrong format');
        errorCode = 1;
        return
    end
    if( settingsSize ~= 2 )
        disp('ovm_initialize : need 2 settings');
        errorCode = 1;
        return
    end
    disp('ovm_initialize : settings number OK');
    
    % check all settings 
    % setting is a cell array with :
    %   setting{1} : setting name (string)
    %   setting{2} : setting type (uint64)
    %   setting{3} : setting value 
    
    setting1 = settings{1};
    if( strcmpi(setting1(1), 'setting 1')==0 )
        disp('ovm_initialize : setting 1 must have name "setting 1"');
        errorCode = 1;
        return
    end
    if( setting1{2} ~= uint64(3675789269079252888) ) %decimal value of OV_TypeId_Filename
        disp('ovm_initialize : setting 1 must have type "filename"');
        errorCode = 1;
        return
    end
    value1 = setting1{3};
    if exist(value1,'file') ~= 2
        disp('ovm_initialize : setting 1 is not valid');
        errorCode = 1;
        return
    end
    
    setting2 = settings{2};
    if( strcmpi(setting2(1), 'setting 2')==0 )
        disp('ovm_initialize : setting 2 must have name "setting 2"');
        errorCode = 1;
        return
    end
    if( setting2{2} ~= uint64(5848511733638821951) ) %decimal value of OV_TypeId_Float
        disp('ovm_initialize : setting 2 must have type "float"');
        errorCode = 1;
        return
    end
    
    disp('ovm_initialize : all settings OK');
    
    
    % check inputs number
    
    [dim1,inputsSize] = size(inputs);
    if( dim1 ~= 1 ),
        disp('ovm_initialize : inputs have wrong format');
        errorCode = 1;
        return
    end
    if( inputsSize ~= 2 )
        disp('ovm_initialize : need 2 inputs');
        errorCode = 1;
        return
    end
    disp('ovm_initialize : inputs number OK');
    
    % check all inputs 
    % input is a cell array with :
    %   input{1} : input name (string)
    %   input{2} : input type (uint64)

    signalInput1 = inputs{1};
    if( signalInput1{2} ~= uint64(6603228299212614369) )  %decimal value of OV_TypeId_Signal
        disp('ovm_initialize : input 1 must have type "signal"');
        errorCode = 1;
        return
    end
    
    signalInput2 = inputs{2};
    if( signalInput2{2} ~= uint64(6603228299212614369) )  %decimal value of OV_TypeId_Signal
        disp('ovm_initialize : input 2 must have type "signal"');
        errorCode = 1;
        return
    end
    disp('ovm_initialize : all inputs OK');
    
    inputHeaderReceived(1) = false;
    inputHeaderReceived(2) = false;
    
    % check outputs number
    
    [dim1,outputsSize] = size(outputs);
    if( dim1 ~= 1 ),
        disp('ovm_initialize : outputs have wrong format');
        errorCode = 1;
        return
    end
    if( outputsSize ~= 2 )
        disp('ovm_initialize : need 2 outputs');
        errorCode = 1;
        return
    end
    disp('ovm_initialize : outputs number OK');
    
    % check all outputs 
    % output is a cell array with :
    %   output{1} : output name (string)
    %   output{2} : output type (uint64)

    signalOutput1 = outputs{1};
    if( signalOutput1{2} ~= uint64(6603228299212614369) )   %decimal value of OV_TypeId_Signal
        disp('ovm_initialize : output 1 must have type "signal"');
        errorCode = 1;
        return
    end
    
    signalOutput2 = outputs{2};
    if( signalOutput2{2} ~= uint64(6603228299212614369) )   %decimal value of OV_TypeId_Signal
        disp('ovm_initialize : output 2 must have type "signal"');
        errorCode = 1;
        return
    end
    disp('ovm_initialize : all outputs OK');
    
    outputHeaderSent(1) = false;
    outputHeaderSent(2) = false;
    

end