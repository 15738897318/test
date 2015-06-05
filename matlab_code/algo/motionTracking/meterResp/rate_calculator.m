function [rates, debug] = hr_calculator(beats, frameRate)
	
	if size(beats, 1) > 0
		%Calculate the instantaneous heart-rates
		rate_inst = 1 ./ diff(beats(:, 2)); % Find OpenCV equivalent
	
		%Find the mode of the instantaneous heart-rates
		[counts, centres] = hist(rate_inst, 5); % OpenCV: hist
		[~, argmax] = max(counts); %Int
		centre_mode = centres(argmax); %Double
	
		%Create a convolution kernel from the found frequency
		kernel = fspecial('gaussian', [1, ceil(2 / centre_mode)], 1 / (4 * centre_mode)); % Perform mathematical calculation
		kernel = kernel / max(kernel);
		base_threshold = 2 * kernel(ceil(1 / (4 * centre_mode)));
	
		%Create a heart-beat count signal
		count_signal = zeros(1, (max(beats(:, 2)) - min(beats(:, 2))) + 1);
		count_signal(beats(:, 2) - min(beats(:, 2)) + 1) = 1;
	
		%Convolve the count_signal with the kernel to generate a score_signal
		score_signal = conv(count_signal, kernel, 'same');
	
		%Decide if the any beats are missing and fill them in if need be
		if length(score_signal) > 3
            factor = 1.5;
            [min_peak_strengths, min_peak_locs] = findpeaks(-score_signal); % Already implemented
            min_peak_strengths = -min_peak_strengths;
            count_signal(min_peak_locs(min_peak_strengths < factor * base_threshold)) = -1;
        else
            rates.average = NaN;
            rates.mode = NaN;

            debug.count_signal = [];
            debug.score_signal = [];
            
            return
        end
        
		%Calculate the heart-rate from the new beat count
		rates.average = sum(abs(count_signal)) / (length(count_signal) + 1 / centre_mode) * frameRate * 60;
	
		rates.mode = centre_mode * frameRate * 60;
	
		debug.count_signal = count_signal;
		debug.score_signal = score_signal;
	else
		rates.average = NaN;
		rates.mode = NaN;
	
		debug.count_signal = [];
		debug.score_signal = [];
	end