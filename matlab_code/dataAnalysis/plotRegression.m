function [fig_handle, stats] = plotRegression(data1, data2, varargin)
	if numel(data1) == 0 || numel(data2) == 0
		error(sprintf('Input data must not be empty!\nData 1 has %d elements; Data 2 has %d elements.', numel(data1), numel(data2)));
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

	P = polyfit(data1, data2, 1);

	figure();
	hold('on')
	plot(data1, data2, 'x');
	plot(data1, data1, 'k--');
	plot(data1, polyval(P, data1), 'r--');
	hold('off')

	xlabel('Ref data');
	ylabel('Obs data');
	title(title_str);

	axis tight;
	grid on;

	Rsq = corrcoef(data1, data2);
	Rsq = Rsq(1, 2);

	r = rmsd(data1, data2);
	p = precision(data1, data2);
	b = meanbias(data1, data2);

	newline = 10;
	plotText = [...
	    'R^2  = ' num2str(Rsq,'%.02f') newline ...
	    'y    = ' num2str(P(1),'%.02f') 'x' ' + ' num2str(P(2),'%.02f') newline ...
	    'rmsd = ' num2str(r,'%.02f') newline ...
	    'prec = ' num2str(p,'%.02f') newline ...
	    'bias = ' num2str(b,'%.02f') ...
	    ];

	a = axis(gca);
	text(a(1) + (a(2) - a(1)) * .05, a(4) - (a(4) - a(3)) * .05, plotText, ...
		'VerticalAlignment', 'top', 'HorizontalAlignment', 'left', 'EdgeColor', [0 0 0], 'FontWeight', 'bold', 'FontSize', 14);

	stats.rmsd = r;
	stats.precision = p;
	stats.bias = b;

	fig_handle = gcf;