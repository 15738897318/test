% magnify_linear_pyr(vidFile, outDir, alpha, 
%                                      level, freq_band_low_end, freq_band_high_end, samplingRate, 
%                                      chromAttenuation)
% Based on code by Hao-yu Wu, Michael Rubinstein, Eugene Shih (June 2012)
%
% Spatial Filtering: Gaussian blur and down sample
% Temporal Filtering: 15-tap FIR
% 
%
function magnify_linear_pyr_allband_frames(vidFolder, ...
													alpha, level, ...
													freq_band_low_end, freq_band_high_end, ...
													chromAttenuation, ...
													new_fr, new_size)
    %Load constants
    initialiser;
	
    C_matrix = [1.0000, 0.9562, 0.6214;
                1.0000, -0.2727, -0.6468;
                1.0000, -1.1037, 1.7006] * ...
                alpha * [1, 0, 0;
                        0, chroma_magnifier, 0;
                        0, 0, chroma_magnifier] * ...
                [0.299, 0.587, 0.114;
                 0.596, -0.274, -0.322;
                 0.211, -0.523, 0.312];
    
    
    % Read video
	vid = frame_loader(vidFolder); % Double array
	
	global full_fr full_vidHeight full_vidWidth
	full_vidHeight = size(vid, 1);
	full_vidWidth = size(vid, 2);
	if exist([vidFolder '/vid_specs.txt'])
		full_fr = textscan(fopen([vidFolder '/vid_specs.txt']), '%d, %d');
		full_fr = double(full_fr{2});
	else
		full_fr = size(vid, 4) / recordingTime; %Double
	end
	
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
    % Decompose the frame stream into the pyramids
    disp('Spatial filtering...')
    [pyramids, pind] = func_build_pyramid_allband(vid, startIndex, endIndex, level); % PxCxT array where P: each pixel in the whole pyramid for 1 frame, C: colour channels, T: time
    disp('Finished')
    
    % Temporal filtering
    disp('Temporal filtering...')
    filtered_pyramids = ideal_bandpassing(pyramids, 3, freq_band_low_end, freq_band_high_end, samplingRate); % PxCxT
    disp('Finished')
    
    % Amplify
    for frame_index = 1 : size(filtered_pyramids, 3)
		temp_array = squeeze(filtered_pyramids(:, :, frame_index));
		
		temp_array = temp_array * C_matrix';
		
		filtered_pyramids(:, :, frame_index) = temp_array;
	end
    
    %% Reconstruct the frame stream from the pyramids and write out
    disp('Rendering...')
    
    if exist(fullfile(vidFolder, 'out'))
    	rmdir(fullfile(vidFolder, 'out'), 's');
    end
    mkdir(fullfile(vidFolder, 'out'));
    
    k = 0;
    for i = startIndex : endIndex
        k = k + 1;
        
        if k <= size(filtered_pyramids, 1)
			% Reconstruct the frame from its pyramid
			filtered_pyramid = filtered_pyramids(:, :, k);
			
			filtered_frame = [];
			for chan = 1 : size(filtered_pyramid, 2)
				filtered_frame(:, :, chan) = func_recon_pyr(filtered_pyramid(:, chan), pind);
			end
			
			% Format the image to the right size
			filtered_frame = imresize(filtered_frame, [vidHeight vidWidth]); %Bicubic interpolation
		
			% Extract the ith frame in the video stream
			original_frame = vid(:, :, :, i); %Double MxNx3 array
			
			% Add the filtered frame to the original frame
			processed_frame = filtered_frame + original_frame;
			
			% Normalise the resultant frame
			for chan = 1 : size(processed_frame, 3)
				temp = processed_frame(:, :, chan);
				temp_range = max(temp(:)) - min(temp(:));
				frame(:, :, chan) = (temp - min(temp(:))) / temp_range;
			end
			
			% Clip the values of the frame by 0 and 1
			processed_frame(processed_frame > 1) = 1;
			processed_frame(processed_frame < 0) = 0;
			
			% Write the frame into the video as unsigned 8-bit integer array
			filename = fullfile(vidFolder, 'out', ...
								[num2str(k)...
								'-ideal-from-' num2str(freq_band_low_end) ...
								'-to-' num2str(freq_band_high_end) ...
								'-alpha-' num2str(alpha) ...
								'-level-' num2str(level) ...
								'-chromAtn-' num2str(chromAttenuation) ...
								'.png']);
			imwrite(im2uint8(processed_frame), filename, 'png');
		else
			break;
		end
    end
    
    if exist([vidFolder '/vid_specs.txt'])
    	copyfile([vidFolder '/vid_specs.txt'], fullfile(vidFolder, 'out'));
	end
    
    disp('Finished')
end
