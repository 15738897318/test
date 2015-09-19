function signals = own_peak_detection(signals, samplingRate, method)

	switch method
		case 'naive'
			width_coeff = 1/2;

			for i = 1 : length(signals)
				time_series = signals{i}.timeseries;
				freq_pulse = signals{i}.freq_pulse;
				
				window_width = round(width_coeff * samplingRate / freq_pulse);
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

				measured_pulse = size(peaks, 1) / length(time_series) * samplingRate;

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
			end

		case 'pda'
			window_size_in_sec = 5;
			window_size = round(window_size_in_sec * samplingRate);

			for i = 1 : length(signals)
				time_series = signals{i}.timeseries;
				freq_pulse = signals{i}.freq_pulse;

				max_freq = freq_pulse * 2;
				minPeakDistance = max(round(samplingRate / max_freq), 1);

				[peaks, ~, ~] = beat_counter_pda(time_series, samplingRate, 1, window_size, 0, minPeakDistance, 0);
				peaks = peaks(:, [2 1]);

				measured_pulse = size(peaks, 1) / length(time_series) * samplingRate;

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
				signals{i}.measured_pulse = measured_pulse;
				signals{i}.hrv = hrv;
			end;

		case 'autocorr'
			window_size_in_sec = 5;
			window_size = round(window_size_in_sec * samplingRate);

			for i = 1 : length(signals)
				time_series = signals{i}.timeseries;
				freq_pulse = signals{i}.freq_pulse;

				max_freq = freq_pulse * 2;
				minPeakDistance = max(round(samplingRate / max_freq), 1);

				[peaks, ~, ~] = beat_counter_autocorr(time_series, samplingRate, 1, window_size, 0, minPeakDistance, 0);
				peaks = peaks(:, [2 1]);

				measured_pulse = size(peaks, 1) / length(time_series) * samplingRate;

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
				signals{i}.measured_pulse = measured_pulse;
				signals{i}.hrv = hrv;
			end;
end