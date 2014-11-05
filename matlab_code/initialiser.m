%%%==== Functions
addpath('./matlabPyrTools');
addpath('./matlabPyrTools/MEX');

%%-- Video-based processing
%func_magnify_euler_pyr = @magnify_euler_pyr_video_v1;
%func_build_pyramid = @build_gPyr_video_v1;
%func_heartRate_calc = @heartRate_calc_video_v1;

%-- Frame-based processing
func_magnify_euler_pyr = @magnify_euler_pyr_frames_v1;
func_build_pyramid = @build_gPyr_frames_v1;
func_heartRate_calc = @heartRate_calc_frames_v1;


%%%==== Constants
constants_v1;