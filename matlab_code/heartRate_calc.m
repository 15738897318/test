% window_size = 90;
% overlap_ratio = 0;
%	'/Users/misfit/Desktop/Codes - Local/Working bench/bioSignalProcessing/eulerianMagnifcation/codeMatlab/Results/sample-Dung-finger-ideal-from-0.83333-to-1.5-alpha-50-level-4-chromAtn-1.avi'


function hr_array = heartRate_calc(vidFile, window_size_in_sec, overlap_ratio, max_bpm, cutoff_freq, colour_channel, ref_reading, colourspace, time_lag)
	
	close all
	
	debug = 1;
	getRaw = 0;
	threshold_fraction = 0;
	conversion_method = 'mode-balance';
	
	%% Block 1 ==== Load the video & convert it to the desired colour-space
	% Get the filename-only part of the full path
	[~, vidName] = fileparts(vidFile);
	display(sprintf('Processing file: %s', vidName));
								   
	% Read video
	vid = VideoReader(vidFile);

	% Extract video info
	vidHeight = vid.Height;
	vidWidth = vid.Width;
	nChannels = 3;
	fr = vid.FrameRate;
	len = vid.NumberOfFrames;
	
	window_size = round(window_size_in_sec * fr);
	firstSample = round(fr * time_lag);
	
	% Define the indices of the frames to be processed
	startIndex = 1; %400
	endIndex = len; %1400
	
	% Convert colourspaces for each frame
	k = 0;
	filt = fspecial('gaussian', [7 7], 2.5);
	clearvars output_array
	clearvars colorframe
	clearvars colorframes
	clearvars monoframe
	clearvars monoframes
	temp = struct('cdata', zeros(vidHeight, vidWidth, nChannels, 'uint8'),...
				  'colormap', []);
	for i = startIndex : endIndex
		k = k + 1;
		
		% Extract the ith frame in the video stream
		temp.cdata = read(vid, i);
		% Convert the extracted frame to RGB image
		[rgbframe, ~] = frame2im(temp);
		
		switch colourspace
			case 'rgb'
				colorframe = rgbframe;
			case 'hsv'
				colorframe = rgb2hsv(rgbframe);
			case 'ntsc'
				colorframe = rgb2ntsc(rgbframe);
			case 'ycbcr'
				colorframe = rgb2ycbcr(rgbframe);
			case 'tsl'
				colorframe = rgb2tsl(rgbframe);
        end
        
		if getRaw
			colorframes(:, :, :, k) = colorframe;
			monoframes(:, :, k) = squeeze(colorframe(:, :, colour_channel))
		
		else
			%monoframes(:, :, k) = squeeze(double(colorframe(:, :, colour_channel)));
			
			% Downsample the frame for ease of computation
			%monoframe = filt_img(squeeze(double(colorframe(:, :, colour_channel))));
			%monoframes(:, :, k) = monoframe(1 : 4 : end, 1 : 4 : end);
			
			monoframe = squeeze(double(colorframe(:, :, colour_channel)));
			monoframes(:, :, k) = corrDn(monoframe, filt, 'reflect1', [4 4], [1 1], size(monoframe));
			
			%colorframe = colorframe(1 * round(size(colorframe, 1) / 4) + 1 : 2 * round(size(colorframe, 1) / 4), 1 * round(size(colorframe, 2) / 4) + 1 : 2 * round(size(colorframe, 2) / 4), :);
			%monoframes(:, :, k) = squeeze(colorframe(:, :, colour_channel));
		end
	end
	
	
	%% Block 2 ==== Extract a signal stream & pre-process it
	% Convert the video frames into double for processing
	monoframes = double(monoframes);
	
	% Convert the frame stream into a 1-D signal
	[temporal_mean, debug_frames2signal] = frames2signal(monoframes, conversion_method, fr, cutoff_freq);
	
	%% Block 3 ==== Heart-rate calculation
	% Set peak-detection params
    threshold = threshold_fraction * max(temporal_mean(firstSample : end));
    minPeakDistance = round(60 / max_bpm * fr);
	
	% Calculate heart-rate using peak-detection on the signal
	[avg_hr_pda, debug_pda] = hr_calc_pda(temporal_mean, fr, firstSample, window_size, overlap_ratio, minPeakDistance, threshold);
	
	% Calculate heart-rate using peak-detection on the signal
	[avg_hr_autocorr, debug_autocorr] = hr_calc_autocorr(temporal_mean, fr, firstSample, window_size, overlap_ratio, minPeakDistance);
	
	
	%% ============ Function output and summary
	% Display the average rate using total peak counts on the full stream
	[~, peak_locs] = findpeaks(temporal_mean(firstSample : end), 'MINPEAKDISTANCE', minPeakDistance, 'THRESHOLD', threshold);
	disp('Average heart-rate: ');
	disp(length(peak_locs) / length(temporal_mean(firstSample : end)) * fr * 60);
	
	% Output of the function
	hr_array = [ref_reading, colour_channel, avg_hr_autocorr, avg_hr_pda];
	
	
	%% ============ Debug
	if debug
		heartBeats_autocorr = debug_autocorr.heartBeats;
		heartRates_autocorr = debug_autocorr.heartRates;
		autocorrelation = debug_autocorr.autocorrelation;
		
		heartBeats_pda = debug_pda.heartBeats;
		heartRates_pda = debug_pda.heartRates;
		
		% Calculate the time & freq vectors
		time_vector = linspace(0, length(temporal_mean) / fr, length(temporal_mean));
		freq_vector = linspace(0, fr, length(temporal_mean));
	
		h(1) = figure();
		ax(1) = subplot(3, 1, 1);
		hold('on');
		plot(time_vector, temporal_mean, 'b', 'DisplayName', '1-D signal')
	
		if ~isempty(heartBeats_autocorr)
			stem(time_vector(heartBeats_autocorr(:, 2)), heartBeats_autocorr(:, 1), 'bx', 'DisplayName', 'AC peaks')
		end
		if ~isempty(heartBeats_pda)
			stem(time_vector(heartBeats_pda(:, 2)), heartBeats_pda(:, 1), 'ro', 'DisplayName', 'PD peaks') 
		end
		hold('off');
		title(sprintf([vidName '\nChannel: %d (%s)'], colour_channel, colourspace))
		xlim([min(time_vector) max(time_vector)])
		xlabel('Time (sec)')
		ylabel('Frame average')

		ax(2) = subplot(3, 1, 2);
		hold('on');
		plot(time_vector(1 : length(autocorrelation)), autocorrelation)
		if ~isempty(heartBeats_autocorr)
			stem(time_vector(heartBeats_autocorr(:, 2)), heartBeats_autocorr(:, 1), 'bx', 'DisplayName', 'AC peaks')
		end
		hold('off');
		xlim([min(time_vector) max(time_vector)])
		xlabel('Time (sec)')
		ylabel('Sliding-window autocorrelation')

		ax(3) = subplot(3, 1, 3);
		hold('on');
		plot(time_vector(1 : length(heartRates_autocorr)), heartRates_autocorr * 60, 'b', 'DisplayName', 'Autocorrelation')
		plot(time_vector(1 : length(heartRates_pda)), heartRates_pda * 60, 'r', 'DisplayName', 'Peak detection')
	
		plot(time_vector(firstSample : length(heartRates_autocorr)), cumsum(heartRates_autocorr(firstSample : end) * 60) ./ (1 : length(heartRates_autocorr(firstSample : end) * 60)), 'b-.', 'DisplayName', 'Autocorrelation - Running avg')
		plot(time_vector(firstSample : length(heartRates_pda)), cumsum(heartRates_pda(firstSample : end) * 60) ./ (1 : length(heartRates_pda(firstSample : end) * 60)), 'r-.', 'DisplayName', 'Peak detection - Running avg')
		if isfinite(ref_reading)
			plot(time_vector([firstSample, length(heartRates_autocorr)]), [ref_reading, ref_reading], 'k-', 'DisplayName', 'Reference reading')
		end
	
		hold('off');
		xlim([min(time_vector) max(time_vector)])
		xlabel('Time (sec)')
		ylabel('Locally-averaged heart-rate (BPM)')
		title(sprintf(['Average heart-rate: ' num2str(avg_hr_autocorr) ' BPM & ' ...
									  num2str(avg_hr_pda) ' BPM' ...
					   '\n Reference: ' num2str(ref_reading) ' BPM']));
		legend('show')
	
		linkaxes(ax, 'x')
		fig_filename = [vidFile '-' colourspace '-cchan-' num2str(colour_channel) '-1.fig'];
		saveas(h(1), fig_filename, 'fig')
	
		h(2) = figure();
		subplot(3, 1, 1)
		hold('on');
		plot(time_vector, temporal_mean, 'b', 'DisplayName', '1-D signal')
		hold('off');
		title(sprintf([vidName '\nChannel: %d (%s)'], colour_channel, colourspace))
		xlim([min(time_vector) max(time_vector)])
		xlabel('Time (sec)')
		ylabel('Frame average')

		subplot(3, 1, 2)
		hold('on');
		plot(freq_vector - fr / 2, abs(fftshift(fft(temporal_mean - mean(temporal_mean)))), 'r', 'DisplayName', 'FFT of 1-D signal')
		hold('off');
		xlim([min(freq_vector - fr / 2) max(freq_vector - fr / 2)])
		xlim([0 +2.5])
		xlabel('Frequency (Hz)')

		window_size = 256;
		overlap_ratio = 7 / 8;
		subplot(3, 1, 3)
		spectrogram(temporal_mean - mean(temporal_mean), window_size, round(overlap_ratio * window_size), 2^(ceil(log(window_size) / log(2))), fr)
		xlim([0 2.5])
		view(90, -90)
		title('Spectrogram of the 1-D signal')
	
		fig_filename = [vidFile '-' colourspace '-cchan-' num2str(colour_channel) '-2.fig'];
		saveas(h(2), fig_filename, 'fig')
		
		
		% Histogram
		[counts, centres] = hist(reshape(debug_frames2signal.monoframes, [size(debug_frames2signal.monoframes, 1) * size(debug_frames2signal.monoframes, 2), size(debug_frames2signal.monoframes, 3)]), 50);
		figure()
		surf([1 : size(debug_frames2signal.monoframes, 3)], centres, counts, 'LineStyle', 'none');
		view([0 90]);
		xlabel('Frame number');
		ylabel(['Bin of values for channel ' colour_channel]);
	end