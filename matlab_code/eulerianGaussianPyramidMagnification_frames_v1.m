% eulerianGaussianPyramidMagnification(vidFile, outDir, alpha, 
%                                      level, freq_band_low_end, freq_band_high_end, samplingRate, 
%                                      chromAttenuation)
% Based on code by Hao-yu Wu, Michael Rubinstein, Eugene Shih (June 2012)
%
% Spatial Filtering: Gaussian blur and down sample
% Temporal Filtering: 15-tap FIR
% 
%
function eulerianGaussianPyramidMagnification_frames(vidFolder, ...
													alpha, level, ...
													freq_band_low_end, freq_band_high_end, ...
													chromAttenuation, ...
													new_fr, new_size)
    %Load constants
    initialiser;
										   
    % Read video
	vid = frameloader(vidFolder); % Double array
	
	global full_fr full_vidHeight full_vidWidth
	full_vidHeight = size(vid, 1);
	full_vidWidth = size(vid, 2);
	full_fr = size(vid, 4) / recordingTime; %Double
	
	% Re-sample & resize the video
	vid = frame_interpolater(vid, new_fr, new_size);

	% Extract video info
	vidHeight = size(vid, 1);
	vidWidth = size(vid, 2);
	len = size(vid, 4); %Int
	fr = len / recordingTime; %Double
	
    nChannels = number_of_channels;
    
	samplingRate = fr;
	level = min(level, floor(log((min(vidHeight, vidWidth) - 1) / (Gpyr_filter_length  - 1)) / log(2)));
	
    
    % Define the indices of the frames to be processed
    startIndex = startFrame;
    
    if endFrame > 0
    	endIndex = endFrame;
    else
    	endIndex = len + endFrame;
    end
    
    %% ================= Core part of the algo described in literature
    % compute Gaussian blur stack
    % This stack actually is just a single level of the pyramid
    disp('Spatial filtering...')
    Gdown_stack = func_build_GDown_stack(vid, startIndex, endIndex, level); % TxMxNxC array
    disp('Finished')
    
    % Temporal filtering
    disp('Temporal filtering...')
    filtered_stack = ideal_bandpassing(Gdown_stack, 1, freq_band_low_end, freq_band_high_end, samplingRate);
    %filtered_stack = filter_bandpassing(Gdown_stack, 1);
    disp('Finished')
    
    %% amplify
    filtered_stack = permute(filtered_stack, [1, 4, 2, 3]);
    for i = 1 : size(filtered_stack, 3)
    	for j = 1 : size(filtered_stack, 4)
    		temp_array = squeeze(filtered_stack(:, :, i, j));
    		
    		temp_array = temp_array * C_matrix';
    		
    		filtered_stack(:, :, i, j) = temp_array;
    	end
    end
    filtered_stack = permute(filtered_stack, [1, 3, 4, 2]);
    
	%% =================

    %% Render on the input video
    disp('Rendering...')
    
    if exist(fullfile(vidFolder, 'out'))
    	rmdir(fullfile(vidFolder, 'out'), 's');
    end
    mkdir(fullfile(vidFolder, 'out'));
    
    % output video
    k = 0;
    % Convert each frame from the filtered stream to movie frame
    for i = startIndex : endIndex
        k = k + 1;
        
        if k <= size(filtered_stack, 1)
			% Reconstruct the frame from pyramid stack		
			% by removing the singleton dimensions of the kth filtered array
			% since the filtered stack is just a selected level of the Gaussian pyramid
			filtered = squeeze(filtered_stack(k, :, :, :));
		
			% Format the image to the right size
			filtered = imresize(filtered, [vidHeight vidWidth]); %Bicubic interpolation
		
			% Extract the ith frame in the video stream
			frame = vid(:, :, :, i); %Double MxNx3 array
			
			% Add the filtered frame to the original frame
			filtered = filtered + frame;
			
			% Normalise the resultant frame
			for chan = 1 : size(filtered, 3)
				temp = filtered(:, :, chan);
				temp_range = max(temp(:)) - min(temp(:));
				frame(:, :, chan) = (temp - min(temp(:))) / temp_range;
			end
			
			% Clip the values of the frame by 0 and 1
			frame(frame > 1) = 1;
			frame(frame < 0) = 0;
			
			% Write the frame into the video as unsigned 8-bit integer array
			filename = fullfile(vidFolder, 'out', ...
						[num2str(k)...
						'-ideal-from-' num2str(freq_band_low_end) ...
						'-to-' num2str(freq_band_high_end) ...
						'-alpha-' num2str(alpha) ...
						'-level-' num2str(level) ...
						'-chromAtn-' num2str(chromAttenuation) ...
                        '.png']);
			imwrite(im2uint8(frame), filename, 'png');
		else
			break;
		end
    end

    disp('Finished')
end
