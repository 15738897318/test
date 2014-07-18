% GDOWN_STACK = build_GDown_stack(VID_FILE, START_INDEX, END_INDEX, LEVEL)
% 
% Apply Gaussian pyramid decomposition on VID_FILE from START_INDEX to
% END_INDEX and select a specific band indicated by LEVEL
% 
% GDOWN_STACK: stack of one band of Gaussian pyramid of each frame 
% the first dimension is the time axis
% the second dimension is the y axis of the video
% the third dimension is the x axis of the video
% the forth dimension is the color channel
% 
% Copyright (c) 2011-2012 Massachusetts Institute of Technology, 
% Quanta Research Cambridge, Inc.
%
% Authors: Hao-yu Wu, Michael Rubinstein, Eugene Shih, 
% License: Please refer to the LICENCE file
% Date: June 2012
%
function GDown_stack = build_GDown_stack(vidFile, startIndex, endIndex, level)

	% Read video
	vid = VideoReader(vidFile);
	
	% Extract video info
	vidHeight = vid.Height;
	vidWidth = vid.Width;
	nChannels = 3;
	
	% Create the placeholder for a single movie-frame (which has to have both cdata & colormap, per Matlab def)
	temp = struct('cdata', zeros(vidHeight, vidWidth, nChannels, 'uint8'), ...
				  'colormap', []);
	
	% firstFrame
	temp.cdata = read(vid, startIndex);
	[rgbframe, ~] = frame2im(temp);
	rgbframe = im2double(rgbframe);
	%frame = rgb2ntsc(rgbframe);
	frame = rgbframe;
	
	% Blur and downsample the frame
	blurred = blurDnClr(frame, level);

	% create pyr stack
	% Note that this stack is actually just a SINGLE level of the pyramid
	GDown_stack = zeros(endIndex - startIndex + 1, size(blurred, 1), size(blurred, 2), size(blurred, 3));
	
	% The first frame in the stack is saved
	GDown_stack(1, :, :, :) = blurred;

	k = 1;
	for i = startIndex + 1 : endIndex
		k = k + 1;
		
		% Create a frame from the ith array in the stream
		temp.cdata = read(vid, i);
		[rgbframe, ~] = frame2im(temp);
		rgbframe = im2double(rgbframe);
		%frame = rgb2ntsc(rgbframe);
		frame = rgbframe;
	
		% Blur and downsample the frame
		blurred = blurDnClr(frame, level);
		
		% The kth element in the stack is saved
		% Note that this stack is actually just a SINGLE level of the pyramid
		GDown_stack(k, :, :, :) = blurred;
	end
end
