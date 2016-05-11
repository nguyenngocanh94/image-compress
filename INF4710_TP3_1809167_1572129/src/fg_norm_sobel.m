function [ gradient ] = fg_norm_sobel( image )
%Fonction qui calcule les forces de gradients d'une image avec le noyau
% de sobel

% Dimensions de l image
[rows, cols ,channels] =size(image);
% Noyaux de sobels
kernelx = [-1 0 1; -2 0 2; -1 0 1 ];
kernely = [1 2 1; 0 0 0; -1 -2 -1];


gradient = zeros(rows,cols,channels);
for l=1:size(image,3)
    % Convolution avec les noyaux de sobels
    image_1ch = image(:,:,l);
    Gx = convolution(image_1ch,kernelx);
    Gy = convolution(image_1ch,kernely);
    % Calcul des forces du gradient
    for i=1:rows
        for j=1:cols
            gradient(i,j,l) = sqrt(Gx(i,j)^2+Gy(i,j)^2);
        end
    end
    m1 = min(min(gradient(:,:,l)));
    m2 = max(max(gradient(:,:,l)));
    % Normalisartion avec la methode min-max
    gradient(:,:,l) = (gradient(:,:,l) - m1) / (m2 - m1);
        
end




end


