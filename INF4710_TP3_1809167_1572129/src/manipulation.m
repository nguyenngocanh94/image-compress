%Fichier de test de manipulation
image = imread('RGB.jpg');
imshow(image);
pause(1);
gradient = fg_norm_sobel(image);
imshow(gradient);
pause(1);
image_binaire = seuillage(gradient);
imshow(image_binaire);
pause(1);
dilated_image = dilate_image(image_binaire,5);
imshow(dilated_image);
pause(1);