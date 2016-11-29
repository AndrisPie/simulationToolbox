% ===============================================================
% CROP AND PLOT IMAGES RUN ALL FILE
% ===============================================================
% File that crops images from directory, saves to another directory and
% then uses the cropped images for 6 subplot figures.

% --------------------------------------------------------------
% INPUTS 
% --------------------------------------------------------------
% File Directories
fileName    = 'D:\PSSimulationData\PS8mmSteady\rawImages\'; % Directory of precropped images
newFileName = 'D:\PSSimulationData\PS8mmSteady\cleanImages\'; % Directory of cropped images
fileSave    = 'D:\PSSimulationData\PS8mmSteady\cleanImages\'; % Directory to save final figures
fileLegend  = 'C:\Simulations\Images\3DPapReal\legends\altlegendIsovolume.tif'; % Directory of the legend

% Extensions
extensionList = {'clotIsovolume_','streamlines3D_'}; % Name of the plto

% Cropping parameters for each image
cropRecList = [0,80,1000,1000;0,80,1500,1500]; % Cropping parameters for each figure
cropTimeArray = 200:1:300; % Array of times to be cropped

% Plotting parameters
timeArray = [235,245,255,265,275,285]; % range of times for images to be cropped

% --------------------------------------------------------------
% SIMULATION ENGINE 
% --------------------------------------------------------------

for it = 1: length(extensionList)

    % Extension
    extension = char(extensionList(it));
    
    % Cropping images
    cropRec = cropRecList(it,:);
    imageCropFn(cropTimeArray,strcat(fileName,extension),strcat(newFileName,extension),cropRec)

    % Plotting images
    
    if it == 1
    
        plotImagesNoLegendFn(timeArray,strcat(newFileName,extension), strcat(fileSave,extension))
    
    else
    
        params = [1.5,1.6,0.8]; % For positioning the legend [Right/Left,Up/Down; Size]
        plotImagesFn(timeArray,strcat(newFileName,extension), strcat(fileSave,extension), fileLegend,params)
        
    end

end





