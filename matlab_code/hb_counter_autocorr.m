function [avg_hr, heartBeats, debug] = hb_counter_autocorr(temporal_mean, fr, firstSample, window_size, overlap_ratio, minPeakDistance, threshold)
	%Only 1 window
	%window_size = length(temporal_mean) - firstSample;
	
	
	% Step 1: Calculate the window-based autocorrelation of the signal stream
	windowStart = firstSample; %Int
	autocorrelation = []; %Double vector
	while windowStart <= (length(temporal_mean) - window_size)
		% Window to calculate the autocorrelation for
		segment = temporal_mean(windowStart : windowStart + window_size - 1); %Double vector
	
		% Calculate the autocorrelation for the current window
		local_autocorr = conv(segment - mean(segment), fliplr(segment - mean(segment)), 'same'); %Double vector
		
		% Define the segment length
		% a. Shine-step-counting style
		[~, max_peak_locs] = findpeaks(local_autocorr, 'MINPEAKDISTANCE', minPeakDistance);
		%[Double vector, Int vector]
		
		if isempty(max_peak_locs)
			segment_length = window_size; %Int
		else
			[~, min_peak_locs] = findpeaks(-local_autocorr, 'MINPEAKDISTANCE', minPeakDistance); %Int vector
			
			if isempty(min_peak_locs)
				segment_length = round((max(max_peak_locs) + window_size) / 2); %Int
			else
				segment_length = round((max(min_peak_locs) + max(max_peak_locs)) / 2); %Int
			end
		end
		
		% b. Equal-step progression
		%segment_length = window_size;
		
		% Record the autocorrelation for the current window
		autocorrelation(windowStart : windowStart + length(local_autocorr) - 1) = local_autocorr;
		
		% Define the start of the next window
		windowStart = windowStart + round((1 - overlap_ratio) * segment_length);
	end
	
	% Step 2: perform peak-counting on the autocorrelation stream
	windowStart = firstSample;
	heartBeats = []; %Tx2 array: col 1 == double, col 2 == int
	heartRates = []; %Double vector
	while windowStart <= (length(autocorrelation) - window_size)
		segment = autocorrelation(windowStart : windowStart + window_size - 1);
	
		[max_peak_strengths, max_peak_locs] = findpeaks(segment, 'MINPEAKDISTANCE', minPeakDistance);
		
		% Define the segment length
		% a. Shine-step-counting style
		if isempty(max_peak_locs)
			segment_length = window_size; %Int
			
			heartRates(windowStart : windowStart + segment_length - 1) = zeros(1, segment_length);
		else
			[~, min_peak_locs] = findpeaks(-segment, 'MINPEAKDISTANCE', minPeakDistance, 'THRESHOLD', threshold); %Int vector
			
			if isempty(min_peak_locs)
				segment_length = round((max(max_peak_locs) + window_size) / 2); %Int
			else
				segment_length = round((max(min_peak_locs) + max(max_peak_locs)) / 2); %Int
			end
			
			heartRates(windowStart : windowStart + segment_length - 1) = ones(1, segment_length) * length(max_peak_locs) / sum(isfinite(segment)) * fr;
		end
		
		% b. Equal-step progression
		%segment_length = window_size;
		
		% Record all beats in the window, even if there are duplicates
		heartBeats = [heartBeats; [max_peak_strengths', (windowStart - 1 + max_peak_locs)']];		
		
		% Define the start of the next window
		windowStart = windowStart + round((1 - overlap_ratio) * segment_length);
	end
	
	% Prune the beats counted to include only unique ones
	heartBeats = unique(heartBeats, 'rows', 'stable');
	
	% Calculate the average HR for the whole stream
	if ~isempty(heartBeats)
		%avg_hr = round(size(heartBeats, 1) / length(heartRates(firstSample : end)) * fr * 60); %Double
		
		number_of_relevant_frames = length(heartRates(firstSample : end)) - sum(~isfinite(temporal_mean(firstSample : end))); %Int
		if number_of_relevant_frames ~= 0
			relevant_time = number_of_relevant_frames / (fr * 60); %Double
			avg_hr = round(size(heartBeats, 1) / relevant_time); %Double
		else
			avg_hr = 0;
		end
	else
		avg_hr = 0;
	end
	
	debug.heartRates = heartRates;
	debug.autocorrelation = autocorrelation;