function [ p ] = edge_ratio ( A, B )  
%Fonction qui calcule le ratio de la matrice A et B comme tel
% p = 1- (A*B)/B
num = 0;
denom =0;
for i=1:size(A,1)
    for j=1:size(A,2)
       num = num + A(i,j)*B(i,j);
       denom = denom + B(i,j);
    end
end

 p = 1 - num/denom;

end

