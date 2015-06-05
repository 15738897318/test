function [m,c]=leastsqr_bestfitline(x,y)


if(all(x == x(1)))
    x(1) = x(2)*1.000000001;
end



xbar=mean(x);
ybar=mean(y);


m=sum((x.*y)-(xbar.*ybar))/sum(x.^2-xbar.^2);
c=ybar-m*xbar;

