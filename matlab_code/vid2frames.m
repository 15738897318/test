vidFolder = '/Users/misfit/Desktop/Codes - Local/Code - Active/bioSignalProcessing/eulerianMagnifcation/codeMatlab/Videos';
vidFiles = dir(vidFolder);
vidFiles = {vidFiles(~[vidFiles(:).isdir]).name};
vidFiles(ismember(vidFiles, {'.', '..', '.DS_Store'})) = [];

for index = 1 : length(vidFiles)
	vidFile = vidFiles{index};
	
	vid = VideoReader(fullfile(vidFolder, vidFile));
	
	[~, vidFile, ~] = fileparts(vidFile);
	if exist(fullfile(vidFolder, vidFile))
    	rmdir(fullfile(vidFolder, vidFile), 's');
    end
    mkdir(fullfile(vidFolder, vidFile));
	
	fprintf(fopen(fullfile(vidFolder, vidFile, 'vid_specs.txt'), 'wt'), sprintf('%d, %d', vid.NumberOfFrames, vid.FrameRate));
	
	fprintf([fullfile(vidFolder, vidFile) '\n']);
	for i = 1 : vid.NumberOfFrames
        frame = read(vid, i);

		filename = fullfile(vidFolder, vidFile, ...
							['frame_' num2str(i) '.png']);
		imwrite(frame, filename, 'png');
	end;
end;