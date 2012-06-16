% tuto2_FFT_filter_Initialize.m
% -------------------------------
% Author : Laurent Bonnet (INRIA)
% Date   : 25 May 2012
%

function box_out = tuto2_FFT_filter_Initialize(box_in)
	
    % we display the setting values
    disp('Box settings are:')
	for i=1:size(box_in.settings,2)
		fprintf('\t%s : %s\n',box_in.settings(i).name, num2str(box_in.settings(i).value)); 
	end
    
	% let's add a user-defined indicator to know if the output header is set
    box_in.user_data.is_headerset = false;
	
	% We also add some statistics
	box_in.user_data.nb_matrix_processed = 0;
	box_in.user_data.mean_fft_matrix = 0;
	
    box_out = box_in;
   
end
    