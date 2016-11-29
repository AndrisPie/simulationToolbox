% ======================================================================
% CODE SEARCHES TRHOUGH FLUENT OUTPUT FILE, RETRIEVES FLOW, PRESSURE AND
% CONVERGENCE DATA
% ======================================================================

% INPUTS: text file directory, max number of iterations per timestep

% OUTPUTS: Pressure, flowrate, final convergence points


function store = readTextFileFn(dir,N_ITER)

% -----------------------------------------------------------------------
% SIMULATION SET-UP
% -----------------------------------------------------------------------

% Open file for reading
fileID = fopen(dir,'r');

% Initial values for store matrix
store.Q1 = 0; store.Q2 = 0;
store.P1 = 0; store.P2 = 0;
store.converge = 0;
store.time = 0;

% Auxillary variables used during calculation
it = 0;         % Store index
line  = 0;      % Line number
nextLine = -1;  % Flag for simulation

% -----------------------------------------------------------------------
% SIMULATION ENGINE
% -----------------------------------------------------------------------

% Start while loop
    while 1
        
    % Read file line by line
    tline = fgetl(fileID);
    
    % Update line number
    line = line + 1;
    
    % If line has characters
    if ischar(tline)
        
        % Keywords to examine
        convergeLine = strfind(tline, 'solution is converged');
        WK1Line      = strfind(tline, 'Windkessel 1 :');
        WK2Line      = strfind(tline, 'Windkessel 2 :');
        timeLine     = strfind(tline, 'Flow time ');
        
        % ---------------------------------------------------------------
        % Find Windkessel components
        % ---------------------------------------------------------------
                
        if isfinite(WK1Line) == 1;
            
            % Read Values of Q and P for Windkessel 1 
            Q_1 = str2double(tline(20:31));
            P_1 = str2double(tline(44:56));
            
            % Update store index
            it = it + 1;
            
            % Store values of Q_1 and P_1
            store.Q1(it) = Q_1;
            store.P1(it) = P_1;             
        end
        
        
        if isfinite(WK2Line) == 1;
            
            % Read Values of Q and P for Windkessel 2 
            Q_2 = str2double(tline(20:31));
            P_2 = str2double(tline(44:56));
            
            % Store values of Q_1 and P_1
            store.Q2(it) = Q_2;
            store.P2(it) = P_2;
        end
        
        % ---------------------------------------------------------------
        % Find time information
        % ---------------------------------------------------------------
        
        if isfinite(timeLine) == 1;
            
            % Read and store value for flowtime
            time = sscanf(tline,'Flow time = %fs');
            store.time(it) = time;
            
            % If solution hasn't converged, store data here
            if nextLine ==-1
               store.converge(it) = 0;
            end
            
            % Reset convergence flag
            nextLine = -1;
        end     
        
        % ---------------------------------------------------------------
        % Find convergence information
        % ---------------------------------------------------------------
        
        % If convergence information has been found, read next line
        if isfinite(convergeLine) == 1;
        
           nextLine = line + 1;
            
        end

        % Reading next line for convergence information
        if line == nextLine
           
            % Use information only if its at the end of the timnestep
            if str2double(tline(end-3:end)) ~= N_ITER
                store.converge(it+1) = str2double(tline(end-3:end));
            end
                
                
        end
        
    end
    % End of if character loop
    
    % Break while loop if line number exceeds an upper limit 
    if line > 5E6;
        break
    end
    
    end
    % End while loop
  
% -----------------------------------------------------------------------
% CLEAN-UP
% -----------------------------------------------------------------------    

fclose(fileID);



end 


