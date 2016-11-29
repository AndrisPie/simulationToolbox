% ===============================================================
% LOADING AND CROPPING IMAGES
% ===============================================================
% INPUTS
% timeArray: range of times for images to be cropped
% fileName: location of the files to be cropped
% newFileName: new location to save the cropped files
% cropRec: rectangle dimensions to crop image; [xmin, ymin, height,width]

function imageCropFn(timeArray,fileName,newFileName,cropRec)

    % --------------------------------------------------------------
    % INPUTS (SETTINGS FOR IMAGE CROPPING)
    % --------------------------------------------------------------

    % Rectangle to be cropped
    
    
    xmin = cropRec(1); 
    ymin = cropRec(2);
    height = cropRec(3);
    width = cropRec(4);
    
    % Scale for image resizing
    scale = 1;


    % --------------------------------------------------------------
    % SIMULATION ENGINE
    % --------------------------------------------------------------

    for k = 1:length(timeArray)

        append = timeArray(k);

        % --------------------------------------------------------------
        % CREATE FILE NAME
        % --------------------------------------------------------------

        jpgFileName = strcat(fileName, num2str(append), '.png');
        
        if exist(jpgFileName, 'file')

        % --------------------------------------------------------------
        % READ DATA AND ALTER IMAGE
        % --------------------------------------------------------------

            imageData = imread(jpgFileName);

            cropImage = imcrop(imageData,[xmin ymin width height]);
            resizeImage = imresize(cropImage,scale);

            tifFileName = strcat(newFileName, num2str(append), '.tif');

            imwrite(resizeImage,tifFileName,'Compression','none','Resolution',600);


        % --------------------------------------------------------------
        % IF NAME DOESN'T EXIST, THEN IGNORE
        % --------------------------------------------------------------

        else
            fprintf('File %s does not exist.\n', jpgFileName);
        end


    end

end


