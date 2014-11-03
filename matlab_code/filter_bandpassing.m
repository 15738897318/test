function filtered = filter_bandpassing(input, dim) %Input is a TxMxNxC array
	% Load constants
	initialiser;
	
    if (dim > size(size(input),2))
        error('Exceed maximum dimension');
    end

    input_shifted = shiftdim(input, dim - 1);
    Dimensions = size(input_shifted);
    
    n = Dimensions(1);
    dn = size(Dimensions, 2);
    
    filter_kernel = eulerianTemporalFilterKernel;
    
    for j = 1 : size(input_shifted, 4)
		for i = 1 : size(input_shifted, 3)
			filtered(:, :, i, j) = conv2(input_shifted(:, :, i, j), filter_kernel, 'same');
		end
	end
    filtered = filtered(ceil(length(filter_kernel) / 2) : end, :, :, :);
    
    %shift_vect = zeros(1, size(size(filtered), 2));
    %shift_vect(1) = -floor(length(filter_kernel) / 2);
    %filtered = circshift(filtered, shift_vect);
    
    filtered = shiftdim(filtered, dn - (dim - 1));
end
