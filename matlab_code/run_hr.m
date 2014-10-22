% Notes:
% - Basis takes 15secs to generate an HR estimate
% - Cardiio takes 30secs to generate an HR estimate


%Finger
file_template = '2014-06-10-Dung-Finger_crop-ideal-from-0.5-to-4-alpha-50-level-4-chromAtn-1.avi';
file_template = '2014-06-10-Dung-Finger_crop-ideal-from-0.5-to-4-alpha-30-level-6-chromAtn-1.avi';
file_template = '2014-06-12-Loc-Finger_crop-ideal-from-0.5-to-4-alpha-30-level-6-chromAtn-1';
file_template = '2014-06-12-Trung-Finger_crop-ideal-from-0.5-to-4-alpha-30-level-6-chromAtn-1';
file_template = '2014-06-16-Dung-Finger_crop-ideal-from-0.5-to-4-alpha-30-level-6-chromAtn-1';
file_template = '2014-06-16-Nam-Finger_crop-ideal-from-0.5-to-4-alpha-30-level-6-chromAtn-1';
file_template = '2014-06-17-Loc-Finger_crop-ideal-from-0.5-to-4-alpha-30-level-6-chromAtn-1';
%Face
file_template = '2014-06-10-Hoang-Face_crop-ideal-from-0.5-to-4-alpha-50-level-6-chromAtn-1.avi';
file_template = '2014-06-10-Hoang-Face_crop-ideal-from-0.5-to-4-alpha-30-level-6-chromAtn-1.avi';
file_template = '2014-06-17-Loc-Face_crop-ideal-from-0.5-to-4-alpha-30-level-6-chromAtn-1.avi';


file_template = 'test1C*.mp4';
%file_template = 'finger - 2014-07-11-14-41-54-ideal-from-0.5-to-4-alpha-50-level-6-chromAtn-1*.avi';
%file_template = '2014-06-10-Self-Face_crop*.avi';
%file_template = '2014*.avi';
%file_template = '2014-06-10-Hoang-Face_crop-ideal-from-0.5-to-4-alpha-50-level-6-chromAtn-1.avi';


task_desc = 'Params testing with the corrected correlation function';

install;

window_size_in_sec = 10;
overlap_ratio = 0;
max_bpm = 200; %BPM
cutoff_freq = 5; %Hz
time_lag = 3; %seconds

results_file = 'hr_results.csv';

src_folder = '/Users/misfit/Desktop/Codes - Local/Code - Active/bioSignalProcessing/eulerianMagnifcation/codeMatlab/Results/';
%src_folder = '/Users/misfit/Desktop/Codes - Local/Code - Active/bioSignalProcessing/eulerianMagnifcation/Archives/Results/Demo quality/';

colourspace = 'tsl';
channels_to_process = 2;

file_list = dir([src_folder file_template]);

hr_array = {};
k = 0;
if ~exist([src_folder results_file], 'file')
	fileID = fopen([src_folder results_file], 'a');
	
	k = 1;
	hr_array{k} = 'ref_reading,channel,autocorr_reading,peak_detection_reading,type,colourspace,video_file,min_freq,max_freq,alpha,level,chromAtn';
	
	fprintf(fileID, '%s', hr_array{k});
else
	fileID = fopen([src_folder results_file], 'a');
end

for file_ind = 1 : length(file_list)
	display(sprintf('File %d of %d \n', file_ind, length(file_list)));
	
	vidFileName = [src_folder file_list(file_ind).name];
	
	% Video-processing params
	buffer = strsplit(file_list(file_ind).name, '-from-');
	buffer = strsplit(buffer{2}, '-');
	min_hr = buffer{1};
	
	buffer = strsplit(file_list(file_ind).name, '-to-');
	buffer = strsplit(buffer{2}, '-');
	max_hr = buffer{1};
	
	buffer = strsplit(file_list(file_ind).name, '-alpha-');
	buffer = strsplit(buffer{2}, '-');
	alpha = buffer{1};
	
	buffer = strsplit(file_list(file_ind).name, '-level-');
	buffer = strsplit(buffer{2}, '-');
	level = buffer{1};
	
	buffer = strsplit(file_list(file_ind).name, '-chromAtn-');
	buffer = strsplit(buffer{2}, '-');
	buffer = strsplit(buffer{1}, '.');
	chromAtn = buffer{1};
	
	% Run the heart-rate counting algo
	src_vidFile = strsplit(file_list(file_ind).name, '-');
	
	type = strsplit(src_vidFile{5}, '_');
	type = type{1};
	
	ref_reading_file = [src_folder strjoin(src_vidFile(1:4), '-') '-Basis.txt'];
	if exist(ref_reading_file, 'file')
		ref_reading = textread(ref_reading_file, '%s');
		ref_reading = str2num(ref_reading{end});
	else
		ref_reading = NaN;
	end
	
	for channel_ind = 1 : length(channels_to_process)
		k = k + 1;
		
		colour_channel = channels_to_process(channel_ind);
		
		hr_output = heartRate_calc(vidFileName, window_size_in_sec, overlap_ratio, max_bpm, cutoff_freq, colour_channel, ref_reading, colourspace, time_lag);
		
		hr_array{k} = [strjoin(cellstr(num2str(hr_output'))', ',') ',' ...
						type ',' colourspace ',' file_list(file_ind).name ',' ...
						min_hr ',' max_hr ',' alpha ',' level ',' chromAtn];
		
		fprintf(fileID, '\n%s', hr_array{k});
	end
end

fclose(fileID);
display(task_desc);