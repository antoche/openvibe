% tuto2_FFT_filter_Process.m
% -------------------------------
% Author : Laurent Bonnet (INRIA)
% Date   : 25 May 2012
%

function box_out = tuto2_FFT_filter_Process(box_in)
	
	for i = 1: OV_getNbPendingInputChunk(box_in,1)
		if(~box_in.user_data.is_headerset)
            % The output is the input + noise, only on first channel
            box_in.outputs{1}.header = box_in.inputs{1}.header;
            box_in.outputs{1}.header.nb_channels = 1;
            box_in.user_data.is_headerset = 1;
            % We print the header in the console
            disp('Input header is :')
            box_in.inputs{1}.header
        end
        
        % we increment the matrix count
		box_in.user_data.nb_matrix_processed = box_in.user_data.nb_matrix_processed + 1;
		
		[box_in, start_time, end_time, matrix_data] = OV_popInputBuffer(box_in,1);
        
		Fs = box_in.inputs{1}.header.sampling_rate;        % Sampling frequency
		T = 1/Fs;                                           % Sample time
		L = box_in.inputs{1}.header.nb_samples_per_buffer; % Length of signal
		t = (0:L-1)*T;                                      % Time vector
		
		% We generate the requested sinusoid noise
		noise_amplitude = box_in.settings(2).value;
		noise_frequency = box_in.settings(1).value;
		sinusoid = noise_amplitude * sin(2*pi* noise_frequency *t);
		
		% We add this sinus to the original signal on first channel only
		sig = sinusoid + matrix_data(1,1:L);
		subplot(3,1,1);
		plot(Fs*t,sinusoid)
		title('Noise')
		xlabel('time (seconds)')
		
		subplot(3,1,2);
		plot(Fs*t,sig)
		title('Signal Corrupted with the noise (channel 1)')
		xlabel('time (seconds)')
		
		NFFT = 2^nextpow2(L); % Next power of 2 from length of y
		Y = fft(sig,NFFT)/L;
		f = Fs/2*linspace(0,1,NFFT/2+1);

		% Plot single-sided amplitude spectrum.
		plot_range_fmin = box_in.settings(3).value;
		plot_range_fmax = box_in.settings(4).value;
		subplot(3,1,3)
		plot(f(plot_range_fmin*2:plot_range_fmax*2),2*abs(Y(plot_range_fmin*2:plot_range_fmax*2))) 
		title('Single-Sided Amplitude Spectrum of the corrupted signal (channel 1)')
		xlabel('Frequency (Hz)')
		ylabel('Amplitude')
		
		% we sum the FFT for later mean computation
		if box_in.user_data.nb_matrix_processed == 1 
			box_in.user_data.mean_fft_matrix = Y;
		else
			box_in.user_data.mean_fft_matrix = box_in.user_data.mean_fft_matrix + Y;
		end
		
		box_in = OV_addOutputBuffer(box_in,1,start_time,end_time,sig);
	end
            
  box_out = box_in;
end
		