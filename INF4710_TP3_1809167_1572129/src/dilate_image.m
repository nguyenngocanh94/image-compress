function [ dilated_image ] = dilate_image( image, N )    
%Fonction qui va dilater les pixels blanc d'un gradient
% après seuillage sur un voisinage de NxN 

dilated_image = zeros(size(image));
for i=1:size(image,1)
    for j=1:size(image,2)
        if image(i,j)==1
            n=floor(N/2);
            % Calcul des frontieres
            top = max(j-n,1);
            down = min(j+n,size(image,2));
            left = max(i-n,1);
            right = min(i+n,size(image,1));
            % Dilatation sur le voisinage
            for x=left:right
                for y=top:down
                    dilated_image(x,y)=1.0;
                end
            end
        end
            
    end


end



end

