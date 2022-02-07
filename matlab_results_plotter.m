clear
clc

load('X.mat', '-ascii')
load('Y.mat', '-ascii')
load('Z.mat', '-ascii')
load('V.mat', '-ascii')

%cleaning up 0.0 values, which give problems when applying log10
% we also find out the max value
for row=1:size(V,1)
    for col=1:size(V,2)
        if V(row, col) == 0.0
            V(row, col) = 1.0e-100;
        end
    end
end
max_value = max(max(V));
max_value = 5e-4;

logV = log10(V);

%FIGURE 1: actual values
figure
clf 
% S1 = surfX,Y,Z,V,'FaceAlpha',0.9,'FaceColor','interp','FaceLighting','gouraud');
% lighting phong
S0 = mesh(X,Y,Z,V,'EdgeColor',[0.2 0.2 0.2],'FaceAlpha',0.4); hold on;
S0.FaceColor = 'interp';
shading interp
% ,'FaceColor','interp','FaceLighting','gouraud'
% S1.EdgeColor = 'none';
% rotate3d on ;
title('Electric Field') ;         
%axis off ; 
hidden on;
colormap(jet);
% cc1=colorbar;
caxis([0 max_value]);
view(-71,14);
cbar = colorbar;
% brighten(0.5); 
% alpha(0.9)
% material('dull'); %shiny, dull, metal

% Title of the colorbar
set(get(cbar,'title'),'string','Electric Field');

% Setting the values on colorbar
% get the color limits
clim = caxis;
ylim(cbar,[clim(1) clim(2)]);
numpts = 24 ;    % Number of points to be displayed on colorbar
kssv = linspace(clim(1),clim(2),numpts);
set(cbar,'YtickMode','manual','YTick',kssv); % Set the tickmode to manual
for i = 1:numpts
    imep = num2str(kssv(i),'%+3.2E');
    vasu(i) = {imep} ;
end
set(cbar,'YTickLabel',vasu(1:numpts),'fontsize',9);
