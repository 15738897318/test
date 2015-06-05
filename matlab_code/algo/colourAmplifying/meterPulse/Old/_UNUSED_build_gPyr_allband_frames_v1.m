% pyramids = build_gPyr(VID_FILE, START_INDEX, END_INDEX, LEVEL)
% 
% Apply Gaussian pyramids decomposition on VID_FILE from START_INDEX to
% END_INDEX and select a specific band indicated by LEVEL
% 
% pyramids: stack of one band of Gaussian pyramids of each frame 
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
function [pyramids, pind] = build_gPyr_frames(vid, startIndex, endIndex, level)

	% Extract video info
	vidHeight = size(vid, 1);
	vidWidth = size(vid, 2);
	nChannels = 3;
	
	% firstFrame
	rgbframe = vid(:, :, :, startIndex); %Double
	%frame = rgb2ntsc(rgbframe);
	frame = rgbframe;
	
	% Build the pyramid for the first channel of the first frame
	% Each pyramid is a 1-D vector where pind shows the sizes of all the frames in all the levels
	% thus the frames can be recovered from pyr
	[pyr, pind] = buildGpyr(frame(:, :, 1) ,'auto');

    % Pre-allocate pyr stack based on the parameters acquired from the first pyramid
    pyramids = zeros(size(pyr, 1), 3, endIndex - startIndex + 1);
    
    % Save the pyramid for each channel of the first frame into the stack
    pyramids(:, 1, 1) = pyr;
    [pyramids(:, 2, 1), ~] = buildGpyr(frame(:, :, 2), 'auto');
    [pyramids(:, 3, 1), ~] = buildGpyr(frame(:, :, 3), 'auto');

    k = 1;
    for i = startIndex + 1 : endIndex
		k = k + 1;
		
		% Create a frame from the ith array in the stream
		rgbframe = vid(:, :, :, i);
		%frame = rgb2ntsc(rgbframe);
		frame = rgbframe;
		
		% Save the pyramid for each channel of the frame into the stack
		[pyramids(:, 1, k), ~] = buildGpyr(frame(:, :, 1), 'auto');
		[pyramids(:, 2, k), ~] = buildGpyr(frame(:, :, 2), 'auto');
		[pyramids(:, 3, k), ~] = buildGpyr(frame(:, :, 3), 'auto');
    end
end
