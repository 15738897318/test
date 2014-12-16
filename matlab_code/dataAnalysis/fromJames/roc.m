
function [bestthresh,sensitivity,specificity,auc,sens,onemspec] = roc(rocdata,thresholds,N,confmult)
%roc: 
%Creates ROC analysis curve(s) and calculates specificity and sensitivity data
%
%Usage: 
%[bestthresh,sensitivity,specificity,auc,sens,onemspec] = roc(rocdata,thresholds,N,confmult)
%
%General Description:
%roc operates in two modes. When the input argument N=1 the roc analysis is run one time
%using the original data.  When N>1, the input data is randomly resampled N times (bootstrapped)
%to create N data sets.  N ROC curves are then created for each data set.  Average and Stdev 
%is then calculuated across all N curves to create upper and lower confidence limits.  
%
%Input arguments:
%   rocdata:
%       roc data is a Mx2 matrix containing M epochs.  Column 1 contains the 
%       epoch score and must contain either 1 (positive) or 0 (negative). Column
%       2 is the numerical output of the algorithm.  The numerical output
%       must be >= 0.
%   thresholds:
%       A row vector which contains all the thresholds to run the roc analysis
%       for.  This input must be sorted and thresholds(1) must equal 0.
%   N:  
%       The number of times to resample rocdata set.  When N=1
%       no resampling is performed and only the original rocdata is used.
%   confmult:
%       The value to multiply stdev by when calculating confidence intervals, i.e:
%           1 = 68.3% confidence interval
%           2 = 95.4%  
%           3 = 99.7%
%           4 = 99.9%
%       confmult is ignored when N=1.
%       confmult must be within the range of 1 and 4.
%
%Output Arguments (when N=1)
%   bestthresh:
%       The threshold value which maximizes specificity and sensitivity.  This
%       is the point on the roc curve which is closest to (0,1).
%   sensitivity:
%       The sensitivity at the bestthresh
%   specificity:
%       The specificity at the bestthresh
%   auc:
%       The area under the ROC curve calculated using the trapz (numerical integration)
%       function.
%   sens:
%       A column vector containing the sensitivity for each threshold. sens(1) is the
%       sensitivity at thresholds(1). 
%   onemspec:
%       A column vector containing 1-specificity at each threshold. onemspec(1) is
%       1-specificity at thresholds(1).
%
%Output Arguments (when N>1)
%   bestthresh:
%       The threshold value which maximizes specificity and sensitivity.  This
%       is the point on the roc curve which is closest to (0,1). 
%   sensitivity:
%       A 3 element row vector, where: 
%           sensitivity(1) = mean sensitivity
%           sensitivity(2) = sensitivity lower confidence limit
%   specificity:
%       A 3 element row vector, where: 
%           specificity(1) = mean specificity
%           specificity(2) = specificity lower confidence limit
%   auc:
%       A 3 element row vector, where: 
%           col 1 = mean auc
%           col 2 = auc lower confidence limit
%   sens:
%       A 3 column matrix, where:
%           col 1 = mean sensitivity for each threshold
%           col 2 = lower sensitivity confidence limit for each threshold
%   onemspec:
%       A 3 column matrix, where:
%           col 1 = mean 1-specificity for each threshold
%           col 2 = lower 1-specificity confidence limit for each threshold
%
%Plotting Roc Curves:
%   For both N=1 and N>1 cases, type plot(sens,onespec).  For the
%   N=1 case one curve will be plotted.  For N>1 the mean, upper, and
%   lower confidence curves will be plotted.
%
%   See ROCPLOT which can be used to plot curves and display statistics
%
    
    if(nargin < 4)
        error('incorrect number of input arguments, type "help roc"')
    end
    
    %check thresholds vector
    %if(thresholds(1) ~= 0)
   %     error('thresholds(1) must equal 0');
   %else
%       if(length(unique(thresholds)) ~= length(thresholds))
%        error('thresholds must not contain duplicates');
%    elseif(sum( sort(thresholds) == thresholds ) ~= length(thresholds))
%        error('thresholds vector must be sorted');
%    end

   
    thresholds = sort(unique(thresholds));

    
    %check confmult data range
    if N ~= 1 && (confmult > 4 || confmult < 1 )
        error('confmult must be within the range of 1 and 4');
    end
    
    %check data type and range for N
    if (rem(N,1) ~= 0) || (N <= 0)
        error('N must be positive integer');
    end

    numDataPoints = length(rocdata);
    
    %if N=1, don't bootstrap and just calculate a single roc curve
    if(N==1)
        [sens,onemspec] = roccurve(rocdata,thresholds);
        [bestindex,sensitivity,specificity,auc] = rocstats(sens,onemspec);
        bestthresh = thresholds(bestindex);
    %otherwise N>1, so create N bootstrap interations
    else
    
        %re-seed the random number generator
        rand('state',sum(100*clock))
        home;
        %for each iteration
        for(i = 1:N)
               
            msg = sprintf('roc: Iteration %d of %d',i,N);
            disp(msg);
            drawnow;
            
            
            %get random set of indices
            idx = round(1 + (numDataPoints-1) * rand(1,numDataPoints));
        
            %create a new roc data set using the rand indices
             rocDataN = rocdata(idx,:);
        
            %run ROC analysis for rocDataN
            [sensN(:,i),onemspecN(:,i)] = roccurve(rocDataN,thresholds);    
        end
        
        %get mean and standard deviation of sensitivity
        m = mean(sensN,2);
        s = std(sensN,0,2);
        
        %create output sensitivity matrix which has 2 columns
        %1 = mean, 2 = mean - confmult*std
        sens = [m,m-confmult*s];
        
        %now do the same for onemspec
        m = mean(onemspecN,2);
        s = std(onemspecN,0,2);
        onemspec = [m,m+confmult*s];
        
        %get roc stats for mean
        [bestindex,sensitivity(1),specificity(1),auc(1)] = rocstats(sens(:,1),onemspec(:,1));
        
        %now get stats for lower confidence band, we are ignoring
        %everything execpt for area under the curve
        [~,~,~,auc(2)] = rocstats(sens(:,2),onemspec(:,2));
        
        %now we have the following:
        %1.) mean auc and lower auc confidence limit.
        %2.) the bestindex (threshold) of the mean, with specificity and 
        %    sensitivity at the bestindex
        %we need lower confidence limit.  To get these we plug
        %the index into our upper and lower roc curves.
        sensitivity(2) = sens(bestindex,2);

        specificity(2) = 1-onemspec(bestindex,2);

        
        %get best threshold
        bestthresh = thresholds(bestindex);
        
        
    end
    
    
    
return

function [bestindex,sensitivity,specificity,auc] = rocstats(sens,onemspec)
    %find the best threshold.  We are looking for the point on the roc curve which is
    %closest to (0,1)  So we are going to solve the right triangle for each
    %point.
    hyp = sqrt( (1-sens).^2 + onemspec.^2);
    
    %now find closest point to (0,1)
    bestindex = find(hyp == min(hyp));
    %use first index, this only happens in the rare case where we have more than one min
    bestindex = bestindex(1);
    %use second index, this only happens in the rare case where all the
    %points on the roc cureve are identical
    if bestindex == 1
        bestindex = 2;
    end 
    %get best sensitivity and specificity
    specificity = 1-onemspec(bestindex);
    sensitivity = sens(bestindex);
    
    %now calculate area under the curve using trapz (numerical integration)
    %note: we are taking absolute value of trap z since we are
    %integrating in the reverse direction
    auc = abs(trapz([1;onemspec;0],[1;sens;0]));    
return


function [sens,onemspec] = roccurve(rocdata, thresholds)
    
    %get number of positives and negatives
    nDataPoints = length(rocdata);
    nPos = sum(rocdata(:,1));
    nNeg = nDataPoints - nPos;
    
    nThresh = length(thresholds);
    sens = zeros(nThresh,1);
    onemspec = zeros(nThresh,1);
    for(i=1:nThresh)
        
        %create truepos and trueneg vectors
        truepos = zeros(nDataPoints,1);
        trueneg = truepos;

        thresh = thresholds(i);
        
        %set truepos equal to 1 when score = 1 and index >= threshold
        truepos((rocdata(:,1) == 1) & (rocdata(:,2) >= thresh)) = 1;
        
        %set true neg equal to 1 when score = 0 and index < threshold
        trueneg((rocdata(:,1) == 0) & (rocdata(:,2) < thresh)) = 1;
         
        %sensitivity = number of true pos / number of positives
        sens(i) = sum(truepos)/nPos;
        
        %1 - specificifity = 1 - (number of true neg/number of negatives)
        onemspec(i) = 1-(sum(trueneg)/nNeg);
           
        if isnan(sens(i))
            sens(i) = 0;
        end
        if isnan(onemspec(i))
            onemspec(i) = 0;
        end
    end
    
return
        