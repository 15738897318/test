function [avg_hr, debug] = hr_calc_autocorr(temporal_mean, fr, firstSample, window_size, overlap_ratio, minPeakDistance)

	% Step 1: Calculate the window-based autocorrelation of the signal stream
	windowStart = firstSample;
	autocorrelation = [];
	while windowStart <= (length(temporal_mean) - window_size)
		% Window to calculate the autocorrelation for
		segment = temporal_mean(windowStart : windowStart + window_size - 1);
	
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
		%segment_length = window_size;
		
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
		%segment_length = window_size;
		
		% Record all beats in the window, even if there are duplicates
		heartBeats = [heartBeats; [max_peak_strengths', (windowStart - 1 + max_peak_locs)']];		
		
		% Calculate the HR for this window
		heartRates(windowStart : windowStart + segment_length - 1) = ones(1, segment_length) * length(max_peak_locs) / segment_length * fr;
		
		% Define the start of the next window
		windowStart = windowStart + round((1 - overlap_ratio) * segment_length);
	end
	
	% Prune the beats counted to include only unique ones
	heartBeats = unique(heartBeats, 'rows', 'stable');
	
	% Calculate the average HR for the whole stream
	if ~isempty(heartBeats)
		avg_hr = round(size(heartBeats, 1) / length(heartRates(firstSample : end)) * fr * 60);
	else
		avg_hr = 0;
	end
	
	debug.heartBeats = heartBeats;
	debug.heartRates = heartRates;
	debug.autocorrelation = autocorrelation;