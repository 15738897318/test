function signals = own_signal_selection(features_pos, samplingRate, method)
	RETAINED_PORTION_4_PCA = 0.75;

	signals = cell(size(features_pos));
	for i = 1 : length(features_pos)
		%% === Perform PCA decomposition to project the feature movements into orthogonal space (Section 3.3 in paper)
		% Define the data to include in PCA calculation
		original_features = cell2mat(features_pos{i});
		l2_norms = sqrt(sum(original_features.^2, 2));
		l2_threshold = prctile(l2_norms, RETAINED_PORTION_4_PCA * 100);
		reduced_features = original_features(l2_norms <= l2_threshold, :);

		switch method
		case 'pca'
			% Calculate the PCA coefficients
			transform_coeffs = pca(reduced_features);
			
			% Transform the features
			transformed_features = original_features * transform_coeffs;

		case 'ica'
			% FastICA function has input/output in the form of NumOfSamples*NumOfSignals
			transformed_features = fastica(reduced_features')';
		end


		%% === Select signal based on periodicity (Section 3.4 in paper)
		% Calculate the power spectra for the features up to Nyquist frequency
		power_spectra = abs(fft(transformed_features));
		power_spectra = power_spectra(1 : ceil(size(power_spectra, 1) / 2), :);

		% Identify the periodicity as ratio of power contained in freq with max power & its 1st harmonic
		periodicity_scores = zeros(1, size(power_spectra, 2));
		[max_power, max_freq] = max(power_spectra, [], 1);
		for j = 1 : length(max_freq)
			periodicity_scores(j) = (max_power(j) + power_spectra((max_freq(j) - 1) * 2 + 1, j)) / sum(power_spectra(:, j), 1);
		end

		% Select the signal with the highest periodicity score
		[~, signal_index] = max(periodicity_scores);
		signals{i}.timeseries = transformed_features(:, signal_index);
		signals{i}.freq_pulse = max_freq(signal_index) / length(signals{i}.timeseries) * samplingRate;
	end
end