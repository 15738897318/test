% Load envs & constants
initialiser;

resultsDir = 'Results';
src_folder = '/Users/misfit/Desktop/Codes - Local/Active/bioSignal/Codes/main/refMIT/vidData/';
file_template = 'baby2.mp4';
file_template = 'baby.mp4';


% src_folder = '/Users/misfit/Desktop/Codes - Local/Active/bioSignal/Data/';
% file_template = 'manual_20150201225543.m4v';

file_list = dir(fullfile(src_folder, file_template));

resultsDir = fullfile(src_folder, resultsDir);
if ~exist(resultsDir, 'dir')
    mkdir(resultsDir);
end

for file_ind = 1 : length(file_list)
	inFile = fullfile(src_folder, file_list(file_ind).name);
	
	display(sprintf('File %d of %d \n', file_ind, length(file_list)));
	display(sprintf('Processing file: %s', inFile));
	
	params_set = allcomb(alpha, pyr_level, min_hr, max_hr, frame_rate, chroma_magnifier);
	for params_ind = 1 : size(params_set, 1)
		curr_combo = params_set(params_ind, :);
		
		curr_alpha = curr_combo(1);
		curr_pyr_level = curr_combo(2);
		curr_min_hr = curr_combo(3);
		curr_max_hr = curr_combo(4);
		curr_frame_rate = curr_combo(5);
		curr_chroma_magnifier = curr_combo(6);
		display(sprintf('Param set %d of %d: %d, %d, %d, %d, %d, %d', params_ind, size(params_set, 1), ...
											curr_alpha, curr_pyr_level, ...
											curr_min_hr, curr_max_hr, ...
											curr_frame_rate, curr_chroma_magnifier));
		
		func_magnify_pyr(inFile, resultsDir, ...
						curr_alpha, curr_pyr_level, ...
						curr_min_hr/60, curr_max_hr/60, ...
						curr_frame_rate, curr_chroma_magnifier);
	end
end