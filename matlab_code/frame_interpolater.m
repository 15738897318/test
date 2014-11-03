function new_frames = frame_interpolater(frames, fr_as_ratio, size_as_ratio)
	new_frames = [];
	
	num_new_frames = floor(size(frames, 4) * fr_as_ratio);
	
	for i = 1 : num_new_frames
		new_frame_in_old = (i - 1) / fr_as_ratio + 1;
		
		new_frame_in_old_floor = floor(new_frame_in_old);
		new_frame_in_old_ceil = ceil(new_frame_in_old);
		
		new_frame = (frames(:, :, :, new_frame_in_old_ceil) - frames(:, :, :, new_frame_in_old_floor)) ...
						* (new_frame_in_old - new_frame_in_old_floor) ...
					+ frames(:, :, :, new_frame_in_old_floor);
					
		new_frame = imresize(new_frame, size_as_ratio);
		
		new_frames(:, :, :, i) = new_frame;
	end