
close all;

%I = im2double(imread('image.png'));
I = im2double(imread('building4.png'));

tic;
%res = RollingGuidanceFilter(I,3,0.05,4);
res = RollingGuidanceFilter(I,40,0.05,10);
toc;

figure,imshow(I);
figure,imshow(res);