function [roi_streams, frameRate] = mit_select_region_frames(vidFolder, roi_params, forced_selection, cv_package)
    % Constants
    SETTLEMENT_TIME = 0.5; %seconds
    %SETTLEMENT_TIME = 5*rand; %seconds
    nChannels = 3;
    addpath(genpath('../../../tools'))

    % Load the frames
    vid = frame_loader(vidFolder, [], [1 : nChannels], 'png'); % Double array

    % Extract video constants
    vidHeight = size(vid, 1);
    vidWidth = size(vid, 2);
    vidLen = size(vid, 4);
    if ~exist(fullfile(vidFolder, 'vid_specs.txt'))
        error('No vid_specs.txt file to get frame rate from.')
    end
    frameRate = textscan(fopen([vidFolder '/vid_specs.txt']), '%d, %d');
    frameRate = double(frameRate{2});

    first_frame_index = max(round(frameRate * SETTLEMENT_TIME), 1);

    % Calculate the ROIs from the first significant frame
    [roi_masks, full_masks] = mit_select_region_frame(uint8(vid(:, :, :, first_frame_index)), ...
                                                      roi_params, forced_selection, cv_package);

    % Calculate the array dimension that represents time
    time_dim = length(size(vid(:, :, :, first_frame_index))) + 1;
    
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
                roi = uint8(bsxfun(@times, full_mask, vid(:, :, :, frame_index)));
                roi_stream = cat(time_dim, roi_stream, imcrop(roi, roi_mask));

                frame_index = frame_index + 1;
            end

            roi_streams{j} = roi_stream;
        end
    else
        roi_streams = {};
    end
end