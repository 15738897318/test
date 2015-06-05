function [r,len] = rmsd(x1,x2)
x1 = x1(:);
x2 = x2(:);
ind = find(x1 == 0 | x2 == 0 | ~isfinite(x1) | ~isfinite(x2));
x1(ind) = [];
x2(ind) = [];
if(isempty(x1) | isempty(x2))
    r = NaN;
    len = 0;
else
%rmsd
len = length(x1);
sumsq = sum((x1-x2).^2);
r = sqrt(sumsq/len);
%mae
%r = sum(abs(x1-x2))/length(x1);
end