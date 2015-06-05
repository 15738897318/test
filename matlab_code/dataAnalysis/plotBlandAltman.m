function fig_handle = plotBlandAltman(data1, data2, varargin)
	if numel(data1) == 0 || numel(data2) == 0
		error('Input data must not be empty!\nData 1 has %d elements; Data 2 has %d elements.', numel(data1), numel(data2));
	end

	data1 = reshape(data1, [numel(data1), 1]);
	data2 = reshape(data2, [numel(data2), 1]);

	if length(data1) ~= length(data2)
		error('Input data must have the same number of elements!');
	end

	if length(varargin) < 1
		title_str = '';
	else
		title_str = varargin{1};
	end

	x_data = (data1 + data2) / 2;
	y_data = data1 - data2;

	y_mean = mean(y_data(isfinite(y_data)));
	y_std = std(y_data(isfinite(y_data)));

	x_min = min(x_data(isfinite(x_data)));
	x_max = max(x_data(isfinite(x_data)));

	figure();
	
	hold('on')
	plot(x_data, y_data, 'kx');
	plot([x_min, x_max], [1, 1] * y_mean, 'b--', 'DisplayName', sprintf('Mean: %d', y_mean));
	plot([x_min, x_max], [1, 1] * (y_mean + y_std), 'r-.', 'DisplayName', sprintf('Upper stan.dev.: %d', y_mean + y_std));
	plot([x_min, x_max], [1, 1] * (y_mean - y_std), 'g-.', 'DisplayName', sprintf('Lower stan.dev.: %d', y_mean - y_std));
	hold('off')
	
	xlabel('Average of two measures');
	ylabel('Difference between two measures');
	title(title_str);

	axis tight;
	grid on;
	legend('show');

	fig_handle = gcf;