% window_size = 90;
% overlap_ratio = 0;
%	'/Users/misfit/Desktop/Codes - Local/Working bench/bioSignalProcessing/eulerianMagnifcation/codeMatlab/Results/sample-Dung-finger-ideal-from-0.83333-to-1.5-alpha-50-level-4-chromAtn-1.avi'


function rate_array = rate_calc_frames(vidFolder, in_filetype, window_size_in_sec, overlap_ratio, max_bpm, cutoff_freq, colour_channel, ref_reading, colourspace, time_lag)
											%Double				%Double			%Double	%Double		 %Int			 %Double		%String		%Double
	close all
	
	%Load constants
	initialiser;
	
	debug = flagDebug;
	getRaw = flagGetRaw;
	threshold_fraction = peakStrengthThreshold_fraction; %Double
	conversion_method = frames2signalConversionMethod; %String
	
	%% Block 1 ==== Load the video & convert it to the desired colour-space
	%display(sprintf('Processing folder: %s', vidFolder));
								   
	% Read video
	vid = frame_loader(vidFolder, frame_size, colour_channel, in_filetype); %Double array

	% Extract video info
	vidHeight = size(vid, 1);
	vidWidth = size(vid, 2);
	len = size(vid, 4); %Int
	if exist([vidFolder '/vid_specs.txt'])
		fr = textscan(fopen([vidFolder '/vid_specs.txt']), '%d, %d');
		fr = double(fr{2});
	else
		fr = size(vid, 4) / recordingTime; %Double
	end
	
	nChannels = number_of_channels;
	window_size = round(window_size_in_sec * fr); %Int
	firstSample = round(fr * time_lag);	%Int
	if endIndex > 0
		endIndex = endIndex;
	else
		endIndex = len + endIndex;
	end
	
	% Convert colourspaces for each frame
	clearvars output_array
	clearvars colorframe
	clearvars colorframes
	clearvars monoframe
	clearvars monoframes
	
	filt = frame_downsampling_filt; %Double array
	frame_downsample_factor = ceil(length(filt) / 2);
	
	k = 0; %Int
	for i = startIndex : endIndex
		k = k + 1;
		
		rgbframe = vid(:, :, :, i); %Double MxNx3 array
		
		% Convert the extracted frame to the desired colour-space
		switch colourspace
			case 'rgb'
				colorframe = rgbframe; %Double MxNx3 array
			case 'hsv'
				colorframe = rgb2hsv(rgbframe); %Double MxNx3 array
			case 'ntsc'
				colorframe = rgb2ntsc(rgbframe); %Double MxNx3 array
			case 'ycbcr'
				colorframe = rgb2ycbcr(rgbframe); %Double MxNx3 array
			case 'tsl'
				colorframe = rgb2tsl(rgbframe); %Double MxNx3 array
        end
        
		if getRaw && debug
			colorframes(:, :, :, k) = colorframe; %Double MxNx3xT array
			monoframes(:, :, k) = squeeze(colorframe(:, :, colour_channel)); %Double MxNxT array
		
		else
			% Extract the right channel from the colour frame
			monoframe = squeeze(colorframe(:, :, colour_channel)); %Double MxN array
			
			% Downsample the frame for ease of computation
			monoframe = corrDn(monoframe, filt, 'reflect1', [frame_downsample_factor frame_downsample_factor], [1 1], size(monoframe));
			
			% Put the frame into the video stream
			monoframes(:, :, k) = monoframe; %Double MxNxT array
		end
	end
	
	
	
	%% Block 2 ==== Extract a signal stream & pre-process it
	% Convert the video frames into double for processing
	monoframes = double(monoframes);
	
	% Convert the frame stream into a 1-D signal
	[temporal_mean, debug_frames2signal] = frames2signal(monoframes, conversion_method, fr, cutoff_freq);
	%Double T-element vector
	
	
	%% Block 3 ==== rate calculation
	% Set peak-detection params
    threshold = threshold_fraction * max(temporal_mean(firstSample : end)); %Double
    minPeakDistance = max(round(60 / max_bpm * fr), 1); %Int
	
	% Calculate rate using peak-detection on the signal
	[beats_pda, avg_rate_simple_pda, debug_beats_pda] = beat_counter_pda(temporal_mean, fr, firstSample, window_size, overlap_ratio, minPeakDistance, threshold);
	%Double
	
	% Calculate rate using peak-detection on the signal
	[beats_autocorr, avg_rate_simple_autocorr, debug_beats_autocorr] = beat_counter_autocorr(temporal_mean, fr, firstSample, window_size, overlap_ratio, minPeakDistance, threshold);
	%Double
	
	%=== v1: rates as the simple average of the heart-beats detected
	rate_pda = avg_rate_simple_pda;
	rate_autocorr = avg_rate_simple_autocorr;
	
	%=== v2: Perform more sophisticated rate calculations based on the detected heart-beats
	[rates_pda, debug_rate_pda] = rate_calculator(beats_pda, fr);
	[rates_autocorr, debug_rate_autocorr] = rate_calculator(beats_autocorr, fr);
	
	rate_pda = rates_pda.average;
	rate_autocorr = rates_autocorr.average;
	
	rate_pda = round(rate_pda);
	rate_autocorr = round(rate_autocorr);
	
	%% ============ Function output and summary
	% Display the average rate using total peak counts on the full stream
	[~, peak_locs] = findpeaks(temporal_mean(firstSample : end), 'MINPEAKDISTANCE', minPeakDistance, 'THRESHOLD', threshold);
	avg_rate = length(peak_locs) / length(temporal_mean(firstSample : end)) * fr * 60;
	disp('Average rate: ');
	disp(avg_rate);
	
	disp('Average rate (PDA): ');
	disp(rate_pda);
	
	disp('Average rate (ACF): ');
	disp(rate_autocorr);
	
	disp('Reference rate (Basis): ');
	disp(ref_reading);
	
	% Output of the function
	rate_array = [ref_reading, colour_channel, rate_autocorr, rate_pda, avg_rate];
	
	
	
	
	%% ============ Debug
    debug = 0;
	if debug
		rates_autocorr = debug_beats_autocorr.rates;
		autocorrelation = debug_beats_autocorr.autocorrelation;
		
		rates_pda = debug_beats_pda.rates;
		
		% Calculate the time & freq vectors
		time_vector = linspace(0, length(temporal_mean) / fr, length(temporal_mean));
		freq_vector = linspace(0, fr, length(temporal_mean));
	
		h(1) = figure();
		ax(1) = subplot(3, 1, 1);
		hold('on');
		plot(time_vector, temporal_mean, 'b', 'DisplayName', '1-D signal')
	
		if ~isempty(beats_autocorr)
			stem(time_vector(beats_autocorr(:, 2)), beats_autocorr(:, 1), 'bx', 'DisplayName', 'AC peaks')
		end
		if ~isempty(beats_pda)
			stem(time_vector(beats_pda(:, 2)), beats_pda(:, 1), 'ro', 'DisplayName', 'PD peaks') 
		end
		hold('off');
		title(sprintf([vidFolder '\nChannel: %d (%s)'], colour_channel, colourspace))
		xlim([min(time_vector) max(time_vector)])
		xlabel('Time (sec)')
		ylabel('Frame average')

		ax(2) = subplot(3, 1, 2);
		hold('on');
		plot(time_vector(1 : length(autocorrelation)), autocorrelation)
		if ~isempty(beats_autocorr)
			stem(time_vector(beats_autocorr(:, 2)), beats_autocorr(:, 1), 'bx', 'DisplayName', 'AC peaks')
		end
		hold('off');
		xlim([min(time_vector) max(time_vector)])
		xlabel('Time (sec)')
		ylabel('Sliding-window autocorrelation')

		ax(3) = subplot(3, 1, 3);
		hold('on');
		plot(time_vector(1 : length(rates_autocorr)), rates_autocorr * 60, 'b', 'DisplayName', 'Autocorrelation')
		plot(time_vector(1 : length(rates_pda)), rates_pda * 60, 'r', 'DisplayName', 'Peak detection')
	
		plot(time_vector(firstSample : length(rates_autocorr)), cumsum(rates_autocorr(firstSample : end) * 60) ./ (1 : length(rates_autocorr(firstSample : end) * 60)), 'b-.', 'DisplayName', 'Autocorrelation - Running avg')
		plot(time_vector(firstSample : length(rates_pda)), cumsum(rates_pda(firstSample : end) * 60) ./ (1 : length(rates_pda(firstSample : end) * 60)), 'r-.', 'DisplayName', 'Peak detection - Running avg')
		if isfinite(ref_reading)
			plot(time_vector([firstSample, length(rates_autocorr)]), [ref_reading, ref_reading], 'k-', 'DisplayName', 'Reference reading')
		end
	
		hold('off');
		xlim([min(time_vector) max(time_vector)])
		xlabel('Time (sec)')
		ylabel('Locally-averaged rate (BPM)')
		title(sprintf(['Average rate: ' num2str(rate_autocorr) ' BPM & ' ...
									  num2str(rate_pda) ' BPM' ...
					   '\n Reference: ' num2str(ref_reading) ' BPM']));
		legend('show')
	
		linkaxes(ax, 'x')
		fig_filename = [vidFolder '-' colourspace '-cchan-' num2str(colour_channel) '-1.fig'];
		saveas(h(1), fig_filename, 'fig')
	
		h(2) = figure();
		subplot(3, 1, 1)
		hold('on');
		plot(time_vector, temporal_mean, 'b', 'DisplayName', '1-D signal')
		hold('off');
		title(sprintf([vidFolder '\nChannel: %d (%s)'], colour_channel, colourspace))
		xlim([min(time_vector) max(time_vector)])
		xlabel('Time (sec)')
		ylabel('Frame average')

		subplot(3, 1, 2)
		hold('on');
		plot(freq_vector - fr / 2, abs(fftshift(fft(temporal_mean - mean(temporal_mean)))), 'r', 'DisplayName', 'FFT of 1-D signal')
		hold('off');
		xlim([min(freq_vector - fr / 2) max(freq_vector - fr / 2)])
		xlim([0 +2.5])
		xlabel('Frequency (Hz)')
		ylabel('FFT of 1-D signal');

		window_size = 256;
		overlap_ratio = 7 / 8;
		subplot(3, 1, 3)
		spectrogram(temporal_mean - mean(temporal_mean), window_size, round(overlap_ratio * window_size), 2^(ceil(log(window_size) / log(2))), fr)
		xlim([0 2.5])
		view(90, -90)
		title('Spectrogram of the 1-D signal')
	
		fig_filename = [vidFolder '-' colourspace '-cchan-' num2str(colour_channel) '-2.fig'];
		saveas(h(2), fig_filename, 'fig')
		
		
		% Histogram
		[counts, centres] = hist(reshape(debug_frames2signal.monoframes, [size(debug_frames2signal.monoframes, 1) * size(debug_frames2signal.monoframes, 2), size(debug_frames2signal.monoframes, 3)]), 50);
		figure()
		surf([1 : size(debug_frames2signal.monoframes, 3)], centres, counts, 'LineStyle', 'none');
		view([0 90]);
		xlabel('Frame number');
		ylabel(['Bin of values for channel ' colour_channel]);
	end