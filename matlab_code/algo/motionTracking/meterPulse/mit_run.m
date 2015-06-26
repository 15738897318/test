vidFolder = './test_data/frames/self1';
vidFile = './test_data/videos/test.avi';


% ROI def:
% cell(1): upper left and bottom right corner of outer box as ratio of the full face-detection rectangle
% cell(2): upper left and bottom right corner of outer box as ratio of the rectangle defined in cell(1)
roi_params = {
			  {[0.25, 0], [0.75, 0.9]}, ...
			  {[0, 0.2], [1, 0.55]}
			 };
forced_selection = false;

% Feature-marker def:
NUMBER_OF_FEATURE_MARKERS_X = 10; % Number of markers in x dimension
NUMBER_OF_FEATURE_MARKERS_Y = 10; % Number of markers in y dimension

% Upsampling
ECG_freq = 250; % Hz

% Temporal filter def:
filter_specs.family = 'butterworth';
filter_specs.type = 'bandpass';
filter_specs.order = 5;
filter_specs.freq = [0.75, 5]; % Hz

% Function handles
func_region_selection = @mit_select_region_video;
func_feature_tracking = @mit_track_feature_frames;
func_feature_processing = @mit_feature_processing;
func_temporal_filtering = @mit_temporal_filtering;
func_signal_selection = @mit_signal_selection;
func_peak_detection = @mit_peak_detection;

cv_package = 'opencv'; % 'native'


%% % ===== Step 1: Region selection and Tracking (Section 3.1 in paper)
%% === Step 1.1: Separate the video into independent masked face-only streams
vid = VideoReader(vidFile);

func_region_selection = @mit_select_region_frames;
vid = vidFolder;
forced_selection = true;

[roi_streams, frameRate] = func_region_selection(vid, roi_params, forced_selection, cv_package);


%% === Step 1.2: Track features in the face-only streams
% Define and only retain feature markers in the meaningful region
linspace_x = linspace(0, 1, NUMBER_OF_FEATURE_MARKERS_X + 1);
linspace_y = linspace(0, 1, NUMBER_OF_FEATURE_MARKERS_Y + 1);
[features_mesh_x, features_mesh_y] = meshgrid(linspace_x(1 : NUMBER_OF_FEATURE_MARKERS_X) + ...
												1/2 * (linspace_x(NUMBER_OF_FEATURE_MARKERS_X) - linspace_x(NUMBER_OF_FEATURE_MARKERS_X - 1)), ...
											  linspace_y(1 : NUMBER_OF_FEATURE_MARKERS_Y) + ...
												1/2 * (linspace_y(NUMBER_OF_FEATURE_MARKERS_Y) - linspace_y(NUMBER_OF_FEATURE_MARKERS_Y - 1)));
features_mesh(:, 1) = features_mesh_x(:);
features_mesh(:, 2) = features_mesh_y(:);

features_def = {};
for i = 1 : size(features_mesh, 1)
	if (features_mesh(i, 1) >= roi_params{2}{1}(1)) && (features_mesh(i, 1) <= roi_params{2}{2}(1))
		if (features_mesh(i, 2) >= roi_params{2}{1}(2)) && (features_mesh(i, 2) <= roi_params{2}{2}(2))
			continue;
		end
	end
	features_def = cat(2, features_def, {features_mesh(i, :)});
end

% Perform tracking
features_pos = cell(size(roi_streams));
if numel(roi_streams) > 0
	% For each such stream, find the time-wise positions of the feature markers
	for i = 1 : length(roi_streams)
		features_loc = {};
		for j = 1 : length(features_def)
			% Convert feature defition from ratios to pixel positions
			feature_loc = ceil(features_def{j} .* [size(roi_streams{i}, 2), size(roi_streams{i}, 1)]);
			feature_loc(feature_loc == 0) = 1;

			features_loc = cat(2, features_loc, feature_loc);
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
	signals = func_signal_selection(features_pos_filt, samplingRate);
end

%% % ===== Step 4: Peak detection (Section 3.5 in paper)
if numel(signals) > 0
	signals = func_peak_detection(signals, samplingRate);
end

