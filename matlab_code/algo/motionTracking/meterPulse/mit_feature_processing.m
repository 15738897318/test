function features_pos = mit_feature_processing(features_pos, freq_params)
	for i = 1 : length(features_pos)
		% Calculate the maximum y-direction movement (rounded to nearest pixels) between frames
		diff_y = zeros(size(features_pos{i}));
		for j = 1 : length(features_pos{i})
			diff_y(j) = max(round(abs(diff(features_pos{i}{j}(:, 2)))));
		end

		% Find the mode of the distance distribution
		temp = min(diff_y);
		[m, k] = max(hist(diff_y, max(diff_y) - temp + 1));
		diff_y_mode = temp + k - 1; % to adjust for 1-based indexing

		% Retain only feature points whose movements are no larger than the mode
		accepted_feature_indices = find(diff_y <= diff_y_mode);
		temp = features_pos{i};
		features_pos{i} = {};
		for j = 1 : length(accepted_feature_indices)
			features_pos{i}{j} = temp{accepted_feature_indices(j)};
		end

		% Upsampling to ECG freq of 250Hz
		for j = 1 : length(features_pos{i})
			temp = spline([1 : 1 : size(features_pos{i}{j}, 1)], ...
										shiftdim(features_pos{i}{j}, length(size(features_pos{i}{j})) - 1), ...
										[1 : (freq_params.original_freq / freq_params.new_freq) : size(features_pos{i}{j}, 1)]);
			features_pos{i}{j} = shiftdim(temp, length(size(temp)) - 1);
		end

		% Retain only the y-direction of movement
		for j = 1 : length(features_pos{i})
			features_pos{i}{j} = features_pos{i}{j}(:, 2);
		end
	end
end