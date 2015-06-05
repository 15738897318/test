function features_pos = mit_temporal_filtering(features_pos, samplingRate, filter_specs)
	% Filter design
	switch filter_specs.family
		case 'butterworth'
			switch filter_specs.type
				case {'bandpass', 'stop'}
					filter_order = filter_specs.order / 2;
				otherwise
					filter_order = filter_specs.order;
			end		

			[z, p, k] = butter(filter_specs.order, filter_specs.freq * 2 / samplingRate, filter_specs.type);
			[b, a] = zp2tf(z, p, k);
	end
		
	% Filter the feature positions along the time dimension
	for i = 1 : length(features_pos)
		for j = 1 : length(features_pos{i})
			features_pos{i}{j} = filter(b, a, features_pos{i}{j});
		end
	end
end