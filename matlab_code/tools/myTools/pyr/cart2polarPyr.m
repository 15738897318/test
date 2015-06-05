function [polar_pyramids, band_pairs, band_indices] = cart2polarPyr(cart_pyramids, pind)
    number_of_levels = spyrHt(pind);
    
    % Find the number of bands (i.e. orientation) in the pyramid
    number_of_bands = spyrNumBands(pind);
    
    if number_of_bands == 1
    	polar_pyramids = cart_pyramids;
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
    
    polar_pyramids = cart_pyramids;
    for frame_ind = 1 : size(cart_pyramids, 3)
		for chan_ind = 1 : size(cart_pyramids, 2)
			cart_pyr = cart_pyramids(:, chan_ind, frame_ind);
			
			polar_pyr = cart_pyr;
			for band_pair = 1 : size(band_pairs, 1)
				inphase = cart_pyr(band_indices{band_pairs(band_pair, 1)});
				quadrature = cart_pyr(band_indices{band_pairs(band_pair, 2)});

				polar_pyr(band_indices{band_pairs(band_pair, 1)}) = sqrt(inphase.^2 + quadrature.^2);
				polar_pyr(band_indices{band_pairs(band_pair, 2)}) = atan2(quadrature, inphase);
			end
			
			polar_pyramids(:, chan_ind, frame_ind) = polar_pyr;
		end
    end
end
