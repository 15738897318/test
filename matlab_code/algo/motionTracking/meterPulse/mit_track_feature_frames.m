function [features_pos] = mit_track_feature_video(vid_array, features_def, cv_package)    
    vid_array_sz = size(vid_array);
    time_dim = length(vid_array_sz);

    % Create a text string to pad the slicing operation later
    % in order to allow different array dimensions representing time
    dim_slice_pad = '';
    for i = 1 : time_dim - 1
        dim_slice_pad = [dim_slice_pad, ':,'];
    end

    % Get frame 1 as the reference frame
    refframe = eval(['vid_array', '(', dim_slice_pad, '1', ')']);

    % Perform optical-flow calculation using Lucas-Kanade method for each frame compared with frame 1
    pos_array = {};
    for i = 1 : vid_array_sz(time_dim)
        % Get the current frame
        currframe = eval(['vid_array', '(', dim_slice_pad, num2str(i), ')']);

        % Perform optical-flow calculation for current frame
        currpos = mit_track_feature_frame(currframe, refframe, features_def, cv_package);

        % Concatenate into result array
        pos_array = cat(1, pos_array, currpos);
    end

    features_pos = {};
    for i = 1 : size(pos_array, 2)
        feature_pos = cell2mat(pos_array(:, i));
        feature_pos = bsxfun(@minus, feature_pos, feature_pos(1, :)); 
        features_pos = cat(2, features_pos, {feature_pos});
    end

    % Plot first frame with markers
    figure();
    imshow(refframe);
    hold('on');
    for i = 1 : size(features_pos, 2)
        scatter(features_def{i}(1, 1), features_def{i}(1, 2), 'r+');
    end
    hold('off');
end