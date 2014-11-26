clear all
vidFolder = '/Users/misfit/Desktop/Codes - Local/Code - Active/bioSignalProcessing/eulerianMagnifcation/codeMatlab/testData/Set2';
template_to_include = 'Face';

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

initialiser;

params_set = allcomb(alphas, pyr_levels, frame_rates_as_multiplier, frame_sizes_as_multiplier);
	
global full_fr full_vidHeight full_vidWidth

hr_arrays = {};
for k = 1 : length(vidFolders)
	current_vidFolder = vidFolders{k};
	display(sprintf('Processing folder: %s', current_vidFolder));
    
	if exist([current_vidFolder '/ref_pulse.txt'])
		ref_pulse = textscan(fopen([current_vidFolder '/ref_pulse.txt']), '%s');
		ref_pulse = str2num(ref_pulse{1}{end});
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
        
		func_magnify_pyr(current_vidFolder, ...
						alpha, pyr_level, ...
						min_hr/60, max_hr/60, ...
						chroma_magnifier, ...
						frame_rate_as_multiplier, frame_size_as_multiplier);
	
		temp_hr_array = func_heartRate_calc(fullfile(current_vidFolder, 'out'), window_size_in_sec, overlap_ratio, max_bpm, cutoff_freq, 2, ref_pulse, 'tsl', time_lag);
		hr_array = [hr_array; temp_hr_array];
	end
		
	hr_arrays{k, 1} = hr_array;
	hr_arrays{k, 2} = current_vidFolder;
    
    save(strcat(vidFolder, '/results_', template_to_include, '.mat'), 'hr_arrays', 'params_set');
end