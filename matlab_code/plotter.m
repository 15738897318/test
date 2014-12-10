resultFolder = '/Users/misfit/Desktop/Codes - Local/Code - Active/bioSignalProcessing/videoHeartRate-Data-Results/Run 01';
template_to_include = 'Finger';
exportFlag = 1;

if ~exist(fullfile(resultFolder, 'Graphs'))
	mkdir(fullfile(resultFolder, 'Graphs'));
end

resultFiles = dir(fullfile(resultFolder, ['*' template_to_include '.mat']));
resultFiles = {resultFiles.name};

hr_ranges = [0 50;
			 50 70;
			 70 100;
			 100 Inf];

% Load the result files into variables of the same names as the files
for file_ind = 1 : length(resultFiles)
	temp = strsplit(resultFiles{file_ind}, '.');
	eval(sprintf(['%s = load(fullfile(resultFolder, resultFiles{%i}));'], temp{1}, file_ind));
end

% Go through each result file
for file_ind = 1 : length(resultFiles)
	temp = strsplit(resultFiles{file_ind}, '.');
	current_var = eval(temp{1});
	
	hr_arrays = current_var.hr_arrays;
	params_set = current_var.params_set;
	
	dataset_names = hr_arrays(:, 2);
	dataset_numbers = hr_arrays(:, 1);
	
	temp = [];
	% Reshape the cell array into a normal array
	for dataset_ind = 1 : length(dataset_numbers)
		temp(dataset_ind, :, :) = dataset_numbers{dataset_ind};
	end
	dataset_numbers = temp;
	
	% Calculate the errors
	dataset_errors.acf = dataset_numbers(:, :, 3) ./ dataset_numbers(:, :, 1) - 1;
	dataset_errors.acf_maxare = max(abs(dataset_errors.acf));
	dataset_errors.acf_meanare = nanmean(abs(dataset_errors.acf), 1);
	[~, dataset_errors.acf_meanare_min_setting] = min(dataset_errors.acf_meanare);
	
	dataset_errors.pda = dataset_numbers(:, :, 4) ./ dataset_numbers(:, :, 1) - 1;
	dataset_errors.pda_maxare = max(abs(dataset_errors.pda));
	dataset_errors.pda_meanare = nanmean(abs(dataset_errors.pda), 1);
	[~, dataset_errors.pda_meanare_min_setting] = min(dataset_errors.pda_meanare);
	
	dataset_errors.avg = dataset_numbers(:, :, 5) ./ dataset_numbers(:, :, 1) - 1;
	dataset_errors.avg_maxare = max(abs(dataset_errors.avg));
	dataset_errors.avg_meanare = nanmean(abs(dataset_errors.avg), 1);
	[~, dataset_errors.avg_meanare_min_setting] = min(dataset_errors.avg_meanare);
	
	pyr_heights = unique(params_set(:, 2));
	alphas = unique(params_set(:, 1));
	
	% ================= Plot the surface of errors
 	figure();
 	subplot(2, 1, 1);
 	title(sprintf('%s', resultFiles{file_ind}));
 	hold('on');
 	if length(pyr_heights) > 1
 		h = surf(alphas, pyr_heights, reshape(dataset_errors.acf_meanare, [length(pyr_heights), length(alphas)]), 'DisplayName', 'ACF');
 		set(h,'FaceColor',[1 0 0],'FaceAlpha',0.5);
 		h = surf(alphas, pyr_heights, reshape(dataset_errors.pda_meanare, [length(pyr_heights), length(alphas)]), 'DisplayName', 'PDA');
 		set(h,'FaceColor',[0 1 0],'FaceAlpha',0.5);
 		h = surf(alphas, pyr_heights, reshape(dataset_errors.avg_meanare, [length(pyr_heights), length(alphas)]), 'DisplayName', 'Avg');
 		set(h,'FaceColor',[0 0 1],'FaceAlpha',0.5);
 		xlabel('Alphas');
 		ylabel('Pyramid heights');
 		zlabel('Mean Abs. Rel. Error');
 		view(3);
 	else
 		plot(alphas, dataset_errors.acf_meanare, 'DisplayName', 'ACF');
 		plot(alphas, dataset_errors.pda_meanare, 'DisplayName', 'PDA');
 		plot(alphas, dataset_errors.avg_meanare, 'DisplayName', 'Avg');
 		xlabel('Alphas');
 		ylabel('Mean Abs. Rel. Error');
 	end
 	hold('off');
 	legend('show');
 	
 	subplot(2, 1, 2);
 	hold('on');
 	if length(pyr_heights) > 1
 		h = surf(alphas, pyr_heights, reshape(dataset_errors.acf_maxare, [length(pyr_heights), length(alphas)]), 'DisplayName', 'ACF');
 		set(h,'FaceColor',[1 0 0],'FaceAlpha',0.5);
 		h = surf(alphas, pyr_heights, reshape(dataset_errors.pda_maxare, [length(pyr_heights), length(alphas)]), 'DisplayName', 'PDA');
 		set(h,'FaceColor',[0 1 0],'FaceAlpha',0.5);
 		h = surf(alphas, pyr_heights, reshape(dataset_errors.avg_maxare, [length(pyr_heights), length(alphas)]), 'DisplayName', 'Avg');
 		set(h,'FaceColor',[0 0 1],'FaceAlpha',0.5);
 		xlabel('Alphas');
 		ylabel('Pyramid heights');
 		zlabel('Max Abs. Rel. Error');
 		view(3);
 	else
 		plot(alphas, dataset_errors.acf_maxare, 'DisplayName', 'ACF');
 		plot(alphas, dataset_errors.pda_maxare, 'DisplayName', 'PDA');
 		plot(alphas, dataset_errors.avg_maxare, 'DisplayName', 'Avg');
 		xlabel('Alphas');
 		ylabel('Max Abs. Rel. Error');
 	end
 	hold('off');
 	legend('show');
 	
 	if exportFlag
 		temp = strsplit(resultFiles{file_ind}, '.');
 		exportThisPlot('name', [temp{1} '-ARE'], 'plotPath', fullfile(resultFolder, 'graphs'))
 		exportThisPlot('name', [temp{1} '-ARE'], 'plotPath', fullfile(resultFolder, 'graphs'), 'plotType', 'fig')
 	end
 	
 	
 	% ================= Plot the relative errors as boxplot
 	figure();
 	boxplot(dataset_errors.acf);
 	xlabel('ACF - Pyramid settings');
 	ylabel('Rel. Errors');
 	title(sprintf('%s', resultFiles{file_ind}));
 	ylim([-1 1]);
 	if exportFlag
 		temp = strsplit(resultFiles{file_ind}, '.');
 		exportThisPlot('name', [temp{1} '-Boxplot-ACF'], 'plotPath', fullfile(resultFolder, 'graphs'))
 		exportThisPlot('name', [temp{1} '-Boxplot-ACF'], 'plotPath', fullfile(resultFolder, 'graphs'), 'plotType', 'fig')
 	end
 	
 	figure();
 	boxplot(dataset_errors.pda);
 	xlabel('PDA - Pyramid settings');
 	ylabel('Rel. Errors');
 	title(sprintf('%s', resultFiles{file_ind}));
 	ylim([-1 1]);
 	if exportFlag
 		temp = strsplit(resultFiles{file_ind}, '.');
 		exportThisPlot('name', [temp{1} '-Boxplot-PDA'], 'plotPath', fullfile(resultFolder, 'graphs'))
 		exportThisPlot('name', [temp{1} '-Boxplot-PDA'], 'plotPath', fullfile(resultFolder, 'graphs'), 'plotType', 'fig')
 	end
 	
 	figure();
 	boxplot(dataset_errors.avg);
 	xlabel('Avg - Pyramid settings');
 	ylabel('Rel. Errors');
 	title(sprintf('%s', resultFiles{file_ind}));
 	ylim([-1 1]);
 	if exportFlag
 		temp = strsplit(resultFiles{file_ind}, '.');
 		exportThisPlot('name', [temp{1} '-Boxplot-Avg'], 'plotPath', fullfile(resultFolder, 'graphs'))
 		exportThisPlot('name', [temp{1} '-Boxplot-Avg'], 'plotPath', fullfile(resultFolder, 'graphs'), 'plotType', 'fig')
 	end
 	
 	
 	
 	% ================= Plot the relative errors of the best settings
 	figure();
 	h = bar(dataset_errors.acf(:, dataset_errors.acf_meanare_min_setting));
 	ylabel('Rel. error: ACF');
 	xlabel('Data points');
 	xlim([1 size(dataset_errors.acf, 1)]);
 	title(sprintf('%s - Best: Alpha %d, Height %d', resultFiles{file_ind}, ...
 												params_set(dataset_errors.acf_meanare_min_setting, 1), ...
 												params_set(dataset_errors.acf_meanare_min_setting, 2)));
 	ylim([-1 1]);
 	if exportFlag
 		temp = strsplit(resultFiles{file_ind}, '.');
 		exportThisPlot('name', [temp{1} '-Error-ACF-Best'], 'plotPath', fullfile(resultFolder, 'graphs'))
 		exportThisPlot('name', [temp{1} '-Error-ACF-Best'], 'plotPath', fullfile(resultFolder, 'graphs'), 'plotType', 'fig')
 	end
 	
 	figure();
 	h = bar(dataset_errors.pda(:, dataset_errors.pda_meanare_min_setting));
 	ylabel('Rel. error: PDA');
 	xlabel('Data points');
 	xlim([1 size(dataset_errors.pda, 1)]);
 	title(sprintf('%s - Best: Alpha %d, Height %d', resultFiles{file_ind}, ...
 												params_set(dataset_errors.pda_meanare_min_setting, 1), ...
 												params_set(dataset_errors.pda_meanare_min_setting, 2)));
 	ylim([-1 1]);
 	if exportFlag
 		temp = strsplit(resultFiles{file_ind}, '.');
 		exportThisPlot('name', [temp{1} '-Error-PDA-Best'], 'plotPath', fullfile(resultFolder, 'graphs'))
 		exportThisPlot('name', [temp{1} '-Error-PDA-Best'], 'plotPath', fullfile(resultFolder, 'graphs'), 'plotType', 'fig')
 	end
 	
 	figure();
 	h = bar(dataset_errors.avg(:, dataset_errors.avg_meanare_min_setting));
 	ylabel('Rel. error: Avg');
 	xlabel('Data points');
 	xlim([1 size(dataset_errors.avg, 1)]);
 	title(sprintf('%s - Best: Alpha %d, Height %d', resultFiles{file_ind}, ...
 												params_set(dataset_errors.avg_meanare_min_setting, 1), ...
 												params_set(dataset_errors.avg_meanare_min_setting, 2)));
 	ylim([-1 1]);
 	if exportFlag
 		temp = strsplit(resultFiles{file_ind}, '.');
 		exportThisPlot('name', [temp{1} '-Error-Avg-Best'], 'plotPath', fullfile(resultFolder, 'graphs'))
 		exportThisPlot('name', [temp{1} '-Error-Avg-Best'], 'plotPath', fullfile(resultFolder, 'graphs'), 'plotType', 'fig')
 	end
	
	
	% ================= Plot the relative errors of the best settings wrt true-HR range
	for range_ind = 1 : size(hr_ranges, 1)
		hr_range = hr_ranges(range_ind, :);
		
		index_mask = (dataset_numbers(:, :, 1) >= hr_range(1)) & (dataset_numbers(:, :, 1) < hr_range(2));
		
		temp = dataset_errors.acf;
		temp(~index_mask) = NaN;
		selected_dataset_errors.acf = temp(:, dataset_errors.acf_meanare_min_setting);
		
		temp = dataset_errors.pda;
		temp(~index_mask) = NaN;
		selected_dataset_errors.pda = temp(:, dataset_errors.pda_meanare_min_setting);
		
		temp = dataset_errors.avg;
		temp(~index_mask) = NaN;
		selected_dataset_errors.avg = temp(:, dataset_errors.avg_meanare_min_setting);
		
		figure();
		h = bar(selected_dataset_errors.acf);
		ylabel('Rel. error: ACF');
		xlabel('Data points');
		xlim([1 size(selected_dataset_errors.acf, 1)]);
		title(sprintf('%s - Best: Alpha %d, Height %d\nHR range: %d-%d', resultFiles{file_ind}, ...
													params_set(dataset_errors.acf_meanare_min_setting, 1), ...
													params_set(dataset_errors.acf_meanare_min_setting, 2), ...
													hr_range(1), hr_range(2)));
		ylim([-1 1]);
		if exportFlag
			temp = strsplit(resultFiles{file_ind}, '.');
			exportThisPlot('name', [temp{1} '-Error-ACF-Best-' num2str(hr_range(1)) '-' num2str(hr_range(2))], 'plotPath', fullfile(resultFolder, 'graphs'))
			exportThisPlot('name', [temp{1} '-Error-ACF-Best-' num2str(hr_range(1)) '-' num2str(hr_range(2))], 'plotPath', fullfile(resultFolder, 'graphs'), 'plotType', 'fig')
		end
	
		figure();
		h = bar(selected_dataset_errors.pda);
		ylabel('Rel. error: PDA');
		xlabel('Data points');
		xlim([1 size(selected_dataset_errors.pda, 1)]);
		title(sprintf('%s - Best: Alpha %d, Height %d\nHR range: %d-%d', resultFiles{file_ind}, ...
													params_set(dataset_errors.pda_meanare_min_setting, 1), ...
													params_set(dataset_errors.pda_meanare_min_setting, 2), ...
													hr_range(1), hr_range(2)));
		ylim([-1 1]);
		if exportFlag
			temp = strsplit(resultFiles{file_ind}, '.');
			exportThisPlot('name', [temp{1} '-Error-PDA-Best-' num2str(hr_range(1)) '-' num2str(hr_range(2))], 'plotPath', fullfile(resultFolder, 'graphs'))
			exportThisPlot('name', [temp{1} '-Error-PDA-Best-' num2str(hr_range(1)) '-' num2str(hr_range(2))], 'plotPath', fullfile(resultFolder, 'graphs'), 'plotType', 'fig')
		end
	
		figure();
		h = bar(selected_dataset_errors.avg);
		ylabel('Rel. error: Avg');
		xlabel('Data points');
		xlim([1 size(selected_dataset_errors.avg, 1)]);
		title(sprintf('%s - Best: Alpha %d, Height %d\nHR range: %d-%d', resultFiles{file_ind}, ...
													params_set(dataset_errors.avg_meanare_min_setting, 1), ...
													params_set(dataset_errors.avg_meanare_min_setting, 2), ...
													hr_range(1), hr_range(2)));
		ylim([-1 1]);
		if exportFlag
			temp = strsplit(resultFiles{file_ind}, '.');
			exportThisPlot('name', [temp{1} '-Error-Avg-Best-' num2str(hr_range(1)) '-' num2str(hr_range(2))], 'plotPath', fullfile(resultFolder, 'graphs'))
			exportThisPlot('name', [temp{1} '-Error-Avg-Best-' num2str(hr_range(1)) '-' num2str(hr_range(2))], 'plotPath', fullfile(resultFolder, 'graphs'), 'plotType', 'fig')
		end
	end
	
	
	% ================= Plot the full relative errors
	figure();
	h = bar3(dataset_errors.acf);
	for i = 1:numel(h)
		index = logical(kron(isnan(dataset_errors.acf(:,i)),ones(6,1)));
		zData = get(h(i),'ZData');
		zData(index,:) = nan;
		set(h(i),'ZData',zData);
	end
	zlabel('Rel. error: ACF');
	ylabel('Data points');
	ylim([1 size(dataset_errors.acf, 1)]);
	title(sprintf('%s', resultFiles{file_ind}));
	zlim([-1 1]);
	view([-90 0]);
	if exportFlag
		temp = strsplit(resultFiles{file_ind}, '.');
		exportThisPlot('name', [temp{1} '-Error-ACF'], 'plotPath', fullfile(resultFolder, 'graphs'), 'plotType', 'fig')
	end
	
	figure();
	h = bar3(dataset_errors.pda);
	for i = 1:numel(h)
		index = logical(kron(isnan(dataset_errors.pda(:,i)),ones(6,1)));
		zData = get(h(i),'ZData');
		zData(index,:) = nan;
		set(h(i),'ZData',zData);
	end
	zlabel('Rel. error: PDA');
	ylabel('Data points');
	ylim([1 size(dataset_errors.pda, 1)]);
	title(sprintf('%s', resultFiles{file_ind}));
	zlim([-1 1]);
	view([-90 0]);
	if exportFlag
		temp = strsplit(resultFiles{file_ind}, '.');
		exportThisPlot('name', [temp{1} '-Error-PDA'], 'plotPath', fullfile(resultFolder, 'graphs'), 'plotType', 'fig')
	end
	
	figure();
	h = bar3(dataset_errors.avg);
	for i = 1:numel(h)
		index = logical(kron(isnan(dataset_errors.avg(:,i)),ones(6,1)));
		zData = get(h(i),'ZData');
		zData(index,:) = nan;
		set(h(i),'ZData',zData);
	end
	zlabel('Rel. error: Avg');
	ylabel('Data points');
	ylim([1 size(dataset_errors.avg, 1)]);
	title(sprintf('%s', resultFiles{file_ind}));
	zlim([-1 1]);
	view([-90 0]);
	if exportFlag
		temp = strsplit(resultFiles{file_ind}, '.');
		exportThisPlot('name', [temp{1} '-Error-Avg'], 'plotPath', fullfile(resultFolder, 'graphs'), 'plotType', 'fig')
	end
end