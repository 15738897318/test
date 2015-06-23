function frames = frame_loader(target_folder, frame_size, channels_to_process, file_type)
	
	switch file_type
		case 'png'
			frame_list = dir(fullfile(target_folder, '*.png'));
			if length(frame_list) == 0
				error('No PNG files in the folder!');
			end

            frame_list = {frame_list.name};
            frame_list = sort_nat(frame_list);
            
			[X, ~] = imread(fullfile(target_folder, frame_list{1}), 'png'); %uint8 array
			X = double(X);
	
			if ~isempty(frame_size)
				if (frame_size(1) ~= size(X, 1)) || (frame_size(2) ~= size(X, 2))
					X = imresize(X, frame_size);
				end
            end
			
            frames = zeros(size(X, 1), size(X, 2), length(channels_to_process), length(frame_list));
			frames(:, :, :, 1) = X(:, :, channels_to_process); % Double array

			if length(frame_list) > 1
				for i = 2 : length(frame_list)
					[X, ~] = imread(fullfile(target_folder, frame_list{i}), 'png'); %uint8 array
					X = double(X);
			
					if ~isempty(frame_size)
						if (frame_size(1) ~= size(X, 1)) || (frame_size(2) ~= size(X, 2))
							X = imresize(X, frame_size);
						end
	                end
	                
					frames(:, :, :, i) = X(:, :, channels_to_process); % Double array
				end
			end
		
		case 'mat'
			frame_list = dir(fullfile(target_folder, '*.mat'));
			if length(frame_list) == 0
				error('No MAT files in the folder!');
			end
			
			frame_list = {frame_list.name};
            frame_list = sort_nat(frame_list);

            X = load(fullfile(target_folder, frame_list{1})); %uint8 array from .mat file

            frames = zeros(sixe(X, 1), size(X, 2), size(X, 3), length(frame_list));
			frames(:, :, :, 1) = X(:, :, channels_to_process); % Double array
            
            if length(frame_list) > 1
				for i = 2 : length(frame_list)
					X = load(fullfile(target_folder, frame_list{i})); %uint8 array from .mat file
					frames(:, :, :, i) = double(X.processed_frame); % Double array
				end
			end
	end