src_folder = '/Users/misfit/Desktop/Codes - Local/Active/bioSignal/Codes/main/refMIT/vidData/';
vidFile = 'baby.mp4';
vidFile = 'baby-iir-r1-0.4-r2-0.05-alpha-10-lambda_c-6-chromAtn-0.1.mp4';
% vidFile = 'baby-ideal-from-0.05-to-0.5-alpha-150-level-6-chromAtn-0.avi';

vidFile = 'baby2-ideal-from-2.3333-to-2.6667-alpha-150-level-6-chromAtn-1.mp4';
% vidFile = 'baby2.mp4';

method = 'canny';
threshold = 0;

% Read video
vid = VideoReader(fullfile(src_folder, vidFile));

% Extract video info
vidHeight = vid.Height;
vidWidth = vid.Width;
vidFR = vid.FrameRate;
vidLen = vid.NumberOfFrames;
nChannels = 3;

% % Create the output file with full path
% [~, vidName] = fileparts(vidFile);
% outName = fullfile(fullfile(src_folder, [vidName '-edge.avi']));
% 
% % Prepare the output video-writer
% vidOut = VideoWriter(outName);
% vidOut.FrameRate = vidFR;
% open(vidOut)


% Create the placeholder for a single movie-frame (which has to have both cdata & colormap, per Matlab def)
temp = struct('cdata', zeros(vidHeight, vidWidth, nChannels, 'uint8'), ...
			  'colormap', []);
k = 0;
sig = zeros(1, vidLen - 1);
for i = 1 : vidLen
	display(sprintf('Frame %d/%d (%d%%)', i, vidLen, round(100*i/vidLen)));

    % Extract the ith frame in the video stream
	temp.cdata = read(vid, i);
	% Convert the extracted frame to greyscale image
	[rgbframe, ~] = frame2im(temp);
	greyframe = im2double(rgb2gray(rgbframe));
	
	% Edge detection on the original frame
	edgeframe = edge(greyframe, method);

	% Gradient magnitude
	[Gmag, Gdir] = imgradient(greyframe);

	% Suppress gradients below the 25th percentile
	Gmag_thres = (max(Gmag(:)) - min(Gmag(:))) * 0.25 + min(Gmag(:));
	Gmag(Gmag < Gmag_thres) = 0;

	% Find the edges in this suppressed gradient map (effectively finding 2nd derivative)
	Gedgeframe = edge(Gmag, method, threshold);
    
    % Perform the distance transform on the resulting edges
    % dtGedgeframe = bwdist(Gedgeframe);
    % dtGedgeframe = dtGedgeframe / max(dtGedgeframe(:));

    if k == 0
    	Gedgeframe_ref = Gedgeframe;
    else
    	Gedgeframe_diff = Gedgeframe - Gedgeframe_ref;
    	sig(k) = sum(abs(Gedgeframe_diff(:)));
    end

    k = k + 1;		
	% Write the frame into the video as unsigned 8-bit integer array
%	writeVideo(vidOut, im2uint8(edgeframe(:, :, [1 1 1])));
end

% disp('Finished')
% close(vidOut);