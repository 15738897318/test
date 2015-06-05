function signal = edge_summariser_video(vidFile, constants)
	% Read video
	vid = VideoReader(vidFile);

	% Extract video info
	vidHeight = vid.Height;
	vidWidth = vid.Width;
	vidFR = vid.FrameRate;
	vidLen = vid.NumberOfFrames;
	nChannels = 3;

	%% Raw summarisation from each frame into one time-series sample
	temp = struct('cdata', zeros(vidHeight, vidWidth, nChannels, 'uint8'), ...
				  'colormap', []);
	k = 0;
	signal = zeros(1, vidLen - 1);
	for i = 1 : vidLen
		display(sprintf('Frame %d/%d (%d%%)', i, vidLen, round(100*i/vidLen)));

	    % Extract & convert to greyscale the ith frame in the video stream
		temp.cdata = read(vid, i);
		greyframe = im2double(rgb2gray(frame2im(temp)));

		% Calculate the gradient magnitude
		[gradients_magnitude, gradients_direction] = imgradient(greyframe);

		% Only take the prominent-enough gradients by suppressing the bottom 25 percents
		gradient_threshold = (max(gradients_magnitude(:)) - min(gradients_magnitude(:))) * 0.25 + min(gradients_magnitude(:));
		gradients_magnitude(gradients_magnitude < gradient_threshold) = 0;

		% Find the edges in this suppressed gradient map
		% So each edge generates 1 edge to either side, effectively thickening the edge
		gradient_edges = edge(gradients_magnitude, constants.method, constants.threshold);

		% Each time-series sample is the number of gradient-pixels that differ between this current frame and the ref frame
		% The rationale behind this is that the length of the moving edge effectively changes between frames
	    if k == 0
	    	gradient_edges_ref = gradient_edges;
	    else
	    	gradient_edges_diff = gradient_edges - gradient_edges_ref;
	    	signal(k) = sum(abs(gradient_edges_diff(:)));
	    end

	    k = k + 1;		
	end
end