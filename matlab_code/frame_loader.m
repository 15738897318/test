function frames = frame_loader(target_folder, frame_size, channels_to_process, file_type)
	
	switch file_type
		case 'png'
			frame_list = dir(fullfile(target_folder, '*.png'));
	
			frames = [];
			for i = 1 : length(frame_list)
				[X, ~] = imread(fullfile(target_folder, frame_list(i).name), 'png'); %uint8 array
				X = double(X);
		
				if ~isempty(frame_size)
					if (frame_size(1) ~= size(X, 1)) || (frame_size(2) ~= size(X, 2))
						X = imresize(X, frame_size);
					end
                end
                
				frames(:, :, :, i) = X(:, :, channels_to_process); % Double array
			end
		
		case 'mat'
			frame_list = dir(fullfile(target_folder, '*.mat'));
			
			frames = [];
			for i = 1 : length(frame_list)
				X = load(fullfile(target_folder, frame_list(i).name)); %uint8 array from .mat file
				frames(:, :, :, i) = double(X.processed_frame); % Double array
			end
	end