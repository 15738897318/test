function [roi_streams, frameRate] = mit_select_region_video(vid, roi_params, forced_selection, cv_package)
    % Constants
    SETTLEMENT_TIME = 0.5; %seconds

    % Extract video constants
    vidHeight = vid.Height;
    vidWidth = vid.Width;
    nChannels = 3;
    vidLen = vid.NumberOfFrames;
    frameRate = vid.FrameRate;

    first_frame_index = max(round(frameRate * SETTLEMENT_TIME), 1);

    % Create the placeholder for a single movie-frame (which has to have both cdata & colormap, per Matlab def)
    temp = struct('cdata', zeros(vidHeight, vidWidth, nChannels, 'uint8'), ...
                  'colormap', []);

    % Calculate the ROIs from the first significant frame
    temp.cdata = read(vid, first_frame_index);
    [roi_masks, full_masks] = mit_select_region_frame(frame2im(temp), roi_params, forced_selection, cv_package);

    % Calculate the array dimension that represents time
    time_dim = length(size(frame2im(temp))) + 1;
    
    % Holder variable for the ROIs
    if numel(full_masks) > 0
        roi_streams = cell(size(full_masks));

        % Separate ROIs in each frame into their own streams (represented as 3-D arrays)
        % Extract the ROI for each face
        for j = 1 : length(full_masks)
            full_mask = full_masks{j};
            roi_mask = roi_masks{j, 1};

            roi_stream = [];
            frame_index = first_frame_index;
            for i = 0 : vidLen - first_frame_index
                % Extract the ith frame in the video stream
                temp.cdata = read(vid, frame_index);
                % Convert the extracted frame to RGB image
                [rgbframe, ~] = frame2im(temp);

                roi = uint8(bsxfun(@times, full_mask, double(rgbframe)));
                roi_stream = cat(time_dim, roi_stream, imcrop(roi, roi_mask));

                frame_index = frame_index + 1;
            end

            roi_streams{j} = roi_stream;
        end
    else
        roi_streams = {};
    end
end