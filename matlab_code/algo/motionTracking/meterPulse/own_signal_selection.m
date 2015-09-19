function signals = own_signal_selection(features_pos, samplingRate, method, freq_range)
	RETAINED_PORTION_4_COMPONENT_ANALYSIS = 0.75;

	signals = cell(size(features_pos));
	for i = 1 : length(features_pos)
		%% === Perform PCA decomposition to project the feature movements into orthogonal space (Section 3.3 in paper)
		% Define the data to include in component-analysis calculation
		original_features = cell2mat(features_pos{i});
		l2_norms = sqrt(sum(original_features.^2, 2));
		l2_threshold = prctile(l2_norms, RETAINED_PORTION_4_COMPONENT_ANALYSIS * 100);
		reduced_features = original_features(l2_norms <= l2_threshold, :);

		switch method
			case 'pca'
				% Calculate the PCA coefficients
				transform_coeffs = pca(reduced_features);
				
				% Transform the features
				transformed_features = original_features * transform_coeffs;

			case 'ica'
				% Re-intialise the random-number generator every time
				rng(2^10);

				% FastICA function has input/output in the form of NumOfSamples*NumOfSignals
				transformed_features = fastica(reduced_features', 'verbose', 'on')';

			% case 'sc'
			% 	% sparse coding parameters
			% 	num_bases = 2^ceil(log(size(reduced_features, 2)) / log(2));
			% 	beta = 0.4;
			% 	batch_size = 1000;
			% 	num_iters = 100;
			%     sparsity_func = 'L1';
			%     epsilon = [];

			% 	[B, S, stat] = sparse_coding(reduced_features, num_bases, beta, sparsity_func, epsilon, num_iters, batch_size);
		end


		%% === Select signal based on periodicity (Section 3.4 in paper)
		% Calculate the power spectra for the features up to Nyquist frequency, but not DC
		power_spectra = abs(fft(transformed_features));
		power_spectra = power_spectra(2 : ceil(size(power_spectra, 1) / 2), :);

		% Identify the periodicity as ratio of power contained in freq with max power & its 1st harmonic
		periodicity_scores = zeros(1, size(power_spectra, 2));
		[max_power, max_freq] = max(power_spectra, [], 1);
		for j = 1 : length(max_freq)
			periodicity_scores(j) = (max_power(j) + power_spectra((max_freq(j) - 1) * 2 + 1, j)) / sum(power_spectra(:, j), 1);
		end
		max_freq = max_freq / size(transformed_features, 1) * samplingRate;

		% Select the signal with the highest periodicity score whose peak falls within the freq range
        eligible_ind = (max_freq >= freq_range(1)) & (max_freq <= freq_range(2));
        max_freq = max_freq(eligible_ind);
        periodicity_scores = periodicity_scores(eligible_ind);
        transformed_features = transformed_features(:, eligible_ind);

		[~, signal_index] = max(periodicity_scores);
		signals{i}.timeseries = transformed_features(:, signal_index);
		signals{i}.freq_pulse = max_freq(signal_index);
	end
end