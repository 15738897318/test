% magnify_linear_pyr(vidFile, outDir, alpha, 
%                                      level, freq_band_low_end, freq_band_high_end, samplingRate, 
%                                      chromAttenuation)
% Based on code by Hao-yu Wu, Michael Rubinstein, Eugene Shih (June 2012)
%
% Spatial Filtering: Gaussian blur and down sample
% Temporal Filtering: 15-tap FIR
% 
%
function magnify_linear_pyr_allband_video(vidFile, outDir, ...
											alpha, level, ...
											freq_band_low_end, freq_band_high_end, ...
											samplingRate, chroma_magnifier)
    %Load constants
    initialiser;

    % Read video
    vid = VideoReader(vidFile);

	% Extract video info
    vidHeight = vid.Height;
    vidWidth = vid.Width;
    fr = vid.FrameRate;
    len = vid.NumberOfFrames;
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
    [pyramids, pind] = func_build_pyramid(vidFile, startIndex, endIndex, level); % PxCxT array where P: each pixel in the whole pyramid for 1 frame, C: colour channels, T: time
    disp('Finished')
    
    % Temporal filtering
    disp('Temporal filtering...')
    filtered_pyramids = ideal_bandpassing(pyramids, length(size(pyramids)), freq_band_low_end, freq_band_high_end, samplingRate); % PxCxT
    disp('Finished')
    
    if length(size(filtered_pyramids)) == 2
        filtered_pyramids = permute(filtered_pyramids, [2 3 1]);
    end
    
    %---- Amplify
    % compute the representative wavelength lambda for the lowest spatial 
	% frequency band of Laplacian pyramid
	disp('Amplifying...')
	lambda0 = sqrt(vidHeight^2 + vidWidth^2) / 3; % 3 is experimental constant (MIT)
	amp_type = 'adaptive';
    filtered_pyramids = func_amplify_pyr(filtered_pyramids, pind, [alpha chroma_magnifier], lambda0, amp_type);
    disp('Finished')
    
    %% Reconstruct the frame stream from the pyramids and write out
    disp('Rendering...')
    
	% Create the placeholder for a single movie-frame (which has to have both cdata & colormap, per Matlab def)
    temp = struct('cdata', zeros(vidHeight, vidWidth, nChannels, 'uint8'),...
    			  'colormap', []);
    
    % output video
    % Get the filename-only part of the full path
    [~, vidName] = fileparts(vidFile);
    
    % Create the output file with full path
    outName = fullfile(outDir,[vidName '-ideal-from-' num2str(freq_band_low_end) ...
									   '-to-' num2str(freq_band_high_end) ...
									   '-alpha-' num2str(alpha) ...
									   '-level-' num2str(level) ...
									   '-chromAtn-' num2str(chroma_magnifier) ...
									   '.avi']);

    % Prepare the output video-writer
    vidOut = VideoWriter(outName);
    vidOut.FrameRate = fr;
    open(vidOut)

    k = 0;
    for i = startIndex : endIndex
        k = k + 1;
        
        if k <= size(filtered_pyramids, 1)
			% Reconstruct the frame from its pyramid
			filtered_pyramid = filtered_pyramids(:, :, k);
			
			filtered_frame = [];
			if strcmpi(pyramid_style, 'steerable')
				for chan = 1 : size(filtered_pyramid, 2)
					filtered_frame(:, :, chan) = func_recon_pyr(filtered_pyramid(:, chan), pind, filter_file);
				end
			else
				for chan = 1 : size(filtered_pyramid, 2)
					filtered_frame(:, :, chan) = func_recon_pyr(filtered_pyramid(:, chan), pind);
				end
			end
			
			% Format the image to the right size
			filtered_frame = imresize(filtered_frame, [vidHeight vidWidth]); %Bicubic interpolation
		
			% Extract the ith frame in the video stream
			temp.cdata = read(vid, i);
			% Convert the extracted frame to RGB image
			[rgbframe, ~] = frame2im(temp);
			% Convert the RGB image to double-precision image
			original_frame = im2double(rgbframe);
			
			% Add the filtered frame to the original frame
			processed_frame = filtered_frame + original_frame;
			
			% Normalise the resultant frame
			for chan = 1 : size(processed_frame, 3)
				temp1 = processed_frame(:, :, chan);
				temp_range = max(temp1(:)) - min(temp1(:));
				processed_frame(:, :, chan) = (temp1 - min(temp1(:))) / temp_range;
			end
			
			% Clip the values of the frame by 0 and 1
			processed_frame(processed_frame > 1) = 1;
			processed_frame(processed_frame < 0) = 0;
			
			% Write the frame into the video as unsigned 8-bit integer array
			writeVideo(vidOut, im2uint8(processed_frame));
		else
			break;
		end
    end
    
    disp('Finished')
    close(vidOut);
end
