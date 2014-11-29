function amplified_pyrs = amplifyPyr_allband(orig_pyrs, pind, amp_coeffs, lambda0, amp_type)
	alpha = amp_coeffs(1);
	chroma_magnifier = amp_coeffs(2);
	
	amplified_pyrs = orig_pyrs;
	switch amp_type
		case 'uniform'	
			if chroma_magnifier == 1
				amplified_pyrs = orig_pyrs * alpha;
			else
				C_matrix = [1.0000, 0.9562, 0.6214;
							1.0000, -0.2727, -0.6468;
							1.0000, -1.1037, 1.7006] * ...
							alpha * [1, 0, 0;
									0, chroma_magnifier, 0;
									0, 0, chroma_magnifier] * ...
							[0.299, 0.587, 0.114;
							 0.596, -0.274, -0.322;
							 0.211, -0.523, 0.312];
				
				for frame_index = 1 : size(orig_pyrs, 3)
					temp_array = squeeze(orig_pyrs(:, :, frame_index));
		
					temp_array = temp_array * C_matrix';
		
					amplified_pyrs(:, :, frame_index) = temp_array;
				end
			end
		
		case 'adaptive'
			%% amplify each spatial frequency bands adaptively so as not to violate the bound
			if size(pind, 1) > 2
				number_of_levels = size(unique(pind(2 : end - 1, 1)));
			elseif size(pind, 1)== 2
				number_of_levels = 2;
			else
				error('The pyramids should have more than 1 level!');
			end
			number_of_bands = size(pind(2 : end - 1, 1)) / number_of_levels;
						
			% lowest spatial frequency band of Laplacian pyramid
			lambda = lambda0;
			
			for l = number_of_levels : -1 : 1
				% Calculate the indices for the current level
				indices = [];
				for band = 1 : number_of_bands
					band_at_level = 1 + band + number_of_bands * (level - 1);
	
					indices = [indices, pyrBandIndices(pind, band_at_level)];
				end
				
				% compute modified alpha for this level
				currAlpha = lambda / lambda0 * alpha;
				
				if chroma_magnifier == 1
					C_matrix = currAlpha * eye(3);
				else
					C_matrix = [1.0000, 0.9562, 0.6214;
								1.0000, -0.2727, -0.6468;
								1.0000, -1.1037, 1.7006] * ...
								currAlpha * [1, 0, 0;
											0, chroma_magnifier, 0;
											0, 0, chroma_magnifier] * ...
								[0.299, 0.587, 0.114;
								 0.596, -0.274, -0.322;
								 0.211, -0.523, 0.312];
				end
				
				if (l < number_of_levels && l > 1) % ignore the highest and lowest frequency band					
					for frame_index = 1 : size(orig_pyrs, 3)
						temp_array = squeeze(orig_pyrs(indices, :, frame_index));
		
						temp_array = temp_array * C_matrix';
		
						amplified_pyrs(indices, :, frame_index) = temp_array;
					end
				end

				% go one level down on pyramid, 
				% representative lambda will reduce by factor of 2
				lambda = lambda / 2; 
			end
	end