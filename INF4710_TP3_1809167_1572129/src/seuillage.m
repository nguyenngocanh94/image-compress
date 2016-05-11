function [ image_binaire ] = seuillage ( gradient )
% Fonction qui applique le seuillage sur un gradient et qui renvoie une
% image binaire

% Seuils 
seuil_RGB = 0.3;
seuil_gray = 0.25;

%Initialisation de l'image binaire
image_binaire = zeros(size(gradient,1));

    for i=1:size(gradient,1)
        for j=1:size(gradient,2)
                % Si image RGB
                if size(gradient,3)==3
                    if gradient(i,j,1)> seuil_RGB || gradient(i,j,2)> seuil_RGB || gradient(i,j,3)> seuil_RGB
                        image_binaire(i,j)=1;
                    else
                        image_binaire(i,j)=0;
                    end
                % Si image noir et blanc    
                else
                    if gradient(i,j)> seuil_gray
                        image_binaire(i,j)=1;
                    else
                        image_binaire(i,j)=0;
                    end
                    
                end
              
        end
    end
    

end

