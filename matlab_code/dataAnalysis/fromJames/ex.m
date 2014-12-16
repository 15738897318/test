function [r] = ex(truth,test,d)

ind = find(truth <= 0 | test <= 0 | ~isfinite(truth) | ~isfinite(test));
truth(ind) = [];
test(ind) = [];
if(isempty(truth) | isempty(test))
    r = NaN;
else

ind = find(abs(truth - test) > d);
r = length(ind)./length(test);
    
end