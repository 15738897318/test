function [temporal_mean_filt, debug] = frames2signal(monoframes, conversion_method, fr, cutoff_freq)
		
	%=== Block 1. Convert the frame stream into a 1-D signal
	%conversion_method = 'simple-mean';
	switch conversion_method
		case 'simple-mean'
			temporal_mean = squeeze(nanmean(nanmean(monoframes, 1), 2));
			
		case 'trimmed-mean'	
			temporal_mean = squeeze(mean(reshape(monoframes, [size(monoframes, 1) * size(monoframes, 2), size(monoframes, 3)]), 30, 1));
		
		case 'mode-balance'
			% Selection parameters
			training_time = 0.5; %seconds
			lower_pct_range = 30;
			upper_pct_range = 30;
	
			% Find the mode of the pixel values in the first few frames
			stretched_first_frame = reshape(monoframes(:, :, 1 : round(fr * training_time)), [size(monoframes, 1) * size(monoframes, 2) * round(fr * training_time), 1]);
			[counts, centres] = hist(stretched_first_frame, 50 * round(fr * training_time));
			[~, argmax] = max(counts);
			centre_mode = centres(argmax);
			
			% Find the percentile range centred on the mode
			percentile_of_centre_mode = invprctile(stretched_first_frame, centre_mode);
			percentile_range = [max(0, percentile_of_centre_mode - lower_pct_range), min(100, percentile_of_centre_mode + upper_pct_range)];
			
			% Correct the percentile range for the boundary cases
			if percentile_range(2) == 100
				percentile_range(1) = 100 - (lower_pct_range + upper_pct_range);
			end
			if percentile_range(1) == 0
				percentile_range(2) = (lower_pct_range + upper_pct_range);
			end
			
			% Convert the percentile range into pixel-value range
			range = prctile(stretched_first_frame, percentile_range);
			
			% For each video frame, values outside the range are rejected
			monoframes((monoframes < range(1)) | (monoframes > range(2))) = NaN;
	
			% Calculate the average of each frame
			temporal_mean = squeeze(nanmean(nanmean(monoframes, 1), 2));
			
			debug.monoframes = monoframes;
	end
	
	%=== Block 2. Low-pass-filter the signal stream to remove unwanted noises
	H = design(fdesign.lowpass('N,Fp,Fst', 14, cutoff_freq / fr, 1.1 * cutoff_freq / fr), 'ALLFIR');
    b = H(2).Numerator / sum(H(2).Numerator);
    a = 1;
	temporal_mean_filt = filter(b, a, temporal_mean);
	temporal_mean_filt = temporal_mean_filt(8 : end);