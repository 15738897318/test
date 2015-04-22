addpath(genpath('../'));

% Read video
vid = VideoReader(vidFile);

% Extract video info
vidHeight = vid.Height;
vidWidth = vid.Width;
fr = 40; vid.FrameRate;
len = vid.NumberOfFrames;
nChannels = 3;

fr_ratio = 1;

% Define the indices of the frames to be processed
startIndex = 1 * fr;
endIndex = 20 * fr; startIndex + 199;

array = uint8([]);
frame_index = 0;
for i  = startIndex : fr_ratio : endIndex
	frame = read(vid, i);
    frame_index = frame_index + 1;
	array(frame_index, :, :, :) = frame;
end

fr = fr / fr_ratio;
% Update the frame indices
startIndex = startIndex / fr_ratio;
endIndex = endIndex / fr_ratio;

temporal_means = [];
for color = 1 : 3
	monoframes = array(:, :, :, color);

	monoframes = permute(monoframes, [2, 3, 1]);

	% Selection parameters
	training_time = [0, 2];
	lower_pct_range = 50; %Double
	upper_pct_range = 50; %Double

	% Find the mode of the pixel values in the first few frames
	stretched_first_frame = reshape(monoframes(:, :, round(fr * training_time(1)) + 1 : round(fr * training_time(2))), ...
											[size(monoframes, 1) * size(monoframes, 2) * (round(fr * training_time(2)) - round(fr * training_time(1))), 1]);
	stretched_first_frame = single(stretched_first_frame);
    
    %Double 1-D vector
	[counts, centres] = hist(stretched_first_frame, 50);
	%[Int vector, Double vector] 
	[~, argmax] = max(counts); %Int
	centre_mode = centres(argmax); %Double
	
	% Find the percentile range centred on the mode
	percentile_of_centre_mode = invprctile(stretched_first_frame, centre_mode); %Double
	percentile_range = [max(0, percentile_of_centre_mode - lower_pct_range), min(100, percentile_of_centre_mode + upper_pct_range)]; %Double 2-element vector
	
	% Correct the percentile range for the boundary cases
	if percentile_range(2) == 100
		percentile_range(1) = 100 - (lower_pct_range + upper_pct_range);
	end
	if percentile_range(1) == 0
		percentile_range(2) = (lower_pct_range + upper_pct_range);
	end
	
	% Convert the percentile range into pixel-value range
	range = prctile(stretched_first_frame, percentile_range); %Double 2-element vector
	range = uint8(range);
    
	% For each video frame, values outside the range are rejected
	monoframes((monoframes < range(1)) | (monoframes > range(2))) = NaN;
    
    temporal_mean = [];
	% Calculate the average of each frame
    for i = 1 : size(monoframes, 3)
        temp = monoframes(:, :, i);
        temp = double(temp);
        temporal_mean(i) = sum(sum(temp(isfinite(temp)))) / sum(sum(isfinite(temp)));
    end
    
    temporal_means(:, color) = temporal_mean;
end
time_vect = linspace(startIndex / fr, endIndex / fr, size(temporal_means, 1));


spectral_power_means = abs(fft(bsxfun(@minus, temporal_means, mean(temporal_means, 1))));
freq_vect = linspace(0, fr, size(spectral_power_means, 1));
half_range = [2 : ceil(size(spectral_power_means, 1) / 2)];


colors = 'rgb';
[~, title_str, ~] = fileparts(vidFile);
figure()
subplot(2, 1, 1)
hold('on')
for i = 1 : size(temporal_means, 2)
	plot(time_vect, temporal_means(:, i), 'DisplayName', ['Chan ', num2str(i)], 'Color', colors(i))
end
hold('off')
xlabel('Time (sec)')
ylabel('Light intensity (A.U.)')
title(title_str)

subplot(2, 1, 2)
hold('on')
for i = 1 : size(spectral_power_means, 2)
	plot(freq_vect(half_range), log(spectral_power_means(half_range, i)), 'DisplayName', ['Chan ', num2str(i)], 'Color', colors(i))
end
hold('off')
xlabel('Frequency (Hz)')
ylabel('Log of DC-suppressed spectral power (A.U.)')


%============= STFT
figure()
for i = 1 : size(temporal_means, 2)
	x = temporal_means(:, i);                        % get the first channel
	xmax = max(abs(x));                 % find the maximum abs value
	x = x/xmax;                         % scalling the signal

	% define analysis parameters
	xlen = length(x);                   % length of the signal
	wlen = 2^7;                        % window length (recomended to be power of 2)
	h = wlen/4;                         % hop size (recomended to be power of 2)
	nfft = 2^7;                        % number of fft points (recomended to be power of 2)

	% define the coherent amplification of the window
	K = sum(hamming(wlen, 'periodic'))/wlen;

	% perform STFT
	[s, f, t] = stft(x, wlen, h, nfft, fr);

	% take the amplitude of fft(x) and scale it, so not to be a
	% function of the length of the window and its coherent amplification
	s = abs(s)/wlen/K;

	% correction of the DC & Nyquist component
	if rem(nfft, 2)                     % odd nfft excludes Nyquist point
	    st(2:end, :) = s(2:end, :).*2;
	else                                % even nfft includes Nyquist point
	    s(2:end-1, :) = s(2:end-1, :).*2;
	end

	% convert amplitude spectrum to dB (min = -120 dB)
	s = 20*log10(s + 1e-6);

	% plot the spectrogram
	subplot(size(temporal_means, 2), 1, i)
	imagesc(t, f, s);
	set(gca,'YDir','normal')
	xlabel('Time, s')
	ylabel('Frequency, Hz')
	title([title_str, ' - Chan ', num2str(i)])
end


