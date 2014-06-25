% This function converts an RGB colourmap to a TSL colourmap
% TSL ref: Wikipedia

function tslmap = rgb2tsl(rgbmap)
	rgbmap = im2double(rgbmap);
	
	tslmap = zeros(size(rgbmap));
	
	r_primes = bsxfun(@minus, bsxfun(@rdivide, rgbmap(:, :, 1), sum(rgbmap, 3)), 1/3);
	r_primes(isnan(r_primes)) = -1/3;
	g_primes = bsxfun(@minus, bsxfun(@rdivide, rgbmap(:, :, 2), sum(rgbmap, 3)), 1/3);
	g_primes(isnan(g_primes)) = -1/3;
	
	temp1 = zeros(size(g_primes));
	temp1(bsxfun(@gt, g_primes, 0)) = 1/4;
	temp1(bsxfun(@lt, g_primes, 0)) = 3/4;
	
	temp2 = ones(size(g_primes));
	temp2(bsxfun(@eq, g_primes, 0)) = 0;
	
	tslmap(:, :, 1) = bsxfun(@plus, 1 / (2 * pi) * bsxfun(@times, bsxfun(@atan2, r_primes, g_primes), temp2), temp1);
	tslmap(:, :, 2) = bsxfun(@power, (9/5 * (bsxfun(@power, r_primes, 2) + bsxfun(@power, g_primes, 2))), 1/2);
	tslmap(:, :, 3) = 0.299 * rgbmap(:, :, 1) + 0.587 * rgbmap(:, :, 2) + 0.114 * rgbmap(:, :, 3);