input_format = 'frames';
full_pyramid = 1;
pyramid_style = 'steerable';


%%%==== Functions
addpath(genpath('./matlabPyrTools'));

if ~full_pyramid
	%%== Single-layer pyramid
	switch input_format
		case 'video'
			%%-- Video-based processing			
			switch pyramid_style
				case 'gaussian'
					func_build_pyramid_1band = @build_gPyr_1band_video_v1;
					func_magnify_pyr = @magnify_linear_pyr_1band_video_v1;
			end
			
			func_heartRate_calc = @heartRate_calc_video_v1;
		
		case 'frames'
			%-- Frame-based processing
			switch pyramid_style
				case 'gaussian'
					func_build_pyramid_1band = @build_gPyr_1band_frames_v1;
					func_magnify_pyr = @magnify_linear_pyr_1band_frames_v1;
			end
			
			func_heartRate_calc = @heartRate_calc_frames_v1;
	end
else
	%%== Full-stack pyramid
	switch input_format
		case 'frames'
		%-- Frame-based processing
			global func_build_pyramid_allband
			global func_make_pyr
			global func_recon_pyr
	
			func_build_pyramid_allband = @build_pyr_allband_frames_v1;
			
			switch pyramid_style
				case 'gaussian'
					func_make_pyr = @buildGpyr;
					func_recon_pyr = @reconGpyr;
					func_magnify_pyr = @magnify_linear_pyr_allband_frames_v1;
				
				case 'laplacian'
					func_make_pyr = @buildLpyr;
					func_recon_pyr = @reconLpyr;
					func_magnify_pyr = @magnify_linear_pyr_allband_frames_v1;
				
				case 'steerable'
					func_make_pyr = @buildSpyr;
					func_recon_pyr = @reconSpyr;
					func_magnify_pyr = @magnify_phase_pyr_allband_frames_v1;
			end
			
			func_heartRate_calc = @heartRate_calc_frames_v1;
	end
end

%%%==== Constants
switch pyramid_style
	case 'gaussian'
		constants_gaussian_v1;
	case 'laplacian'
		constants_laplacian_v1;
	case 'steerable'
		constants_steerable_v1;
end