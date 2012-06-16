% tuto3_sinus_generator_Process.m
% -------------------------------
% Author : Laurent Bonnet (INRIA)
% Date   : 25 May 2012
%

function box_out = tuto3_sinus_generator_Process(box_in)
	
    channel_count = box_in.settings(1).value;
    samples_per_buffer = box_in.settings(2).value;
    amplitude = box_in.settings(3).value;
    stimulation = box_in.settings(4).value;
    
    start_time = box_in.user_data.last_end_time;
    end_time = box_in.clock;
    % We produce data of the past step (e.g. at t2 we produce the data of [t1,t2]), thus we skip the first call
    if start_time ~= 0
        T = 1/box_in.outputs{1}.header.sampling_rate;
		t = (box_in.user_data.nb_samples:box_in.user_data.nb_samples+samples_per_buffer-1)*T;
        signal = zeros(channel_count, samples_per_buffer);
        freq = 10;
        for chan = 1 : channel_count
           sinus = amplitude * sin(2*pi* freq *t);
           signal(chan,1:samples_per_buffer) = sinus;
           freq = freq + 10;
        end
        
        if mod(box_in.user_data.call_count, box_in.clock_frequency) == 0
            stim_set = [stimulation; box_in.clock; 0];
            disp('Sending a stimulation...');
        else
            stim_set = [];
        end
        
        box_in = OV_addOutputBuffer(box_in,1,start_time,end_time,signal);
        box_in = OV_addOutputBuffer(box_in,2,start_time,end_time,stim_set);
        
        box_in.user_data.nb_samples = box_in.user_data.nb_samples + samples_per_buffer;
    end
    
    box_in.user_data.last_end_time = end_time;
    box_in.user_data.call_count = box_in.user_data.call_count +1;
    
    box_out = box_in;
end
		