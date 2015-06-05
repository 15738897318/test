src_folder = '~/Desktop/Codes - Local/Active/bioSignal/Codes/main/refMIT/vidData/';
vidFile = 'baby.mp4';
vidFile = 'baby-iir-r1-0.4-r2-0.05-alpha-10-lambda_c-6-chromAtn-0.1.mp4';
% vidFile = 'baby-ideal-from-0.05-to-0.5-alpha-150-level-6-chromAtn-0.avi';

vidFile = 'baby2-ideal-from-2.3333-to-2.6667-alpha-150-level-6-chromAtn-1.mp4';
% vidFile = 'baby2.mp4';

%% ======
addpath(genpath('../tools'));
addpath(genpath('./'));

% Constants
constants_gaussian_v1;
constants.method = 'canny';
constants.threshold = 0;
constants.threshold_fraction = 0.5;
constants.filter = beatSignalFilterKernel;

firstSample = 20;
window_size = 50;
overlap_ratio = 0;
max_bpm = 80;


%% ======Summarise the video into a 1-D signal sig(k)
signal = edge_summariser_video(fullfile(src_folder, vidFile), constants);

figure();
plot(signal);


%% ======Band-pass filter the 1-D signal to include only the relevant frequencies
signal = edge_temporal_filter(signal, constants);

figure();
plot(signal);


%% ======Calculate the rate
% Set peak-detection params
threshold = threshold_fraction * max(sig(firstSample : end));
minPeakDistance = max(round(60 / max_bpm * vidFR), 1);

% Calculate rate using peak-detection on the signal
[beats_pda, avg_rate_simple_pda, debug_beats_pda] = beat_counter_pda(sig(firstSample : end), vidFR, firstSample, window_size, overlap_ratio, minPeakDistance, threshold);
[rates_pda, debug_rate_pda] = rate_calculator(beats_pda, vidFR);

% Calculate rate using peak-detection on the signal
[beats_autocorr, avg_rate_simple_autocorr, debug_beats_autocorr] = beat_counter_autocorr(sig(firstSample : end), vidFR, firstSample, window_size, overlap_ratio, minPeakDistance, threshold);
[rates_autocorr, debug_rate_autocorr] = rate_calculator(beats_autocorr, vidFR);


%% ======Function output and summary
rate_pda = round(rates_pda.average);
disp(sprintf('Average rate (PDA): %f', rate_pda));

rate_autocorr = round(rates_autocorr.average);
disp(sprintf('Average rate (ACF): %f', rate_autocorr));

% Display the average rate using total peak counts on the full stream
[~, peak_locs] = findpeaks(sig(firstSample : end), 'MINPEAKDISTANCE', minPeakDistance, 'THRESHOLD', threshold);
avg_rate = length(peak_locs) / length(sig(firstSample : end)) * vidFR * 60;
disp(sprintf('Average rate: %f', avg_rate));