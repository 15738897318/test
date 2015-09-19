function [beats, avg_rate, debug] = hb_counter_autocorr(temporal_mean, fr, firstSample, window_size, overlap_ratio, minPeakDistance, threshold)
	%Only 1 window
	%window_size = length(temporal_mean) - firstSample;
	
	
	% Step 1: Calculate the window-based autocorrelation of the signal stream
	windowStart = firstSample; %Int
	autocorrelation = []; %Double vector
	last_segment_end_value = 0; %Double
	while windowStart < length(temporal_mean)
		% Window to calculate the autocorrelation for
		windowEnd = min(windowStart + window_size - 1, length(temporal_mean));
		segment = temporal_mean(windowStart : windowEnd); %Double vector
	
		% Calculate the autocorrelation for the current window
		local_autocorr = filter(segment - mean(segment), 1, segment - mean(segment)); %Double vector
        local_autocorr = local_autocorr - min(local_autocorr);
		local_autocorr = local_autocorr - local_autocorr(1) + last_segment_end_value;
		
		% Define the segment length
		% a. Shine-step-counting style
        if  length(local_autocorr) > 3
            [~, max_peak_locs] = findpeaks(local_autocorr, 'MINPEAKDISTANCE', min(minPeakDistance, length(segment) - 2));
            %[Double vector, Int vector]

            if isempty(max_peak_locs)
                segment_length = length(segment); %Int
            else
                [~, min_peak_locs] = findpeaks(-local_autocorr, 'MINPEAKDISTANCE', min(minPeakDistance, length(segment) - 2)); %Int vector

                if isempty(min_peak_locs)
                    segment_length = round((max(max_peak_locs) + window_size) / 2); %Int
                    segment_length = min(segment_length, length(segment));
                else
                    segment_length = round((max(min_peak_locs) + max(max_peak_locs)) / 2); %Int
                end
            end

            % b. Equal-step progression
            %segment_length = length(segment);

            % Record the autocorrelation for the current window
            autocorrelation(windowStart : windowStart + length(local_autocorr) - 1) = local_autocorr;

            % Define the start of the next window
            windowStart = windowStart + round((1 - overlap_ratio) * segment_length);
            last_segment_end_value = autocorrelation(windowStart - 1);
        else
            break;
        end
	end
	
	% Step 2: perform peak-counting on the autocorrelation stream
	windowStart = firstSample;
	beats = []; %Tx2 array: col 1 == double, col 2 == int
	rates = []; %Double vector
	while windowStart < length(autocorrelation)
		windowEnd = min(windowStart + window_size - 1, length(autocorrelation));
		segment = autocorrelation(windowStart : windowEnd);
		
		if  length(segment) > 3
			[max_peak_strengths, max_peak_locs] = findpeaks(segment, 'MINPEAKDISTANCE', min(minPeakDistance, length(segment) - 2));
		
			% Define the segment length
			% a. Shine-step-counting style
			if isempty(max_peak_locs)
				segment_length = length(segment); %Int
			
				rates(windowStart : windowStart + segment_length - 1) = zeros(1, segment_length);
			else
				[~, min_peak_locs] = findpeaks(-segment, 'MINPEAKDISTANCE', min(minPeakDistance, length(segment) - 2), 'THRESHOLD', threshold); %Int vector
			
				if isempty(min_peak_locs)
					segment_length = round((max(max_peak_locs) + window_size) / 2); %Int
                    segment_length = min(segment_length, length(segment));
				else
					segment_length = round((max(min_peak_locs) + max(max_peak_locs)) / 2); %Int
				end
			
				rates(windowStart : windowStart + segment_length - 1) = ones(1, segment_length) * length(max_peak_locs) / sum(isfinite(segment)) * fr;
			end
		
			% b. Equal-step progression
			%segment_length = length(segment);
		
			% Record all beats in the window, even if there are duplicates
			beats_pos = windowStart - 1 + max_peak_locs(:);
			% beats = [beats; [max_peak_strengths(:), beats_pos]];		
			beats = [beats; [temporal_mean(beats_pos), beats_pos]];		
		
			% Define the start of the next window
			windowStart = windowStart + round((1 - overlap_ratio) * segment_length);
		else
			break;
		end
	end
	
	% Prune the beats counted to include only unique ones
	beats = unique(beats, 'rows', 'stable');
	
	% Calculate the average HR for the whole stream
	if ~isempty(beats)
		%avg_rate = round(size(beats, 1) / length(rates(firstSample : end)) * fr * 60); %Double
		
		number_of_relevant_frames = length(rates(firstSample : end)) - sum(~isfinite(temporal_mean(firstSample : end))); %Int
		if number_of_relevant_frames ~= 0
			relevant_time = number_of_relevant_frames / (fr * 60); %Double
			avg_rate = size(beats, 1) / relevant_time; %Double
		else
			avg_rate = 0;
		end
	else
		avg_rate = 0;
	end
	
	debug.rates = rates;
	debug.autocorrelation = autocorrelation;