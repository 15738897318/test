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
			% Downsample the frame for ease of computation
			monoframe = squeeze(double(colorframe(:, :, colour_channel)));
			monoframes(:, :, k) = corrDn(monoframe, filt, 'reflect1', [4 4], [1 1], size(monoframe));
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
    minPeakDistance = round(60 / max_bpm * fr);
	
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
		heartBeats2 = [heartBeats2; [max_peak_strengths, (windowStart - 1 + max_peak_locs)]];		
		
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
    minPeakDistance = round(60 / max_bpm * fr);
    
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
	
	
	%% ============ Function output
	% Output of the function
	hr_array = [ref_reading, colour_channel, avg_hr, avg_hr2];