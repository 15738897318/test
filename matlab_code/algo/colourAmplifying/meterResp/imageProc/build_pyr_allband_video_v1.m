% pyramids = build_pyr(VID_FILE, START_INDEX, END_INDEX, LEVEL)
% 
% Apply steerable-Gabor-wavelet pyramids decomposition on VID_FILE from START_INDEX to
% END_INDEX and select a specific band indicated by LEVEL
% 
% pyramids: stack of one band of Wavelet pyramids of each frame 
% the first dimension is the time axis
% the second dimension is the y axis of the video
% the third dimension is the x axis of the video
% the forth dimension is the color channel

function [pyramids, pind] = build_pyr_allband_video(vidFile, startIndex, endIndex, level)
	
	global func_make_pyr pyramid_style
	
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
	
	% Build the pyramid for the first channel of the first frame
	% Each pyramid is a 1-D vector where pind shows the sizes of all the frames in all the levels
	% thus the frames can be recovered from pyr
	if strcmpi(pyramid_style, 'steerable')
		global filter_file
		[pyr, pind] = func_make_pyr(frame(:, :, 1), level, filter_file);
	else
		[pyr, pind] = func_make_pyr(frame(:, :, 1), level);
	end
	
    % Pre-allocate pyr stack based on the parameters acquired from the first pyramid
    pyramids = zeros(size(pyr, 1), nChannels, endIndex - startIndex + 1);
    
    % Save the pyramid for each channel of the first frame into the stack
    k = 1;
    pyramids(:, 1, 1) = pyr;
    if strcmpi(pyramid_style, 'steerable')
		if nChannels > 1
			for chan = 2 : nChannels
				[pyramids(:, chan, 1), ~] = func_make_pyr(frame(:, :, chan), level, filter_file);
			end
		end
				
		for i = startIndex + 1 : endIndex
			k = k + 1;
		
			% Create a frame from the ith array in the stream
			temp.cdata = read(vid, i);
			[rgbframe, ~] = frame2im(temp);
			rgbframe = im2double(rgbframe);
			%frame = rgb2ntsc(rgbframe);
			frame = rgbframe;
		
			% Save the pyramid for each channel of the frame into the stack
			for chan = 1 : nChannels
				[pyramids(:, chan, k), ~] = func_make_pyr(frame(:, :, chan), level, filter_file);
			end
		end
	else
		if nChannels > 1
			for chan = 2 : nChannels
				[pyramids(:, chan, 1), ~] = func_make_pyr(frame(:, :, chan), level);
			end
		end
   		
		for i = startIndex + 1 : endIndex
			k = k + 1;
		
			% Create a frame from the ith array in the stream
			temp.cdata = read(vid, i);
			[rgbframe, ~] = frame2im(temp);
			rgbframe = im2double(rgbframe);
			%frame = rgb2ntsc(rgbframe);
			frame = rgbframe;
		
			% Save the pyramid for each channel of the frame into the stack
			for chan = 1 : nChannels
				[pyramids(:, chan, k), ~] = func_make_pyr(frame(:, :, chan), level);
			end
		end
	end
end
