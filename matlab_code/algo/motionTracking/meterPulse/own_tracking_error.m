multiple_vids = false;

vid_as_frames = true;
vidSource = './test_data/markers/test5-crop';

% vid_as_frames = false;
% vidSource = './test_data/videos/test.avi';


% ROI def:
% cell(1): upper left and bottom right corner of outer box as ratio of the full face-detection rectangle
% cell(2): upper left and bottom right corner of outer box as ratio of the rectangle defined in cell(1)
roi_params = {
              {[0, 0], [1, 1]}
%             {[0.15, 0], [0.85, 0.2]}
% 			  {[0.25, 0], [0.75, 0.9]}, ...
% 			  {[0, 0.2], [1, 0.55]}
			 };
forced_selection = true;

% Feature-marker def:
NUMBER_OF_FEATURE_MARKERS_X = 0; % Number of markers in x dimension
NUMBER_OF_FEATURE_MARKERS_Y = 0; % Number of markers in y dimension

% Function handles
if vid_as_frames
	func_region_selection = @mit_select_region_frames;
else
	func_region_selection = @mit_select_region_video;
end
func_feature_tracking = @own_track_feature_frames;


forced_region_selection = true;
cv_package = 'opencv'; % 'native'

%% %% ====== Load video source
if multiple_vids
	vid_list = dir(vidSource);
	vid_list = {vid_list.name};
	index = ismember(vid_list, {'.', '..', '.DS_Store'});
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
end