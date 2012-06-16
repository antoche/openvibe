% tuto3_sinus_generator_Initialize.m
% -------------------------------
% Author : Laurent Bonnet (INRIA)
% Date   : 25 May 2012
%

function box_out = tuto3_sinus_generator_Initialize(box_in)
	
    % we display the setting values
    disp('Box settings are:')
	for i=1:size(box_in.settings,2)
		fprintf('\t%s : %s\n',box_in.settings(i).name, num2str(box_in.settings(i).value)); 
	end
    
    % As we produce the signal ourselves, we already know the output header
    % we construct the channel names
    nb_channels = box_in.settings(1).value;
    channel_names = cell(1,nb_channels);
    for chan = 1 : nb_channels
        channel_names{chan} = sprintf('channel %i',chan);
    end
    % and deduce the other header elements from the box settings
    nb_samples_per_buffer = box_in.settings(2).value;
    sampling_rate = box_in.clock_frequency * nb_samples_per_buffer;
    
    box_in = OV_setSignalOutputHeader(box_in, 1, nb_channels, nb_samples_per_buffer, channel_names, sampling_rate);
    box_in = OV_setStimulationOutputHeader(box_in,2);
    
    % end time of the last chunk sent
    box_in.user_data.last_end_time = 0;
    % number of call to the process function
    box_in.user_data.call_count = 0;
    % sample count
    box_in.user_data.nb_samples = 0;
   
    box_out = box_in;
end
    