% ===============================================================
% PLOTTING IMAGES
% ===============================================================
% INPUTS
% timeArray: 6 times for plotting figure
% fileName: location of the files
% fileSave: save directory for the final figure 
% fileLegend: directory for the legend
% param: 3 element array, up/down; right/left/size

function plotImagesFn(timeArray,fileName, fileSave, fileLegend,params)

% --------------------------------------------------------------
% INPUTS (SETTINGS FOR IMAGE CROPPING)
% --------------------------------------------------------------


% Plot variables
nRows = 2;
nCols = 4;

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

list = [2,3,4,6,7,8];

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
% LEGEND PLOT
% --------------------------------------------------------------

% If there is a legend file
if exist(fileLegend, 'file')

    legendData = imread(fileLegend);

    h = subplot(nRows,nCols,[1 5]);
    
    % Size legend image accordingly
    rightLeft   = params(1);
    upDown      = params(2);
    size        = params(3);
    
    ax = get(h,'position'); %Get current position
    ax(1) = ax(1)*rightLeft; % Move to the right/left
    ax(2) = ax(2)*upDown; % Move up/down
    ax(4) = ax(4)*size; % Shrink size
    
    set(h,'Position',ax)
    imshow(legendData);


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




