function frames = frame_loader(target_folder)
	frame_list = dir(fullfile(target_folder, '*.png'));
    
    frames = [];
	for i = 1 : length(frame_list)
		[X, map] = imread(fullfile(target_folder, frame_list(i).name), 'png'); %uint8 array
		frames(:, :, :, i) = double(X); % Double array
	end;