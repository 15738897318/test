function p=precision(truth,test)

ind = find(truth == 0 | test == 0 | ~isfinite(truth) | ~isfinite(test));
truth(ind) = [];
test(ind) = [];
if(isempty(truth) || isempty(test) || length(truth) < 3 || length(test) < 3)
    p = nan;
else
  [m,c] = leastsqr_bestfitline(truth,test);
  
  
  
  
  fit = m*truth + c;
  
  p = sum( (test - fit).^2);
  p = p / (length(fit)-2);
  p = sqrt(p);
  

end
