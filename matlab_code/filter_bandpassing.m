function filtered = filter_bandpassing(input, dim)

    if (dim > size(size(input),2))
        error('Exceed maximum dimension');
    end

    input_shifted = shiftdim(input, dim - 1);
    Dimensions = size(input_shifted);
    
    n = Dimensions(1);
    dn = size(Dimensions, 2);
    
    filter_kernel = [0.0034; 0.0087; 0.0244; 0.0529; 0.0909; 0.1300; 0.1594; 0.1704; 0.1594; 0.1300; 0.0909; 0.0529; 0.0244; 0.0087; 0.0034];
    
    for j = 1 : size(input_shifted, 4)
		for i = 1 : size(input_shifted, 3)
			filtered(:, :, i, j) = conv2(input_shifted(:, :, i, j), filter_kernel, 'same');
		end
	end
    filtered = filtered(8 : end, :, :, :);
    
    filtered = shiftdim(filtered, dn - (dim - 1));
end
