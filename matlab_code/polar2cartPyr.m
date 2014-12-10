function [cart_pyramids, band_pairs, band_indices] = polar2cartPyr(polar_pyramids, pind)
    number_of_levels = spyrHt(pind);
    
    % Find the number of bands (i.e. orientation) in the pyramid
    number_of_bands = spyrNumBands(pind);
    
    if number_of_bands == 1
    	cart_pyramids = polar_pyramids;
    	band_pairs = [];
    	band_indices = [];
    	
    	return;
    end
    
    band_pairs = [1 : number_of_bands / 2; ...
    			  (number_of_bands / 2 + 1) : number_of_bands]';
    
    % Find the indices for each band (across all levels)
    band_indices = {};
    for band = 1 : number_of_bands
		indices = [];
		for level = 1 : number_of_levels
			band_at_level = 1 + band + number_of_bands * (level - 1);
		
			indices = [indices, pyrBandIndices(pind, band_at_level)];
		end
		
		band_indices{band} = indices;
	end
    
    cart_pyramids = polar_pyramids;
    for frame_ind = 1 : size(polar_pyramids, 3)
		for chan_ind = 1 : size(polar_pyramids, 2)
			polar_pyr = polar_pyramids(:, chan_ind, frame_ind);
			
			cart_pyr = polar_pyr;
			for band_pair = 1 : size(band_pairs, 1)
				magnitude = polar_pyr(band_indices{band_pairs(band_pair, 1)});
				phase = polar_pyr(band_indices{band_pairs(band_pair, 2)});

				cart_pyr(band_indices{band_pairs(band_pair, 1)}) = magnitude .* cos(phase);
				cart_pyr(band_indices{band_pairs(band_pair, 2)}) = magnitude .* sin(phase);
			end
			
			cart_pyramids(:, chan_ind, frame_ind) = cart_pyr;
		end
    end
end
