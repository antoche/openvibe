function [errorCode, outHeaders, outChunks] = ovm_processBuffer(inputIndex, chunkStartTime, chunkEndTime, chunkMatrix)
% Warning : input arguments are deleted after this function returns.
% If needed, copy their content before returning.

% This function is called each time a chunk is received by OpenViBE Matlab
% BoxAlgorithm.
% It is possible to return headers for outputs that have not been returned by
% ovm_processHeader, and chunks for outputs whose header has already been
% returned or is returned now.
global referenceInput
global inputHeaderReceived outputHeaderSent
global outSamplingRate outSamplesPerChunk

    %disp('ovm_processBuffer : received chunk');

    errorCode = 0;
    outHeaders = cell(1,0);
    outChunks = cell(1,0);
    
    if( ~inputHeaderReceived(inputIndex+1) )
        disp('ovm_processBuffer : no header for input ');
            errorCode = 1;
            return
    end

    % outHeaders : cell array of output headers
    %             (output headers can also be given in processHeader function,
    %              but should be given once per output described in ovm_initialize function)
    % header : cell-array of
    %   header{1}: outputIndex 
    %   header{2}: samplingRate(no signification for something else than signal) 
    %   header{3}: samplesPerChunk
    %   header{4}: channelsList (cell array of string)
    
    % outChunks : cell array of output chunks.
    %             if header for output index i has not been returned yet
    %             and you need to return a chunk for this output, please return
    %             both header and chunk in this call. 
    % chunk : cell-array of
    %   header{1}: outputIndex
    %   header{2}: chunk matrix
    
    %
    % Algo here.
    % In this example, the first sample of each channel 
    % of each chunk is 1 for output 1 and 5 for output 2.
    %
    
        
    if( inputIndex + 1 == referenceInput && outputHeaderSent(1) && outputHeaderSent(2) )
        outChunks = cell(1,2);
        
        outChunks{1} = cell(1,2);
        outChunks{1}{1} = 0;          % index of output 1. header should have been returned by ovm_processHeader
        outChunks{1}{2} = zeros(2,outSamplesPerChunk); % chunk size is determined by dimensions given in corresponding header (nb channels x samples per chunk)

        outChunks{1}{2}(1,1) = 1;
        outChunks{1}{2}(2,1) = 1;
        
        outChunks{2} = cell(1,2);
        outChunks{2}{1} = 1;          % index of output 2. header should have been returned by ovm_processHeader
        outChunks{2}{2} = zeros(3,outSamplesPerChunk); % chunk size is determined by dimensions given in corresponding header (nb channels x samples per chunk)

        outChunks{2}{2}(1,1) = 5;
        outChunks{2}{2}(2,1) = 5;
        outChunks{2}{2}(3,1) = 5;
        
    end
    
    
       
    
end