% ===============================================================
% PLOTTING IMAGES (without legend)
% ===============================================================
% INPUTS
% timeArray: 6 times for plotting figure
% fileName: location of the files
% fileSave: save directory for the final figure 

function plotImagesNoLegendFn(timeArray,fileName, fileSave)

% --------------------------------------------------------------
% INPUTS (SETTINGS FOR IMAGE CROPPING)
% --------------------------------------------------------------


% Plot variables
nRows = 2;
nCols = 3;

% Controlling size of figure

figHandle = figure(1);
width = 20;
height = 10;
set(figHandle, 'units', 'centimeters', 'pos', [0 0 width height])

% Title labels
titleLabel =['A','B','C','D','E','F'];

% --------------------------------------------------------------
% SIMULATION ENGINE
% --------------------------------------------------------------

list = [1,2,3,4,5,6];

for k = 1:length(timeArray)
    
    append = timeArray(k);
    
    % --------------------------------------------------------------
    % CREATE FILE NAME
    % --------------------------------------------------------------
    
    tiffFileName = strcat(fileName, num2str(append), '.tif');
    
    if exist(tiffFileName, 'file')
        
        
       imageData = imread(tiffFileName);
        
       subplot(nRows,nCols,list(k));
       imshow(imageData);
       title(num2str(titleLabel(k)),'fontweight','bold','fontsize',12);
        
    % --------------------------------------------------------------
    % IF NAME DOESN'T EXIST, THEN IGNORE
    % --------------------------------------------------------------
    
	else
		fprintf('File %s does not exist.\n', tiffFileName);
	end
    
    
end

% --------------------------------------------------------------
% SAVE PLOT
% --------------------------------------------------------------

fileSaveM = strcat(fileSave,  'M.fig');
fileSaveT = strcat(fileSave,  'T.tif');

saveas(figHandle,fileSaveM);
set(figHandle,'PaperPositionMode','auto')
print(figHandle,'-dtiff',fileSaveT,'-r600');

end




