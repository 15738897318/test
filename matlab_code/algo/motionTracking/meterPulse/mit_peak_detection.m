function signals = mit_peak_detection(signals, samplingRate)
	for i = 1 : length(signals)
		time_series = signals{i}.timeseries;
		freq_pulse = signals{i}.freq_pulse;
		
		window_width = round(samplingRate / freq_pulse);
		if length(time_series) <= window_width
			window_width = length(time_series);
		end

		peaks = [];
		% Perform peak-detection at each sample
		for j = 1 : length(time_series)
			% Define window start & end
			window_start = max(1, j - floor(window_width / 2));
			window_end = min(length(time_series), window_start + window_width - 1);
			window_start = window_end - window_width + 1;

			if time_series(j) == max(time_series(window_start : window_end))
				peaks = [peaks; [j, time_series(j)]];
			end
		end

		% Calculate the heart-rate variability (hrv)
		hrv.sdnn = [];
		hrv.sdsd = [];
		hrv.rmssd = [];
		if numel(peaks) > 0
			hrv.sdnn = std(diff(peaks(:, 1)) / samplingRate);
			hrv.sdsd = std(diff(diff(peaks(:, 1))) / samplingRate);
			hrv.rmssd = sqrt(sum((diff(diff(peaks(:, 1)))).^2)) / samplingRate;
		end

		signals{i}.peaks = peaks;
		signals{i}.hrv = hrv;

		% Plot signal and peaks
		figure();
		hold('on');
		plot([0 : length(time_series) - 1] / samplingRate, time_series);
		scatter(peaks(:, 1) / samplingRate, peaks(:, 2));
		hold('off');
		xlabel('Seconds');
		legend(sprintf('HR: %g BMP', size(peaks, 1) / (length(time_series) / samplingRate) * 60), ...
			   sprintf('HRV (SDNN): %.4g ms', hrv.sdnn));
	end
end