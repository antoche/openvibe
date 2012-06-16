% tuto1_signal_filter_Initialize.m
% -------------------------------
% Author : Laurent Bonnet (INRIA)
% Date   : 25 May 2012
%
% The function tuto1_signal_filter_Initialize is called when pressing 'play' in the scenario.
%
function box_out = tuto1_signal_filter_Initialize(box_in)
	
	disp('Matlab initialize function has been called.')

	box_in.user_data.trigger_state = false;
	
	% Don't forget to pass the modified box as output.
	box_out = box_in;
   
end
    