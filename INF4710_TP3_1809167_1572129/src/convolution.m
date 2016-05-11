function [ response ] = convolution( image_1ch, kernel )
%Fonction qui retourne la convolution de image_1ch qui est une image avec
%un seul  canal par le noyau kernel.

%Taille de l'image
[rows, cols, channels]=size(image_1ch); 

%Test si l'image possède bien un seul canal
if channels ~=  1
     response = [];
    return 
end

%Test si le noyau est bien de dimension 3x3
if size(kernel,1) ~= 3 || size(kernel,2) ~= 3
    response = [];
    return 
end

%Rotation du noyau
h = rot90(kernel, 2);

%Expansion de l image pour rajouter des 0 sur les bords (voir rapport)
image_modifie = zeros(rows + 2, cols + 2);
for x = 2 : rows + 1
    for y = 2 : cols +1
        image_modifie(x,y) = image_1ch(x - 1, y - 1);
    end
end

%Operation de convolution (voir fin du rapport)
response = zeros(rows , cols);
for x=2:rows-1 
    for y=2:cols-1
        for i = 1 : 3
            for j = 1 : 3
                 response(x, y) = response(x, y) + image_modifie(x+i -1 , y+j- 1) * h(i, j);
            end
        end
    end
end

end

