function [currpos] = mit_track_feature_frame(currframe, refframe, params, cv_package)
    feature_points = params;
    
    switch cv_package
	    case 'opencv'
		    currpos = cv.calcOpticalFlowPyrLK(refframe, currframe, ...
		    								  feature_points, ...
		                                      'MaxLevel', 0);

		case 'native'
	end
end