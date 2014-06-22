% window_size = 90;
% overlap_ratio = 0;
%	'/Users/misfit/Desktop/Codes - Local/Working bench/bioSignalProcessing/eulerianMagnifcation/codeMatlab/Results/sample-Dung-finger-ideal-from-0.83333-to-1.5-alpha-50-level-4-chromAtn-1.avi'


function hr_array = heartRate_calc(vidFile, window_size_in_sec, overlap_ratio, max_bpm, cutoff_freq, colour_channel, ref_reading, colourspace, time_lag)
	
	close all
	
	getRaw = 0;
	threshold_fraction = 0;
	
	%% Block 1 ==== Load the video & convert it to the desired colour-space
	% Get the filename-only part of the full path
	[~, vidName] = fileparts(vidFile);
	display(sprintf('Processing file: %s', vidName));
								   
	% Read video
	vid = VideoReader(vidFile);

	% Extract video info
	vidHeight = vid.Height;
	vidWidth = vid.Width;
	nChannels = 3;
	fr = vid.FrameRate;
	len = vid.NumberOfFrames;
	
	window_size = round(window_size_in_sec * fr);
	
	% Define the indices of the frames to be processed
	startIndex = 1; %400
	endIndex = len; %1400
	
	% Convert colourspaces for each frame
	k = 0;
	filt = fspecial('gaussian', [7 7], 2.5);
	clearvars output_array
	clearvars colorframe
	clearvars colorframes
	clearvars monoframe
	clearvars monoframes
	temp = struct('cdata', zeros(vidHeight, vidWidth, nChannels, 'uint8'),...
				  'colormap', []);
	for i = startIndex : endIndex
		k = k + 1;
		
		% Extract the ith frame in the video stream
		temp.cdata = read(vid, i);
		% Convert the extracted frame to RGB image
		[rgbframe, ~] = frame2im(temp);
		
		switch colourspace
			case 'rgb'
				colorframe = rgbframe;
			case 'hsv'
				colorframe = rgb2hsv(rgbframe);
			case 'ntsc'
				colorframe = rgb2ntsc(rgbframe);
			case 'ycbcr'
				colorframe = rgb2ycbcr(rgbframe);
			case 'tsl'
				colorframe = rgb2tsl(rgbframe);
        end
        
		if getRaw
			colorframes(:, :, :, k) = colorframe;
			monoframes(:, :, k) = squeeze(colorframe(:, :, colour_channel))
	
		else
			%monoframes(:, :, k) = squeeze(double(colorframe(:, :, colour_channel)));
			
			% Downsample the frame for ease of computation
			%monoframe = filt_img(squeeze(double(colorframe(:, :, colour_channel))));
			%monoframes(:, :, k) = monoframe(1 : 4 : end, 1 : 4 : end);
			
			monoframe = squeeze(double(colorframe(:, :, colour_channel)));
			monoframes(:, :, k) = corrDn(monoframe, filt, 'reflect1', [4 4], [1 1], size(monoframe));
			
			%colorframe = colorframe(1 * round(size(colorframe, 1) / 4) + 1 : 2 * round(size(colorframe, 1) / 4), 1 * round(size(colorframe, 2) / 4) + 1 : 2 * round(size(colorframe, 2) / 4), :);
			%monoframes(:, :, k) = squeeze(colorframe(:, :, colour_channel));
		end
	end

	
	
	%% Block 2 ==== Extract a signal stream & pre-process it
	
	% Calculate a signal stream from the video stream to perform heart-beat counting on
	monoframes = double(monoframes);
	temporal_mean = squeeze(trimmean(reshape(monoframes, [size(monoframes, 1) * size(monoframes, 2), size(monoframes, 3)]), 30, 1));
	%temporal_mean = squeeze(mean(mean(monoframes, 1), 2));
	
	% Low-pass-filter the signal stream to remove unwanted noises
	firstSample = round(fr * time_lag);
	H = design(fdesign.lowpass('N,Fp,Fst', 14, cutoff_freq / fr, cutoff_freq * 1.1 / fr), 'ALLFIR');
    b = H(2).Numerator / sum(H(2).Numerator);
    a = 1;
	temporal_mean_filt = filter(b, a, temporal_mean);
	
	
	%% Block 3.1 ==== Algorithm 1: Peak counting
	% Set peak-detection params
    threshold = threshold_fraction * max(temporal_mean_filt(firstSample : end));
    minPeakDistance = max_bpm;
	
	% Perform peak counting for each window
	windowStart = firstSample;
	heartBeats2 = [];
	heartRates2 = [];
	while windowStart <= (length(temporal_mean_filt) - window_size)
		% Window to perform peak-counting in
		segment = temporal_mean_filt(windowStart : windowStart + window_size - 1);
		
		% Count the number of peaks in this window
		[max_peak_strengths, max_peak_locs] = findpeaks(segment, 'MINPEAKDISTANCE', minPeakDistance, 'THRESHOLD', threshold);
		
		% Define the segment length
		% a. Shine-step-counting style
		if isempty(max_peak_locs)
			segment_length = window_size;
		else
			[~, min_peak_locs] = findpeaks(-segment, 'MINPEAKDISTANCE', minPeakDistance, 'THRESHOLD', threshold);
			segment_length = round((max(min_peak_locs) + max(max_peak_locs)) / 2);
		end
		
		% b. Equal-step progression
		segment_length = window_size;
		
		% Record all beats in the window, even if there are duplicates
		heartBeats2 = [heartBeats2; [max_peak_strengths', (windowStart - 1 + max_peak_locs)']];		
		
		% Calculate the HR for this window
		heartRates2(windowStart : windowStart + segment_length - 1) = ones(1, segment_length) * length(max_peak_locs) / segment_length * fr;
		
		% Define the start of the next window
		windowStart = windowStart + round((1 - overlap_ratio) * segment_length);
	end
	
	% Calculate the average HR for the whole stream
	if ~isempty(heartBeats2)
		avg_hr2 = round(size(unique(heartBeats2(:, 2)), 1) / length(heartRates2(firstSample : end)) * fr * 60);
	else
		avg_hr2 = 0;
	end
	
	%% Block 3.2 ==== Algorithm 2: Autocorrelation
	% Set peak-detection params
    minPeakDistance = max_bpm;
    
	% Step 1: Calculate the window-based autocorrelation of the signal stream
	windowStart = firstSample;
	autocorrelation = [];
	while windowStart <= (length(temporal_mean_filt) - window_size)
		% Window to calculate the autocorrelation for
		segment = temporal_mean_filt(windowStart : windowStart + window_size - 1);
	
		% Calculate the autocorrelation for the current window
		local_autocorr = conv(segment - mean(segment), fliplr(segment - mean(segment)), 'same');
		
		% Define the segment length
		% a. Shine-step-counting style
		[max_peak_strengths, max_peak_locs] = findpeaks(local_autocorr, 'MINPEAKDISTANCE', minPeakDistance);
		
		if isempty(max_peak_locs)
			segment_length = window_size;
		else
			[~, min_peak_locs] = findpeaks(-local_autocorr, 'MINPEAKDISTANCE', minPeakDistance);
			segment_length = round((max(min_peak_locs) + max(max_peak_locs)) / 2);
		end
		
		% b. Equal-step progression
		segment_length = window_size;
		
		% Record the autocorrelation for the current window
		autocorrelation(windowStart : windowStart + length(local_autocorr) - 1) = local_autocorr;
		
		% Define the start of the next window
		windowStart = windowStart + round((1 - overlap_ratio) * segment_length);
	end
	
	% Step 2: perform peak-counting on the autocorrelation stream
	windowStart = firstSample;
	heartBeats = [];
	heartRates = [];
	while windowStart <= (length(autocorrelation) - window_size)
		segment = autocorrelation(windowStart : windowStart + window_size - 1);
	
		[max_peak_strengths, max_peak_locs] = findpeaks(segment, 'MINPEAKDISTANCE', minPeakDistance);
		
		% Define the segment length
		% a. Shine-step-counting style
		if isempty(max_peak_locs)
			segment_length = window_size;
		else
			[~, min_peak_locs] = findpeaks(-segment, 'MINPEAKDISTANCE', minPeakDistance);
			segment_length = round((max(min_peak_locs) + max(max_peak_locs)) / 2);
		end
		
		% b. Equal-step progression
		segment_length = window_size;
		
		% Record all beats in the window, even if there are duplicates
		heartBeats = [heartBeats; [max_peak_strengths', (windowStart - 1 + max_peak_locs)']];		
		
		% Calculate the HR for this window
		heartRates(windowStart : windowStart + segment_length - 1) = ones(1, segment_length) * length(max_peak_locs) / segment_length * fr;
		
		% Define the start of the next window
		windowStart = windowStart + round((1 - overlap_ratio) * segment_length);
	end
	
	% Calculate the average HR for the whole stream
	if ~isempty(heartBeats)
		avg_hr = round(size(unique(heartBeats(:, 2)), 1) / length(heartRates(firstSample : end)) * fr * 60);
	else
		avg_hr = 0;
	end
	
	
	%% ============ Function output and summary
	% Display the average rate using total peak counts on the full stream
	[~, peak_locs] = findpeaks(temporal_mean_filt(firstSample : end), 'MINPEAKDISTANCE', minPeakDistance, 'THRESHOLD', threshold);
	disp('Average heart-rate: ');
	disp(length(peak_locs) / length(temporal_mean_filt(firstSample : end)) * fr * 60);
	
	% Output of the function
	hr_array = [ref_reading, colour_channel, avg_hr, avg_hr2];
	
	
	%% ============ Plotting
	% Calculate the time & freq vectors
	time_vector = linspace(0, length(temporal_mean) / fr, length(temporal_mean));
	freq_vector = linspace(0, fr, length(temporal_mean));
	
	h(1) = figure();
	ax(1) = subplot(3, 1, 1);
	hold('on');
	plot(time_vector, temporal_mean, 'b', 'DisplayName', 'Raw')
	plot(time_vector(1 : end - 7), temporal_mean_filt(8 : end), 'r', 'DisplayName', 'LPF-d') 
	
	if ~isempty(heartBeats)
		stem(time_vector(heartBeats(:, 2)), heartBeats(:, 1), 'bx', 'DisplayName', 'AC peaks')
	end
	if ~isempty(heartBeats2)
		stem(time_vector(heartBeats2(:, 2) - 7), heartBeats2(:, 1), 'ro', 'DisplayName', 'PD peaks') 
	end
	hold('off');
	title(sprintf([vidName '\nChannel: %d (%s)'], colour_channel, colourspace))
	xlim([min(time_vector) max(time_vector)])
	xlabel('Time (sec)')
	ylabel('Frame average')

	ax(2) = subplot(3, 1, 2);
	hold('on');
	plot(time_vector(1 : length(autocorrelation)), autocorrelation)
	if ~isempty(heartBeats)
		stem(time_vector(heartBeats(:, 2)), heartBeats(:, 1), 'bx', 'DisplayName', 'AC peaks')
	end
	hold('off');
	xlim([min(time_vector) max(time_vector)])
	xlabel('Time (sec)')
	ylabel('Sliding-window autocorrelation')

	ax(3) = subplot(3, 1, 3);
	hold('on');
	plot(time_vector(1 : length(heartRates)), heartRates * 60, 'b', 'DisplayName', 'Autocorrelation')
	plot(time_vector(1 : length(heartRates2)), heartRates2 * 60, 'r', 'DisplayName', 'Peak detection')
	
	plot(time_vector(firstSample : length(heartRates)), cumsum(heartRates(firstSample : end) * 60) ./ (1 : length(heartRates(firstSample : end) * 60)), 'b-.', 'DisplayName', 'Autocorrelation - Running avg')
	plot(time_vector(firstSample : length(heartRates2)), cumsum(heartRates2(firstSample : end) * 60) ./ (1 : length(heartRates2(firstSample : end) * 60)), 'r-.', 'DisplayName', 'Peak detection - Running avg')
	if isfinite(ref_reading)
		plot(time_vector([firstSample, length(heartRates)]), [ref_reading, ref_reading], 'k-', 'DisplayName', 'Reference reading')
	end
	
	hold('off');
	xlim([min(time_vector) max(time_vector)])
	xlabel('Time (sec)')
	ylabel('Locally-averaged heart-rate (BPM)')
	title(sprintf(['Average heart-rate: ' num2str(avg_hr) ' BPM & ' ...
								  num2str(avg_hr2) ' BPM' ...
				   '\n Reference: ' num2str(ref_reading) ' BPM']));
	legend('show')
	
	linkaxes(ax, 'x')
	fig_filename = [vidFile '-' colourspace '-cchan-' num2str(colour_channel) '-1.fig'];
	saveas(h(1), fig_filename, 'fig')
	
	h(2) = figure();
	subplot(2, 1, 1)
	hold('on');
	plot(time_vector, temporal_mean, 'b', 'DisplayName', 'Raw')
	plot(time_vector(1 : end - 7), temporal_mean_filt(8 : end), 'r', 'DisplayName', 'LPF-d') 
	hold('off');
	title(sprintf([vidName '\nChannel: %d (%s)'], colour_channel, colourspace))
	xlim([min(time_vector) max(time_vector)])
	xlabel('Time (sec)')
	ylabel('Frame average')

	subplot(2, 1, 2)
	hold('on');
    plot(freq_vector - fr / 2, abs(fftshift(fft(temporal_mean - mean(temporal_mean)))), 'b', 'DisplayName', 'Autocorrelation')
	plot(freq_vector - fr / 2, abs(fftshift(fft(temporal_mean_filt - mean(temporal_mean_filt)))), 'r', 'DisplayName', 'Peak detection')
	hold('off');
    xlim([min(freq_vector - fr / 2) max(freq_vector - fr / 2)])
	xlim([0 +2.5])
	xlabel('Frequency (Hz)')
	
    fig_filename = [vidFile '-' colourspace '-cchan-' num2str(colour_channel) '-2.fig'];
	saveas(h(2), fig_filename, 'fig')
	
	h(3) = figure();
	subplot(3, 1, 1)
	hold('on')
	plot(time_vector, temporal_mean, 'b', 'DisplayName', 'Raw')
	plot(time_vector(1 : end - 7), temporal_mean_filt(8 : end), 'r', 'DisplayName', 'LPF-d') 
	hold('off')
	title(sprintf([vidName '\nChannel: %d (%s)'], colour_channel, colourspace))
	xlim([min(time_vector) max(time_vector)])
	xlabel('Time (sec)')
	ylabel('Frame average')

	window_size = 256;
	overlap_ratio = 7 / 8;
	subplot(3, 1, 2)
	spectrogram(temporal_mean - mean(temporal_mean), window_size, round(overlap_ratio * window_size), 2^(ceil(log(window_size) / log(2))), fr)
	xlim([0 2.5])
	view(90, -90)
	title('Spectrogram of the raw signal')
	
	window_size = 256;
	overlap_ratio = 7 / 8;
	subplot(3, 1, 3)
	spectrogram(temporal_mean_filt - mean(temporal_mean_filt), window_size, round(overlap_ratio * window_size), 2^(ceil(log(window_size) / log(2))), fr)
	xlim([0 2.5])
	view(90, -90)
	title('Spectrogram of the LPF-d signal')
	
	fig_filename = [vidFile '-' colourspace '-cchan-' num2str(colour_channel) '-3.fig'];
	saveas(h(3), fig_filename, 'fig')
	
	% Histogram
	[counts, centres] = hist(reshape(monoframes, [size(monoframes, 1) * size(monoframes, 2), size(monoframes, 3)]), 50);
	figure()
	surf([1 : size(monoframes, 3)], centres, counts);
	view([0 90]);
	xlabel('Frame number');
	ylabel(['Bin of values for channel ' colour_channel]);