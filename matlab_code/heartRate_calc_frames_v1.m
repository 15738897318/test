% window_size = 90;
% overlap_ratio = 0;
%	'/Users/misfit/Desktop/Codes - Local/Working bench/bioSignalProcessing/eulerianMagnifcation/codeMatlab/Results/sample-Dung-finger-ideal-from-0.83333-to-1.5-alpha-50-level-4-chromAtn-1.avi'


function hr_array = heartRate_calc_frames(vidFolder, in_filetype, window_size_in_sec, overlap_ratio, max_bpm, cutoff_freq, colour_channel, ref_reading, colourspace, time_lag)
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
	
	
	%% Block 3 ==== Heart-rate calculation
	% Set peak-detection params
    threshold = threshold_fraction * max(temporal_mean(firstSample : end)); %Double
    minPeakDistance = max(round(60 / max_bpm * fr), 1); %Int
	
	% Calculate heart-rate using peak-detection on the signal
	[heartBeats_pda, avg_hr_simple_pda, debug_beats_pda] = hb_counter_pda(temporal_mean, fr, firstSample, window_size, overlap_ratio, minPeakDistance, threshold);
	%Double
	
	% Calculate heart-rate using peak-detection on the signal
	[heartBeats_autocorr, avg_hr_simple_autocorr, debug_beats_autocorr] = hb_counter_autocorr(temporal_mean, fr, firstSample, window_size, overlap_ratio, minPeakDistance, threshold);
	%Double
	
	%=== v1: Heart-rates as the simple average of the heart-beats detected
	hr_pda = avg_hr_simple_pda;
	hr_autocorr = avg_hr_simple_autocorr;
	
	%=== v2: Perform more sophisticated heart-rate calculations based on the detected heart-beats
	[heartRate_pda, debug_hr_pda] = hr_calculator(heartBeats_pda, fr);
	[heartRate_autocorr, debug_hr_autocorr] = hr_calculator(heartBeats_autocorr, fr);
	
	hr_pda = heartRate_pda.average;
	hr_autocorr = heartRate_autocorr.average;
	
	hr_pda = round(hr_pda);
	hr_autocorr = round(hr_autocorr);
	
	%% ============ Function output and summary
	% Display the average rate using total peak counts on the full stream
	[~, peak_locs] = findpeaks(temporal_mean(firstSample : end), 'MINPEAKDISTANCE', minPeakDistance, 'THRESHOLD', threshold);
	avg_hr = length(peak_locs) / length(temporal_mean(firstSample : end)) * fr * 60;
	disp('Average heart-rate: ');
	disp(avg_hr);
	
	disp('Average heart-rate (PDA): ');
	disp(hr_pda);
	
	disp('Average heart-rate (ACF): ');
	disp(hr_autocorr);
	
	disp('Reference heart-rate (Basis): ');
	disp(ref_reading);
	
	% Output of the function
	hr_array = [ref_reading, colour_channel, hr_autocorr, hr_pda, avg_hr];
	
	
	
	
	%% ============ Debug
    debug = 0;
	if debug
		heartRates_autocorr = debug_beats_autocorr.heartRates;
		autocorrelation = debug_beats_autocorr.autocorrelation;
		
		heartRates_pda = debug_beats_pda.heartRates;
		
		% Calculate the time & freq vectors
		time_vector = linspace(0, length(temporal_mean) / fr, length(temporal_mean));
		freq_vector = linspace(0, fr, length(temporal_mean));
	
		h(1) = figure();
		ax(1) = subplot(3, 1, 1);
		hold('on');
		plot(time_vector, temporal_mean, 'b', 'DisplayName', '1-D signal')
	
		if ~isempty(heartBeats_autocorr)
			stem(time_vector(heartBeats_autocorr(:, 2)), heartBeats_autocorr(:, 1), 'bx', 'DisplayName', 'AC peaks')
		end
		if ~isempty(heartBeats_pda)
			stem(time_vector(heartBeats_pda(:, 2)), heartBeats_pda(:, 1), 'ro', 'DisplayName', 'PD peaks') 
		end
		hold('off');
		title(sprintf([vidFolder '\nChannel: %d (%s)'], colour_channel, colourspace))
		xlim([min(time_vector) max(time_vector)])
		xlabel('Time (sec)')
		ylabel('Frame average')

		ax(2) = subplot(3, 1, 2);
		hold('on');
		plot(time_vector(1 : length(autocorrelation)), autocorrelation)
		if ~isempty(heartBeats_autocorr)
			stem(time_vector(heartBeats_autocorr(:, 2)), heartBeats_autocorr(:, 1), 'bx', 'DisplayName', 'AC peaks')
		end
		hold('off');
		xlim([min(time_vector) max(time_vector)])
		xlabel('Time (sec)')
		ylabel('Sliding-window autocorrelation')

		ax(3) = subplot(3, 1, 3);
		hold('on');
		plot(time_vector(1 : length(heartRates_autocorr)), heartRates_autocorr * 60, 'b', 'DisplayName', 'Autocorrelation')
		plot(time_vector(1 : length(heartRates_pda)), heartRates_pda * 60, 'r', 'DisplayName', 'Peak detection')
	
		plot(time_vector(firstSample : length(heartRates_autocorr)), cumsum(heartRates_autocorr(firstSample : end) * 60) ./ (1 : length(heartRates_autocorr(firstSample : end) * 60)), 'b-.', 'DisplayName', 'Autocorrelation - Running avg')
		plot(time_vector(firstSample : length(heartRates_pda)), cumsum(heartRates_pda(firstSample : end) * 60) ./ (1 : length(heartRates_pda(firstSample : end) * 60)), 'r-.', 'DisplayName', 'Peak detection - Running avg')
		if isfinite(ref_reading)
			plot(time_vector([firstSample, length(heartRates_autocorr)]), [ref_reading, ref_reading], 'k-', 'DisplayName', 'Reference reading')
		end
	
		hold('off');
		xlim([min(time_vector) max(time_vector)])
		xlabel('Time (sec)')
		ylabel('Locally-averaged heart-rate (BPM)')
		title(sprintf(['Average heart-rate: ' num2str(hr_autocorr) ' BPM & ' ...
									  num2str(hr_pda) ' BPM' ...
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