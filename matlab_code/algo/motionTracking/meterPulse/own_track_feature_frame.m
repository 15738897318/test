function [currpos] = own_track_feature_frame(currframe, refframe, params, cv_package)
    feature_points = params;
    
    switch cv_package
	    case 'opencv'
		    currpos = cv.calcOpticalFlowPyrLK(refframe, currframe, ...
		    								  feature_points, ...
		                                      'MaxLevel', 3, ...
		                                      'WinSize', [21 21]);

		case 'native'
			
	end
end