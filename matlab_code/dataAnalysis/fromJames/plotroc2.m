function plotroc2(bestthresh,sensitivity,specificity,auc,sens,onemspec,clr,plotText)

if(nargin < 7)
    clr = 'b';
    plotText = 1;
end

%plotroc: 
%   plots the output of roc().  
%
%Example:
%   [bestthresh,sensitivity,specificity,auc,sen,onemspec] = roc(rocdata,thresholds,N,confmult)
%   plotroc(bestthresh,sensitivity,specificity,auc,sen,onemspec)
%
%General Description:
%   plotroc plots the output of the function roc().  If roc was run with N=1, 
%   1 curve will be plotted. If N was >1, the mean and 
%   lower confidence limit curves will be plotted.  In all cases, bestthreshold, 
%   sensitivity,specificity,and auc statistics will be displayed on the graph.
%
%Input arguments:
%   The output of roc(). type 'help roc' for more info.
%   
%Outpput arguments:
%   None
%
    
   
    if(max(size(bestthresh)) > 1)
        error('bestthresh must be a scalar')
    end
    
    nCols = size(sensitivity,2);
    if(nCols ~= 1 & nCols ~= 2)
        error('sensitivity,specificity,auc,sens,and onemspec must all be Mx1 or Mx2 matries')
    end
    
    if(nCols ~= size(specificity,2) | nCols ~= size(auc,2) | nCols ~= size(sens,2) | nCols ~= size(onemspec,2))
             error('sensitivity,specificity,auc,sens,and onemspec must all be Mx1 or Mx2 matries')
    end
    


    plot([1;onemspec;0],[1;sens;0],clr);
        hold on;
    axis([0,1,0,1]);
    plot(1-specificity(1),sensitivity(1), 'rx','linewidth',10);
    ylabel('True positive rate');
    xlabel('False positive rate');
    hold on;
    x = 0:.1:1;
    plot(x,x,'k--');
    hold off;
    
    if(plotText)
    if(nCols == 1)
    text(.6,.2,sprintf('Best Thresh(X): %f\nSpecificity: %f\nSensitivity: %f\nAUC: %f',bestthresh,specificity,sensitivity,auc));
    else
    text(.5,.2,sprintf('Best Thresh(X): %f\nMean Specificity: %f\nLower Specificity Limit: %f\nMean Sensitivity: %f\nLower Sensitivity Limit: %f\nMean AUC: %f\nLower AUC Limit: %f\n',bestthresh,specificity(1),specificity(2),sensitivity(1),sensitivity(2),auc(1),auc(2)));
    end
    end
    
return