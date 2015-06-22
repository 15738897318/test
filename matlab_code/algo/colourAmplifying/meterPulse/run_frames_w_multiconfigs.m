vidFolder = '~/Desktop/Codes - Local/Active/bioSignal/Data/fieldData/fieldData-Processed';
templates_to_include = {'Finger', 'Face'};

for template_ind = 1 : length(templates_to_include)
	template_to_include = templates_to_include{template_ind};
	
	vidFolders = dir(vidFolder);
	vidFolders = {vidFolders([vidFolders(:).isdir]).name};
	vidFolders(ismember(vidFolders, {'.', '..'})) = [];

	for i = 1 : length(vidFolders)
		vidFolders{i} = fullfile(vidFolder, vidFolders{i});
	end

	nomatches = [];
	for i = 1 : length(vidFolders)
		if isempty(strfind(vidFolders{i}, template_to_include))
			nomatches = [nomatches, i];
		end
	end
	vidFolders(nomatches) = [];

	frame_rates_as_multiplier = 1; 0.5 : 0.025 : 1;
	frame_sizes_as_multiplier = 1; 0.5: 0.01 : 1;

	% Load envs & constants
	initialiser;

	params_set = allcomb(alphas, pyr_levels, frame_rates_as_multiplier, frame_sizes_as_multiplier);
	
	global full_fr full_vidHeight full_vidWidth
	
	data_point = 0;
	hr_arrays = {};
	iter_limit = 1; max(1, length(vidFolders) - 100 + 1); %Limit the number of loops
	for k = iter_limit : length(vidFolders)
		current_vidFolder = vidFolders{k};
        data_point = data_point + 1;
		display(sprintf('Processing folder %d of %d: %s', data_point, length(vidFolders) - iter_limit + 1, current_vidFolder));
	
		if exist([current_vidFolder '/ref_pulse.txt'])
			ref_pulse = textscan(fopen([current_vidFolder '/ref_pulse.txt']), '%s');
			ref_pulse = str2double(ref_pulse{1}{end});
		else
			ref_pulse = 75;
		end
	
		hr_array = [];
		for params_ind = 1 : size(params_set, 1)
			curr_combo = params_set(params_ind, :);
	
			alpha = curr_combo(1);
			pyr_level = curr_combo(2);
			frame_rate_as_multiplier = curr_combo(3);
			frame_size_as_multiplier = curr_combo(4);
		
			display(sprintf('Param set %d of %d: %d, %d, %d, %d', ...
								params_ind, size(params_set, 1), ...
								alpha, pyr_level, ...
								frame_rate_as_multiplier, frame_size_as_multiplier));
		
			if ~strcmpi(pyramid_style, 'none')
				func_magnify_pyr(current_vidFolder, ...
								alpha, pyr_level, ...
								min_hr/60, max_hr/60, ...
								chroma_magnifier, ...
								channel_to_process, ...
								frame_rate_as_multiplier, frame_size_as_multiplier,...
								'png', 'mat');
			
				frameFolder_for_hr = fullfile(current_vidFolder, 'out');
				hr_in_filetype = 'mat';
			else
				frameFolder_for_hr = current_vidFolder;
				hr_in_filetype = 'png';
			end
	
			temp_hr_array = func_rate_calc(frameFolder_for_hr, hr_in_filetype, ...
												window_size_in_sec, overlap_ratio, ...
												max_bpm, cutoff_freq, ...
												channel_to_process, ref_pulse, ...
												colourspace, time_lag);
			hr_array = [hr_array; temp_hr_array];
		
			if ~strcmpi(frameFolder_for_hr, current_vidFolder)
				rmdir(frameFolder_for_hr, 's');
			end
		end
		
		hr_arrays{k, 1} = hr_array;
		hr_arrays{k, 2} = current_vidFolder;
	
		save(strcat(vidFolder, '/results_', pyramid_style, '_', template_to_include, '.mat'), 'hr_arrays', 'params_set');
	end
end