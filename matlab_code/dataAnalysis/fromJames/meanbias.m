function [y,len] = meanbias(truth,x)
    ind = find(x == 0 | ~isfinite(x) | truth == 0 | ~isfinite(truth));
    x(ind) = [];
    truth(ind) = [];
    
    if(isempty(x))
    y = NaN;
    len = NaN;
    else
    y = mean(x-truth);
    len = length(x);
    end
