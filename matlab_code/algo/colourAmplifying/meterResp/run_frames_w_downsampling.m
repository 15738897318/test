vidFolders = {'/Users/misfit/Desktop/Codes - Local/Active/bioSignal/Data/devData/testData/Set1/testData4/original';...
			  '/Users/misfit/Desktop/Codes - Local/Active/bioSignal/Data/devData/testData/Set1/testData4/removal'};

frame_rates_as_multiplier = 1; 0.5 : 0.025 : 1;
frame_sizes_as_multiplier = 1; 0.5: 0.01 : 1;

% Load envs & constants
initialiser;

global full_fr full_vidHeight full_vidWidth

hr_arrays = {};
for k = 1 : size(vidFolders)
	current_vidFolder = vidFolders{k};
	display(sprintf('Processing folder: %s', current_vidFolder));
    
	if exist([current_vidFolder '/ref_pulse.txt'])
		ref_pulse = textscan(fopen([current_vidFolder '/ref_pulse.txt']), '%f');
		ref_pulse = ref_pulse{1};
	else
		ref_pulse = 75;
	end
	
	hr_array = [];
	for i = 1 : length(frame_rates_as_multiplier)
		for j = 1 : length(frame_sizes_as_multiplier)
			if ~strcmpi(pyramid_style, 'none')
                func_magnify_pyr(current_vidFolder, ...
                                alpha, pyr_level, ...
                                min_hr/60, max_hr/60, ...
                                chroma_magnifier, ...
                                frame_rates_as_multiplier(i), frame_sizes_as_multiplier(j));

                frameFolder_for_hr = fullfile(current_vidFolder, 'out');
            else
                frameFolder_for_hr = current_vidFolder;
            end

            temp_hr_array = func_rate_calc(frameFolder_for_hr, ...
            									window_size_in_sec, overlap_ratio, ...
            									max_bpm, cutoff_freq, ...
            									channel_to_process, ref_pulse, ...
            									colourspace, time_lag);
			hr_array = [hr_array; temp_hr_array];
		end
	end
		
	hr_arrays{k} = hr_array;
end

save([vidFolders{1} '/results.mat']);

figure();
lineSpecs = {'r-*', 'b-^'};
if (length(frame_rates_as_multiplier) > 1) && (length(frame_sizes_as_multiplier) > 1)
	for k = 1 : size(vidFolders)		
		subplot(2, 1, 1);
		hold('on');
		temp = reshape(hr_arrays{k}(:, 3), length(frame_sizes_as_multiplier), length(frame_rates_as_multiplier));
		surface(full_fr * frame_rates_as_multiplier, ...
			full_vidHeight * frame_sizes_as_multiplier, ...
			temp, ...
			'DisplayName', vidFolders{k});
		zlabel('HR (BPM)');

		subplot(2, 1, 2);
		hold('on');
		temp = reshape(100 * hr_arrays{k}(:, 3) / hr_arrays{k}(end, 3), length(frame_sizes_as_multiplier), length(frame_rates_as_multiplier));
		surface(full_fr * frame_rates_as_multiplier, ...
				full_vidHeight * frame_sizes_as_multiplier, ...
				temp, ...
				'DisplayName', vidFolders{k});
		zlabel('%age of HR at full frame-rate');
	end
	xlabel('Nominal frame-rates (FPS)');
	ylabel('Frame-sizes (px)');
else
	if length(frame_rates_as_multiplier) > 1
		for k = 1 : size(vidFolders)
			subplot(2, 1, 1);
			hold('on');
			plot(full_fr * frame_rates_as_multiplier, hr_arrays{k}(:, 3), lineSpecs{k}, 'DisplayName', vidFolders{k});
			ylabel('HR (BPM)');
	
			subplot(2, 1, 2);
			hold('on');
			plot(full_fr * frame_rates_as_multiplier, 100 * hr_arrays{k}(:, 3) / hr_arrays{k}(end, 3), lineSpecs{k}, 'DisplayName', vidFolders{k});
			ylabel('%age of HR at full frame-rate');
		end
		xlabel('Nominal frame-rates (FPS)');
	else
		for k = 1 : size(vidFolders)
			subplot(2, 1, 1);
			hold('on');
			plot(full_vidHeight * frame_sizes_as_multiplier, hr_arrays{k}(:, 3), lineSpecs{k}, 'DisplayName', vidFolders{k});
			ylabel('HR (BPM)');
	
			subplot(2, 1, 2);
			hold('on');
			plot(full_vidHeight * frame_sizes_as_multiplier, 100 * hr_arrays{k}(:, 3) / hr_arrays{k}(end, 3), lineSpecs{k}, 'DisplayName', vidFolders{k});
			ylabel('%age of HR at full frame-rate');
		end
		xlabel('Frame-sizes (px)');
	end
end

figure();
boxplot(100 * hr_arrays{2}(:, 3) ./ hr_arrays{1}(:, 3));
ylabel('HR ratio for face-with-removal to full-face');
legend;