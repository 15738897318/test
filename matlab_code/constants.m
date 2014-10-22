recordingTime = 30; %seconds

eulerian_filter_length = 31;
beat_filter_length = 15;

%%------- Eulerian-magnification parameters

% Control params for the algorithm
alpha = 50; %Eulerian magnifier %Standard: < 50
pyr_level = 6; %Standard: 6, but updated by the real frame size
min_hr = 30; %BPM %Standard: 30
max_hr = 240; %BPM %Standard: > 150
frame_rate = 30; %Standard: 30, but updated by the real frame-rate
chroma_magnifier = 1; %Standard: 1

C_matrix = [1.0000, 0.9562, 0.6214,
            1.0000, -0.2727, -0.6468,
            1.0000, -1.1037, 1.7006] * ...
			alpha * [1, 0, 0,
					0, chroma_magnifier, 0,
					0, 0, chroma_magnifier] * ...
			[0.299, 0.587, 0.114,
             0.596, -0.274, -0.322,
             0.211, -0.523, 0.312];


% Native params of the algorithm
number_of_channels = 3;
Gpyr_filter_length = 5;
startFrame = 1;
endFrame = 0; %Positive number to get definite end-frame, negative number to get end-frame relative to stream length

% filter_bandpassing:
%H = design(fdesign.bandpass('N,Fc1,Fc2,Ast1,Ap,Ast2', eulerian_filter_length - 1, (min_hr/ 60) / (frame_rate / 2), (max_hr/ 60) / (frame_rate / 2), 60, 0.5, 60), 'ALLFIR');
%eulerianTemporalFilterKernel = H(1).Numerator; % / sum(H(1).Numerator);

%Same as above, filter_length = 31
%eulerianTemporalFilterKernel = [-0.0083; -0.0183; -0.0234; -0.0209; -0.0115; -0.0010;  0.0003; -0.0155; -0.0461; -0.0759; -0.0824; -0.0487;  0.0249;  0.1173;  0.1942;  0.2241;  0.1942;  0.1173;  0.0249; -0.0487; -0.0824; -0.0759; -0.0461; -0.0155;  0.0003; -0.0010; -0.0115; -0.0209; -0.0234; -0.0183; -0.0083];

%%------- HR calculation parameters

% Control params for the algorithm
window_size_in_sec = 10;
overlap_ratio = 0;
max_bpm = 200; %BPM
cutoff_freq = 2.5; %Hz
time_lag = 3; %seconds

colourspace = 'tsl';
channels_to_process = 1 : 3; %If only 1 channel: 2 for tsl, 1 for rgb


% heartRate_calc: Native params of the algorithm
flagDebug = 1;
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


% frames2signal: 
%Native params for the 'trimmed-mean' conversion method
trimmed_size = 30;

%Native params for the 'mode-balance' conversion method
training_time_range = [0, 0.2]; %seconds %Double
number_of_bins = 50; %50 * round(frame_rate * training_time);
pct_reach_below_mode = 45; %Percent %Double
pct_reach_above_mode = 45; %Percent %Double

% frames2signal: Filtering kernel for the beat signal
%H = design(fdesign.lowpass('N,Fp,Fst', beat_filter_length - 1, cutoff_freq / (frame_rate / 2), 1.1 * cutoff_freq / (frame_rate / 2)), 'ALLFIR');
%beatSignalFilterKernel = H(2).Numerator / sum(H(2).Numerator);

%Same as above, with cutoff_freq / frame_rate = 3 / 30, filter_length = 15
beatSignalFilterKernel = [-0.0447, -0.0389, -0.0106, 0.0378, 0.0975, 0.1554, 0.1973, 0.2125, 0.1973	0.1554, 0.0975, 0.0378, -0.0106, -0.0389, -0.0447];
beatSignalFilterKernel = [-0.0265, -0.0076, 0.0217, 0.0580, 0.0956, 0.1285, 0.1509, 0.1589, 0.1509, 0.1285, 0.0956, 0.0580, 0.0217, -0.0076, -0.0265];