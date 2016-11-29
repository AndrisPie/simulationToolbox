
outputFile  = 'D:\PSSimulationData\PS8mmSteady\output\neput.output';


% Reading output file to get pressure and flowrate values
store = readTextFileFn(outputFile,100);
store2 = readTextFileFn('D:\PSSimulationData\PS3mmSteady\output\PS3mmFinal.output',100);

% --------------------------------------------------------------
% POST-PROCESSING
% --------------------------------------------------------------

% Limiting values to just a few chosen ones
num = 50;
store = discreteStore(store,num);
store2 = discreteStore(store2,num);

% Plotting figure

fig1 = figure(1);

BTR = [290,270];

set(fig1, 'units', 'centimeters', 'pos', [0 0 15 7.5])

% Flowrates
subplot(1,2,1)
plot(store.Ptime/BTR(1),store.PQ1*1e6,'-k',store.Ptime/BTR(1),store.PQ2*1e6,'--k',...
    store2.Ptime/BTR(2),store2.PQ1*1e6,'-r',store2.Ptime/BTR(2),store2.PQ2*1e6,'--r',...
'LineWidth',1.25,'MarkerSize',8);
axis([0.8 1.05 0 4.5])
xlabel('Normalised Time'); ylabel('Flowrate (ml/s)');
title('A');

% Pressures
subplot(1,2,2)
plot(store.Ptime/BTR(1),store.PP1/133.33,'-k',store.Ptime/BTR(1),store.PP2/133.33,'--k',...
    store2.Ptime/BTR(2),store2.PP1/133.33,'-r',store2.Ptime/BTR(2),store2.PP2/133.33,'--r',...
    'LineWidth',1.25,'MarkerSize',8)
axis([0.8 1.05 0 160])
xlabel('Normalised Time'); ylabel('Pressure (mmHg)');
title('B');

set(fig1,'PaperPositionMode','auto')

saveas(fig1,'PS8mmvPS3mm.fig')
saveas(fig1,'PS8mmvPS3mm.png')
