function [avg_hr, debug] = hr_calc_pda(temporal_mean, fr, firstSample, window_size, overlap_ratio, minPeakDistance, threshold)
	
	% Perform peak counting for each window
	windowStart = firstSample;
	heartBeats = [];
	heartRates = [];
	while windowStart <= (length(temporal_mean) - window_size)
		% Window to perform peak-counting in
		segment = temporal_mean(windowStart : windowStart + window_size - 1);
		
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
		%segment_length = window_size;
		
		% Record all beats in the window, even if there are duplicates
		heartBeats = [heartBeats; [max_peak_strengths, (windowStart - 1 + max_peak_locs)]];		
		
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