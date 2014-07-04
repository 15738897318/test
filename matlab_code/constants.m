%%------- Eulerian-magnification parameters

% Control params for the algorithm
alpha = 30; %Eulerian magnifier %Standard: < 50
pyr_level = 6; %Standard: 6, but updated by the real frame size
min_hr = 30; %BPM %Standard: 30
max_hr = 240; %BPM %Standard: > 150
frame_rate = 30; %Standard: 30, but updated by the real frame-rate
chroma_magnifier = 1; %Standard: 1


% Native params of the algorithm
number_of_channels = 3;
Gpyr_filter_length = 5;
startFrame = 1;
endFrame = - 10; %Positive number to get definite end-frame, negative number to get end-frame relative to stream length

% filter_bandpassing:
eulerianTemporalFilterKernel = [0.0034; 0.0087; 0.0244; 0.0529; 0.0909; 0.1300; 0.1594; 0.1704; 0.1594; 0.1300; 0.0909; 0.0529; 0.0244; 0.0087; 0.0034];


%%------- HR calculation parameters

% Control params for the algorithm
window_size_in_sec = 10;
overlap_ratio = 0;
max_bpm = 200; %BPM
cutoff_freq = 5; %Hz
time_lag = 3; %seconds

colourspace = 'tsl';
channels_to_process = 1 : 3; %If only 1 channel: 2 for tsl, 1 for rgb


% heartRate_calc: Native params of the algorithm
flagDebug = 0;
flagGetRaw = 0;

startIndex = 1; %400	%Int
endIndex = 0; %1400	%Int  %Positive number to get definite end-frame, negative number to get end-frame relative to stream length

peakStrengthThreshold_fraction = 0; %Double
frames2signalConversionMethod = 'mode-balance'; %String

%frame_downsampling_filt = fspecial('gaussian', [7 7], 2.5); %Double array
frame_downsampling_filt = [0.0085, 0.0127, 0.0162, 0.0175, 0.0162, 0.0127, 0.0085;...
						   0.0127, 0.0190, 0.0241, 0.0261, 0.0241, 0.0190, 0.0127;...
						   0.0162, 0.0241, 0.0307, 0.0332, 0.0307, 0.0241, 0.0162;...
						   0.0175, 0.0261, 0.0332, 0.0360, 0.0332, 0.0261, 0.0175;...
						   0.0162, 0.0241, 0.0307, 0.0332, 0.0307, 0.0241, 0.0162;...
						   0.0127, 0.0190, 0.0241, 0.0261, 0.0241, 0.0190, 0.0127;...
						   0.0085, 0.0127, 0.0162, 0.0175, 0.0162, 0.0127, 0.0085];


% frames2signal: Native params for the 'mode-balance' conversion method in 
training_time_range = [0.5, 3]; %seconds %Double
number_of_bins = 50; %50 * round(fr * training_time);
pct_reach_below_mode = 45; %Percent %Double
pct_reach_above_mode = 45; %Percent %Double

% frames2signal: Filtering kernel for the beat signal
%H = design(fdesign.lowpass('N,Fp,Fst', 14, cutoff_freq / fr, 1.1 * cutoff_freq / fr), 'ALLFIR');
%beatSignalFilterKernel = H(2).Numerator / sum(H(2).Numerator);

%Same as above, with cutoff_freq / fr = 5 / 30
beatSignalFilterKernel = [-0.0265, -0.0076, 0.0217, 0.0580, 0.0956, 0.1285, 0.1509, 0.1589, 0.1509, 0.1285, 0.0956, 0.0580, 0.0217, -0.0076, -0.0265];