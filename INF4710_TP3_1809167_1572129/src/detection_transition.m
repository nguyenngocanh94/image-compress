vrobj = VideoReader('INF4710_TP3_video.avi');

pin = zeros(vrobj.NumberOfFrames);
pout = zeros(vrobj.NumberOfFrames);
% Initialisation des variables de compteur pour coupure et fondu
nbcut = 0;
nbdissolve = 0 ;
isdissolving = false;
N = 5;
% Seuil coupure et fondu
cut_threshold = 0.75;
dissolve_threshold = 0.15;
after_dissolve_threshold = 0.10;

for t=1:vrobj.NumberOfFrames
    image = read(vrobj,t);
    % Calcul des forces du gradient
    gradient = fg_norm_sobel(image);
    % Seuillage du gradient
    image_binaire_t1 = seuillage(gradient);
    % Dilatation de l'image binaire
    dilated_image_t1 = dilate_image(image_binaire_t1,N);
    
    % Calcul des ratio pin et pout
    if t>1
        pin(t) = edge_ratio(dilated_image_t0,image_binaire_t1);
        pout(t) = edge_ratio(dilated_image_t1,image_binaire_t0);
    end
    
    dilated_image_t0 = dilated_image_t1;  % trame t - 1
    image_binaire_t0 = image_binaire_t1;  % trame t - 1
    
    % Detection de coupure
    if max(pin(t),pout(t)) > cut_threshold
        nbcut= nbcut +1;
        cut(nbcut) = t;
    end
    
    % Detection de fondus
     if max(pin(t),pout(t)) > dissolve_threshold && max(pin(t),pout(t)) < cut_threshold && ~isdissolving
        nbdissolve= nbdissolve +1;
        dissolve(nbdissolve) = t;
        isdissolving = true;
     % Evite de redetecter le meme fondu
     elseif  max(pin(t),pout(t)) < after_dissolve_threshold && isdissolving
        isdissolving = false;
     end
    
    imshow(image);
    pause(0.01);
end


