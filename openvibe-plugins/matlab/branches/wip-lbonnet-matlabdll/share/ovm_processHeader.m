function [errorCode, outHeaders] = ovm_processHeader(inputIndex, samplingRate, samplesPerChunk, channelsList)
% Warning : arguments are deleted after this function returns.
% If needed, copy their content before returning.

% This function is called once per inputIndex, before ovm_process is
% called.
global referenceInput
global inputHeaderReceived outputHeaderSent
global outSamplingRate outSamplesPerChunk

    disp('ovm_processHeader : received header');
    errorCode = 0;
    outHeaders = cell(1,0);
    
    if( inputIndex == 0 )
        if( inputHeaderReceived(1) == true )
            disp('ovm_processHeader : already received header for input 1');
            errorCode = 1;
            return
        else
            inputHeaderReceived(1) = true;
            referenceInput = 1;
        end
    end
    
    if( inputIndex == 1 )
        if( inputHeaderReceived(2) == true )
            disp('ovm_processHeader : already received header for input 2');
            errorCode = 1;
            return
        else
            inputHeaderReceived(2) = true;
            referenceInput = 2;
        end
    end
    
    if( outSamplingRate == 0 )
        outSamplingRate = samplingRate;
    end
    if( outSamplesPerChunk == 0 )
        outSamplesPerChunk = samplesPerChunk;
    end
    
    % outHeaders : cell array of output headers
    %             (output headers can also be given in process function,
    %              but should be given once per output described in ovm_initialize function)
    % header : cell-array of
    %   header{1}: outputIndex 
    %   header{2}: samplingRate(no signification for something else than signal) 
    %   header{3}: samplesPerChunk
    %   header{4}: channelsList (cell array of string)
    
    if( outputHeaderSent(1) ~= outputHeaderSent(2) )
        disp('ovm_processHeader : one header sent but not the other');
        errorCode = 1;
        return
    end
    
    if( ~outputHeaderSent(1) && ~outputHeaderSent(2) )
        outHeaders = cell(1,2); % 2 headers to send

        outHeaders{1} = cell(1,4);
        outHeaders{1}{1} = 0; % index output 1 (first index is 0)
        outHeaders{1}{2} = samplingRate; % sampling rate
        outHeaders{1}{3} = samplesPerChunk; % samples per chunk
        outHeaders{1}{4} = cell(1,2); % channels names
        outHeaders{1}{4}{1} = 'chan1';
        outHeaders{1}{4}{2} = 'chan2';
        
        outHeaders{2} = cell(1,4);
        outHeaders{2}{1} = 1; % index output 2
        outHeaders{2}{2} = samplingRate; % sampling rate
        outHeaders{2}{3} = samplesPerChunk; % samples per chunk
        outHeaders{2}{4} = cell(1,3); % channels names
        outHeaders{2}{4}{1} = 'chan1';
        outHeaders{2}{4}{2} = 'chan2';
        outHeaders{2}{4}{3} = 'chan3';
        
        outputHeaderSent(1) = true;
        outputHeaderSent(2) = true;
        
    end
    
    
end