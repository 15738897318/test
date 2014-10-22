vidFolders = {'/Users/misfit/Desktop/Codes - Local/Code - Active/bioSignalProcessing/eulerianMagnifcation/codeMatlab/pulsarTestData/original';...
			  '/Users/misfit/Desktop/Codes - Local/Code - Active/bioSignalProcessing/eulerianMagnifcation/codeMatlab/pulsarTestData/removal'};
				
frame_rates_as_multiplier = 0.1 : 0.025 : 1;
frame_sizes_as_multiplier = 1; 0.1: 0.01 : 1;

constants;

global full_fr full_vidHeight full_vidWidth

hr_arrays = {};
for k = 1 : size(vidFolders)
	vidFolder = vidFolders{k};
	
	hr_array = [];
	for i = 1 : length(frame_rates_as_multiplier)
		for j = 1 : length(frame_sizes_as_multiplier)
			eulerianGaussianPyramidMagnification_frames(vidFolder, ...
														alpha, pyr_level, ...
														min_hr/60, max_hr/60, ...
														chroma_magnifier, ...
														frame_rates_as_multiplier(i), frame_sizes_as_multiplier(j));
	
			temp_hr_array = heartRate_calc_frames(fullfile(vidFolder, 'out'), window_size_in_sec, overlap_ratio, max_bpm, cutoff_freq, 2, 75, 'tsl', time_lag);
			hr_array = [hr_array; temp_hr_array];
		end
	end
	
	figure();
	plot(full_fr * frame_rates_as_multiplier, 100 * hr_array(:, 3) / hr_array(end, 3), '-*');
	ylabel('%age of HR at full frame-rate');
	xlabel('Nominal frame-rates (FPS)');
	
	hr_arrays{k} = hr_array;
end

figure();
hold('on');
lineSpecs = {'r-*', 'b-^'};
if length(frame_rates_as_multiplier) > 1
	for k = 1 : size(vidFolders)
		plot(full_fr * frame_rates_as_multiplier, 100 * hr_arrays{k}(:, 3) / hr_arrays{k}(end, 3), lineSpecs{k}, 'DisplayName', vidFolders{k});
	end
	xlabel('Nominal frame-rates (FPS)');
else
	for k = 1 : size(vidFolders)
		plot(full_vidHeight * frame_sizes_as_multiplier, 100 * hr_arrays{k}(:, 3) / hr_arrays{k}(end, 3), lineSpecs{k}, 'DisplayName', vidFolders{k});
	end
	xlabel('Frame-sizes (px)');
end
hold('off');
ylabel('%age of HR at full frame-rate');
legend;


figure();
boxplot(100 * (hr_arrays{2}(:, 3) / hr_arrays{2}(end, 3)) ./ (hr_arrays{1}(:, 3) / hr_arrays{1}(end, 3)));
ylabel('HR ratio for face-with-removal to full-face');
legend;