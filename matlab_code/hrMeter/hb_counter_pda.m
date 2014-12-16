function [heartBeats, avg_hr, debug] = hb_counter_pda(temporal_mean, fr, firstSample, window_size, overlap_ratio, minPeakDistance, threshold)
	
	% Perform peak counting for each window
	windowStart = firstSample; %Int
	heartBeats = []; %Tx2 array: col 1 == double, col 2 == int
	heartRates = []; %Double vector
	while windowStart < length(temporal_mean)
		% Window to perform peak-counting in
		windowEnd = min(windowStart + window_size - 1, length(temporal_mean));
		segment = temporal_mean(windowStart : windowEnd); %Double vector
		
		if length(segment) > 3
			% Count the number of peaks in this window
			[max_peak_strengths, max_peak_locs] = findpeaks(segment, 'MINPEAKDISTANCE', min(minPeakDistance, length(segment) - 2), 'THRESHOLD', threshold);
			%[Double vector, Int vector]
		
			% Define the segment length
			% a. Shine-step-counting style
			if isempty(max_peak_locs)
				segment_length = length(segment); %Int
			
				heartRates(windowStart : windowStart + segment_length - 1) = zeros(1, segment_length);
			else
				[~, min_peak_locs] = findpeaks(-segment, 'MINPEAKDISTANCE', min(minPeakDistance, length(segment) - 2), 'THRESHOLD', threshold); %Int vector
			
				if isempty(min_peak_locs)
					segment_length = round((max(max_peak_locs) + window_size) / 2); %Int
                    segment_length = min(segment_length, length(segment));
				else
					segment_length = round((max(min_peak_locs) + max(max_peak_locs)) / 2); %Int
				end
			
				heartRates(windowStart : windowStart + segment_length - 1) = ones(1, segment_length) * length(max_peak_locs) / sum(isfinite(segment)) * fr;
			end
		
			% b. Equal-step progression
			%segment_length = length(segment);
		
			% Record all beats in the window, even if there are duplicates
			heartBeats = [heartBeats; [max_peak_strengths(:), (windowStart - 1 + max_peak_locs(:))]];		
		
			% Define the start of the next window
			windowStart = windowStart + round((1 - overlap_ratio) * segment_length);
		else
			break;
		end
	end
	
	% Prune the beats counted to include only unique ones
	heartBeats = unique(heartBeats, 'rows', 'stable');
	
	% Calculate the average HR for the whole stream
	if ~isempty(heartBeats)
		%avg_hr = round(size(heartBeats, 1) / length(heartRates(firstSample : end)) * fr * 60); %Double
		
		number_of_relevant_frames = length(heartRates(firstSample : end)) - sum(~isfinite(temporal_mean(firstSample : end))); %Int
		if number_of_relevant_frames ~= 0
			relevant_time = number_of_relevant_frames / (fr * 60); %Double
			avg_hr = size(heartBeats, 1) / relevant_time; %Double
		else
			avg_hr = 0;
		end
	else
		avg_hr = 0; %Double
	end
	
	debug.heartRates = heartRates;