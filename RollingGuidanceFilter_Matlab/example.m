
close all;

%I = im2double(imread('image.png'));
I = im2double(imread('1_2.png'));

tic;
%res = RollingGuidanceFilter(I,3,0.05,4);
res = RollingGuidanceFilter(I,4,0.05,8);
toc;

figure,imshow(I);
figure,imshow(res);