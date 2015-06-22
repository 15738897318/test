global pyramid_style
global filter_file

input_format = 'frames';
full_pyramid = 0;
pyramid_style = 'gaussian';
frame_size = []; [128 128]; %Or empty array to avoid resizing the images


%%%==== Functions
addpath(genpath('../../../tools'))
addpath(genpath('./'));

if ~full_pyramid
	%%== Single-layer pyramid
	switch input_format
		case 'video'
			%%-- Video-based processing			
			switch pyramid_style
				case 'gaussian'
					func_build_pyramid = @build_gPyr_1band_video_v1;
					func_magnify_pyr = @magnify_linear_pyr_1band_video_v2;
			end
			
			func_rate_calc = @rate_calc_video_v1;
		
		case 'frames'
			%-- Frame-based processing
			switch pyramid_style
				case 'gaussian'
					func_build_pyramid = @build_gPyr_1band_frames_v1;
					func_magnify_pyr = @magnify_linear_pyr_1band_frames_v2;
			end
			
			func_rate_calc = @rate_calc_frames_v1;
	end
else
	%%== Full-stack pyramid
	switch input_format
		case 'frames'
		%-- Frame-based processing
			% global func_build_pyramid
			global func_make_pyr
			global func_recon_pyr
	
			func_build_pyramid = @build_pyr_allband_frames_v1;
			
			switch pyramid_style
				case 'gaussian'
					func_make_pyr = @buildGpyr;
					func_recon_pyr = @reconGpyr;
					func_magnify_pyr = @magnify_linear_pyr_allband_frames_v1;
				
				case 'laplacian'
					func_make_pyr = @buildLpyr;
					func_recon_pyr = @reconLpyr;
					func_magnify_pyr = @magnify_linear_pyr_allband_frames_v2;
				
				case 'steerable'
					func_make_pyr = @buildSpyr;
					func_recon_pyr = @reconSpyr;
					func_magnify_pyr = @magnify_phase_pyr_allband_frames_v2;
					
					filter_file = 'sp3Filters'; %Accepted: 'sp1Filters', 'sp3Filters', 'sp5Filters'
			end
			
			func_rate_calc = @rate_calc_frames_v1;
	end
	
	amp_type = 'adaptive';
	func_amplify_pyr = @amplifyPyr_allband_v1;
end

%%%==== Constants
switch pyramid_style
	case 'gaussian'
		constants_gaussian_v1;
	case 'laplacian'
		constants_laplacian_v1;
	case 'steerable'
		constants_steerable_v1;
	otherwise
		constants_gaussian_v1;
end