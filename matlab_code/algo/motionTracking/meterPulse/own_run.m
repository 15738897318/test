multiple_vids = true;

vid_as_frames = true;
vidSource = '~/Desktop/Codes - Local/Active/bioSignal/Data/motionAmp/frames-refpulsox/';
% vidSource = '~/Desktop/Codes - Local/Active/bioSignal/Data/motionAmp/remnant/';

% vid_as_frames = false;
% vidSource = './test_data/videos/test.avi';


% ROI def:
% cell(1): upper left and bottom right corner of outer box as ratio of the full face-detection rectangle
% cell(2): upper left and bottom right corner of outer box as ratio of the rectangle defined in cell(1)
roi_params = {
                {[0.25, 0], [0.75, 0.9]}, ...  % Version 0 -- MIT paper
                {[0, 0.2], [1, 0.55]}
%                 {[0.15, 0], [0.85, 0.9]}, ...  % Version 1 -- Full face
%                 {[0, 0], [0, 0]}
%                 {[0.15, 0], [0.85, 0.2]},...   % Version 2 -- Forehead only
%                 {[0, 0], [0, 0]}
%                 {[0.15, 0.45], [0.85, 0.65]}, ... % Version 3 -- Mid-face only
%                 {[0, 0], [0, 0]}
			 };
forced_selection = false;

% Feature-marker def:
NUMBER_OF_FEATURE_MARKERS_X = 0; % Number of markers in x dimension
NUMBER_OF_FEATURE_MARKERS_Y = 10; % Number of markers in y dimension

% Upsampling
ECG_freq = 250; % Hz

% Temporal filter def:
filter_specs.family = 'butterworth';
filter_specs.type = 'bandpass';
filter_specs.order = 5;
filter_specs.freq = [0.75, 5]; % Hz

% Function handles
if vid_as_frames
	func_region_selection = @mit_select_region_frames;
else
	func_region_selection = @mit_select_region_video;
end
func_feature_tracking = @own_track_feature_frames;
func_feature_processing = @mit_feature_processing;
func_temporal_filtering = @mit_temporal_filtering;
func_signal_selection = @own_signal_selection;
func_peak_detection = @mit_peak_detection;

forced_region_selection = false;
cv_package = 'opencv'; % 'native'
signal_separation_method = 'ica';

%% %% ====== Load video source
if multiple_vids
	vid_list = dir(vidSource);
	is_folders = [vid_list(:).isdir];
	vid_list = {vid_list(is_folders).name};
	index = ismember(vid_list, {'.', '..', 'graphs'});
	vid_list(index) = [];

	vid_list = fullfile(vidSource, vid_list);
else
	vid_list = {vidSource};
end

results = cell(1, length(vid_list));
for index = 1 : length(vid_list)
	vid = vid_list{index};

	%% % ===== Step 1: Region selection and Tracking (Section 3.1 in paper)
	%% === Step 1.1: Separate the video into independent masked face-only streams
	if ~vid_as_frames
		vid = VideoReader(vid);
	end

	[roi_streams, frameRate] = func_region_selection(vid, roi_params, forced_region_selection, cv_package);


	%% === Step 1.2: Track features in the face-only streams
	% Define and only retain feature markers in the meaningful region
	features_def = {};
	if (NUMBER_OF_FEATURE_MARKERS_X > 0) && (NUMBER_OF_FEATURE_MARKERS_Y > 0)
		linspace_x = linspace(0, 1, NUMBER_OF_FEATURE_MARKERS_X + 1);
		linspace_y = linspace(0, 1, NUMBER_OF_FEATURE_MARKERS_Y + 1);
		[features_mesh_x, features_mesh_y] = meshgrid(linspace_x(1 : NUMBER_OF_FEATURE_MARKERS_X) + ...
														1/2 * (linspace_x(NUMBER_OF_FEATURE_MARKERS_X) - linspace_x(NUMBER_OF_FEATURE_MARKERS_X - 1)), ...
													  linspace_y(1 : NUMBER_OF_FEATURE_MARKERS_Y) + ...
														1/2 * (linspace_y(NUMBER_OF_FEATURE_MARKERS_Y) - linspace_y(NUMBER_OF_FEATURE_MARKERS_Y - 1)));
		features_mesh(:, 1) = features_mesh_x(:);
		features_mesh(:, 2) = features_mesh_y(:);

		for i = 1 : size(features_mesh, 1)
			if (features_mesh(i, 1) >= roi_params{2}{1}(1)) && (features_mesh(i, 1) <= roi_params{2}{2}(1))
				if (features_mesh(i, 2) >= roi_params{2}{1}(2)) && (features_mesh(i, 2) <= roi_params{2}{2}(2))
					continue;
				end
			end
			features_def = cat(2, features_def, {features_mesh(i, :)});
		end
	end

	% Perform tracking
	features_pos = cell(size(roi_streams));
	if numel(roi_streams) > 0
		% For each such stream, find the time-wise positions of the feature markers
		for i = 1 : length(roi_streams)
			
			features_loc = {};
			if ~isempty(features_def)
				for j = 1 : length(features_def)
					% Convert feature defition from ratios to pixel positions
					feature_loc = ceil(features_def{j} .* [size(roi_streams{i}, 2), size(roi_streams{i}, 1)]);
					feature_loc(feature_loc == 0) = 1;

					features_loc = cat(2, features_loc, feature_loc);
				end
			end

			features_pos{i} = func_feature_tracking(roi_streams{i}, features_loc, cv_package);
		end
	end


	%% === Step 1.3: Post-processing of the optical-flow data based on Y-dimension optical-flow
	features_pos_proc = {};
	if numel(features_pos) > 0
		freq_params.original_freq = frameRate;
		freq_params.new_freq = ECG_freq;
		features_pos_proc = func_feature_processing(features_pos, freq_params);
	end


	%% % ===== Step 2: Temporal filtering (Section 3.2 in paper)
	features_pos_filt = {};
	if numel(features_pos_proc) > 0
		samplingRate = freq_params.new_freq;
		features_pos_filt = func_temporal_filtering(features_pos_proc, samplingRate, filter_specs);
	end


	%% % ===== Step 3: Signal selection (Section 3.3 & 3.4 in paper)
	signals = {};
	if numel(features_pos_filt) > 0
		signals = func_signal_selection(features_pos_filt, samplingRate, signal_separation_method);
		signals{:}.freq_params = freq_params;
	end

	%% % ===== Step 4: Peak detection (Section 3.5 in paper)
	if numel(signals) > 0
		signals = func_peak_detection(signals, samplingRate);
		signals{:}.source = vid_list{index};
		signals = signals{1};
	end

	%% % ===== Step 5: Store results
	if vid_as_frames
		if exist(fullfile(vid, 'ref_pulse.txt'))
	        ref_pulse = textscan(fopen(fullfile(vid, 'ref_pulse.txt')), '%s');
	        ref_pulse = ref_pulse{1};
	        ref_pulse = str2double(ref_pulse{length(ref_pulse)});
            signals.ref_pulse = ref_pulse;
	    end
	end

	if length(vid_list) > 1
		results{index} = signals;
	else
		results = signals;
	end

	if multiple_vids
		if isempty(features_def)
			feature_type = 'auto';
		else
			feature_type = 'regular';
		end
		result_file = ['results-' feature_type '-' signal_separation_method '-' cv_package];
		save(fullfile(vidSource, [result_file '.mat']), 'results');
	end
end

%% Plot the results
if multiple_vids
    close all
    
	results_struct = [results{:, :}];
	labels = {results_struct.source};
	for i = 1 : length(labels)
		temp = strsplit(labels{i}, '/');
		labels{i} = temp{end};
	end;
	
	plotBlandAltman([results_struct.freq_pulse] * 60, [results_struct.ref_pulse]);
	title(result_file);
	exportThisPlot('name', [result_file, '-BlAlt'], 'plotPath', fullfile(vidSource, 'graphs'));
	exportThisPlot('name', [result_file, '-BlAlt'], 'plotPath', fullfile(vidSource, 'graphs'), 'plotType', 'fig');

	figure();
	scatter([results_struct.ref_pulse], [results_struct.freq_pulse] * 60);
	text([results_struct.ref_pulse], [results_struct.freq_pulse] * 60, labels);
	refline([1 0]);
    title(result_file);
	xlabel('Reference - Average HR (BMP) from pulsox');
	ylabel('Heart-rate result of Pulsar (BMP)');
	exportThisPlot('name', result_file, 'plotPath', fullfile(vidSource, 'graphs'));
	exportThisPlot('name', result_file, 'plotPath', fullfile(vidSource, 'graphs'), 'plotType', 'fig');

	figure();
	bar([results_struct.freq_pulse] * 60 ./ [results_struct.ref_pulse] * 100 - 100);
	text([1 : length(results_struct)], [results_struct.freq_pulse] * 60 ./ [results_struct.ref_pulse] * 100 - 100, labels);
    title(result_file);
	ylabel('Difference between Pulsar & pulsox HRs (%)');
	exportThisPlot('name', [result_file, '-Rel'], 'plotPath', fullfile(vidSource, 'graphs'));
	exportThisPlot('name', [result_file, '-Rel'], 'plotPath', fullfile(vidSource, 'graphs'), 'plotType', 'fig');
end
