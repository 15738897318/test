%install;

% Load constants
constants;


%alpha = 30; 50 : 30 : 80; %Eulerian magnifier %Standard: < 50
%pyr_level = 6; 4 : 2 : 6; %Standard: 4, but updated by the real frame size
%min_hr = 30; 30 : 10 : 50; %BPM %Standard: 30
%max_hr = 300; 60 : 30 : 210; %BPM %Standard: > 150
%frame_rate = 30; %Standard: 30, but updated by the real frame-rate
%chroma_magnifier = 1; 1 : 2; %Standard: 1

resultsDir = 'Results';
src_folder = '/Users/misfit/Desktop/Codes - Local/Working bench/bioSignalProcessing/eulerianMagnifcation/codeMatlab/';
file_template = '2014-06-10-Self-Face_crop.mp4';
%file_template = 'finger - 2014-07-11-14-41-54*.mp4';

file_list = dir([src_folder file_template]);

for file_ind = 1 : length(file_list)
	inFile = [src_folder file_list(file_ind).name];
	
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
		
		%eulerianGaussianPyramidMagnification(inFile, resultsDir, ...									
		eulerianGaussianPyramidMagnification_rev(inFile, resultsDir, ...
											curr_alpha, curr_pyr_level, ...
											curr_min_hr/60, curr_max_hr/60, ...
											curr_frame_rate, curr_chroma_magnifier);
	end
end