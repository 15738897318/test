function [temporal_mean_filt, debug] = frames2signal(monoframes, conversion_method, fr, cutoff_freq)
	% Load constants
	constants;
	
	%=== Block 1. Convert the frame stream into a 1-D signal
	%conversion_method = 'simple-mean';
	switch conversion_method
		case 'simple-mean'
			temporal_mean = squeeze(nanmean(nanmean(monoframes, 1), 2));
			
		case 'trimmed-mean'	
			temporal_mean = squeeze(trimmean(reshape(monoframes, [size(monoframes, 1) * size(monoframes, 2), size(monoframes, 3)]), trimmed_size, 1));
		
		case 'mode-balance'
			% Selection parameters
			training_time = training_time_range; %seconds %Double
			lower_pct_range = pct_reach_below_mode; %Double
			upper_pct_range = pct_reach_above_mode; %Double
	
			% Find the mode of the pixel values in the first few frames
			stretched_first_frame = reshape(monoframes(:, :, round(fr * training_time(1)) + 1 : round(fr * training_time(2))), ...
											[size(monoframes, 1) * size(monoframes, 2) * (round(fr * training_time(2)) - round(fr * training_time(1))), 1]);
			%Double 1-D vector
			[counts, centres] = hist(stretched_first_frame, number_of_bins);
			%[Int vector, Double vector] 
			[~, argmax] = max(counts); %Int
			centre_mode = centres(argmax); %Double
			
			% Find the percentile range centred on the mode
			percentile_of_centre_mode = invprctile(stretched_first_frame, centre_mode); %Double
			percentile_range = [max(0, percentile_of_centre_mode - lower_pct_range), min(100, percentile_of_centre_mode + upper_pct_range)]; %Double 2-element vector
			
			% Correct the percentile range for the boundary cases
			if percentile_range(2) == 100
				percentile_range(1) = 100 - (lower_pct_range + upper_pct_range);
			end
			if percentile_range(1) == 0
				percentile_range(2) = (lower_pct_range + upper_pct_range);
			end
			
			% Convert the percentile range into pixel-value range
			range = prctile(stretched_first_frame, percentile_range); %Double 2-element vector
			
			% For each video frame, values outside the range are rejected
			monoframes((monoframes < range(1)) | (monoframes > range(2))) = NaN;
	
			% Calculate the average of each frame
			temporal_mean = squeeze(nanmean(nanmean(monoframes, 1), 2));
			
			debug.monoframes = monoframes;
	end
	
	%=== Block 2. Low-pass-filter the signal stream to remove unwanted noises
	filter_kernel = beatSignalFilterKernel;
    
    temporal_mean_filt = filter(filter_kernel, 1, temporal_mean);  %Double 1-D vector
	temporal_mean_filt = temporal_mean_filt((length(filter_kernel) - floor(length(filter_kernel) / 2)) : end);