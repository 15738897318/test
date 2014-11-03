%%%==== Functions
addpath('./matlabPyrTools');
addpath('./matlabPyrTools/MEX');

%-- Video-based processing
func_eulerianGaussianPyramidMagnification = @eulerianGaussianPyramidMagnification_video_v1;
func_build_GDown_stack = @build_GDown_stack_video_v1;
func_heartRate_calc = @heartRate_calc_video_v1;

%-- Frame-based processing
%func_eulerianGaussianPyramidMagnification = @eulerianGaussianPyramidMagnification_frames_v1;
%func_build_GDown_stack = @build_GDown_stack_frames_v1;
%func_heartRate_calc = @heartRate_calc_frames_v1;


%%%==== Constants
constants_v1;