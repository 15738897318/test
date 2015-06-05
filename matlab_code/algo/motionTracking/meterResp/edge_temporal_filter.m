function signal = edge_temporal_filter(signal, constants)
	filter_kernel = constants.filter;
    
	signal = filter(filter_kernel, 1, signal);  %Double 1-D vector
	signal = signal((length(filter_kernel) - floor(length(filter_kernel) / 2)) : end);
end