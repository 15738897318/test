function [out] = filt_img(im)

f = fspecial( 'gaussian', [7 1], 2.5);

[h w] = size(im);

out = im;

for j = 1 : w
    out(:,j) = conv(im(:,j),f,'same');
end

for i = 1 : h
    out(i,:) = conv(im(i,:),f,'same');
end