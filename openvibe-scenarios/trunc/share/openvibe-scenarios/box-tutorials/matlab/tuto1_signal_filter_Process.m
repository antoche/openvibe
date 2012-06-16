% tuto1_signal_filter_Process.m
% -------------------------------
% Author : Laurent Bonnet (INRIA)
% Date   : 25 May 2012
%
% The function tuto1_signal_filter_Process is called at the clock frequency.
%

function box_out = tuto1_signal_filter_Process(box_in)
	
    % we iterate over the pending chunks on input 2 (STIMULATIONS)
	for i = 1: OV_getNbPendingInputChunk(box_in,2)
		
		% we pop the first chunk to be processed, note that box_in is used as the output variable to continue processing
		[box_in, start_time, end_time, stim_set] = OV_popInputBuffer(box_in,2);
		
		% the stimulation set is a 3xN matrix.
		% The openvibe stimulation stream sends even empty stimulation set
		% so the following boxes know there is no stimulation to expect in
		% the latest time range. These empty chunk are also in the matlab buffer.
		if(numel(stim_set) >= 3) % at least one stim in the set.
			fprintf('Received stimulation code %i at time %f\n', stim_set(1), stim_set(2))
			if stim_set(1) == box_in.settings(1).value % I'm lazy... I only check the first stim in the set.
				box_in.user_data.trigger_state = ~box_in.user_data.trigger_state;
				disp('Trigger is switched.')
			end
		end
	end
	
	% we iterate over the pending chunks on input 1 (SIGNAL)
	for i = 1: OV_getNbPendingInputChunk(box_in,1)
		
		% we pop the first chunk to be processed, note that box_in is used as the output variable to continue processing
		[box_in, start_time, end_time, matrix_data] = OV_popInputBuffer(box_in,1);
        
		box_in.outputs{1}.header = box_in.inputs{1}.header;
		
        % if requested, all samples are set to 0
		if(box_in.user_data.trigger_state)
			matrix_data(1:end) = 0;
		end
		
		% we add the chunk to the signal output buffer. Note that we use box_in as the output variable.
		box_in = OV_addOutputBuffer(box_in,1,start_time,end_time,matrix_data);
		
    end
    
    % Pass the modified box as output to continue processing
    box_out = box_in;
end
