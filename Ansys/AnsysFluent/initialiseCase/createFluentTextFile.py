# -*- coding: utf-8 -*-
"""
Created on Tue Nov 29 13:30:21 2016

@author: ap4409

Issues to fix:
1) No knowledge of the zone ids beforehand
2) Windkessel scalar component needs to be translated to another file
3) Print important information (e.g max. and avg. WSS)

"""

# ------------------------------------------------------------------------------
# INPUTS
# ------------------------------------------------------------------------------

# Scalars/Clot Needed?
scalarNeed = 0;
clotNeed = 1;

# Mesh Details
mesh_name = 'the_mesh_file.msh';
inlet_name = 'inlet';
outlet_names = ['outlet_mca','outlet_aca'];
clot_names = ['clot1','clot2','clot3','clot4']

# Material Properties
density = 1060;
viscosity = 3.5E-3;
diff = [5.3E-8,0,5.3E-8,0,5.3E-8,0,0,0]

# User Defined Function Information
library_name = 'libudf';
C_fileNames = ['flowAt0p1007.c','windkesselWithAvgV2.c','Initialise_clot_domain_Nov_2016.c','lysisKineticsSlowx1.c','Mom_source_term_Feb_2016_change.c'];
UDF_inlet = 'flowAt0p1007.c';
UDF_outlet = 'pressure.c'
source_array = ['Darcian_x','Darcian_y','Darcian_z','free_tPA','bound_tPA','free_PLG','bound_PLG','free_PLS','bound_PLS','tot_BS','L_PLS_woof']
initial_UDF = 'initialise_clot_domain'
execEnd_UDF = 'execute'
ondemandUDF = 'variable_initialization'

# User Defined Scalars
NUM_SCALAR = 8;
scalar_options = ['"mass flow rate" "default"','"none" "default"','"mass flow rate" "default"','"none" "default"','"mass flow rate" "default"','"none" "default"','"none" "default"','"none" "default"']

# Boundary Conditions
concArray = [0.04,0,2.2,0,0,0,0,0]

# Saving
startName = 'start.cas'
save_interval = 1000;
dir_name = 'dataFile'

# Running 
N_dt = 2;
max_ts = 100; 

# ------------------------------------------------------------------------------
# SIMULATION ENGINE
# ------------------------------------------------------------------------------

text = [];

# Creating mesh
text.append('file/read ' + mesh_name)
text.append('mesh/scale 0.001 0.001 0.001');

# Model Set Up
text.append('define/models/unsteady-2nd-order yes');
text.append('define/materials/change-create air blood yes constant ' + str(density) 
+ ' no no yes constant ' +str(viscosity) +' no no no yes')

# User defined functions
text.append('define/user-defined/compiled-functions/compile ' + library_name + ' yes ' + ' '.join(C_fileNames))
text.append('')
text.append('')
text.append('define/user-defined/compiled-functions/load ' + library_name);

# Hooking the UDFs
if clotNeed == 1:
    text.append('define/user-defined/function-hooks/initialization "' + initial_UDF + '::' + library_name +'"')
    text.append('')
    
text.append('define/user-defined/function-hooks/execute-at-end "' + execEnd_UDF + '::' + library_name +'"')
text.append('')

# User defined scalars
line = 'define/user-defined/user-defined-scalars ' + str(NUM_SCALAR) + ' yes no ';

for scalar in scalar_options:
    
    line += ' yes ' + scalar;

text.append(line)

# Scalar Diffusion
line = 'define/materials/change-create blood blood no no no no no no yes defined-per-uds '

for it in range(NUM_SCALAR):

    line += str(it) + ' constant ' + str(diff[it]) + ' '; 

line += '-1 no'    

text.append(line)

# User defined memory
text.append('define/user-defined/user-defined-memory 20')
text.append('define/user-defined/user-defined-node-memory 20')

# Boundary Conditions

# Inlet
line = 'define/boundary-conditions/velocity-inlet ' + inlet_name + ' no no yes yes yes yes "udf" "' + UDF_inlet[0:len(UDF_inlet)-2] + '::' + library_name+ '" no 0 no '
for conc in concArray:
    
    line += 'yes no '
    
line = line[0:len(line)-3]
for conc in concArray:
    
    line += 'no ' + str(conc) + ' ';

text.append(line);

# Outlets
for outlet in outlet_names:
    
    text.append('define/boundary-conditions/pressure-outlet ' + outlet + ' yes yes "udf" "' 
    + UDF_outlet [0:len(UDF_outlet)-2] + '::' + library_name+ '" no yes ' 
    + 'yes '*len(concArray) + 'no 0 '*len(concArray) + 'no no no')
    
# Cell Zone Conditions
line = 'define/boundary-conditions/fluid ' + clot_names[0] + ' no yes 0'

for source_UDF in source_array:
    
    line += ' 1 no yes "' + source_UDF + '::' + library_name + '"';


line += 'no no no 0 no 0 no 0 no 0 no 0 no 1 no no no';

text.append(line)

if len(clot_names) > 1:
    
    line = 'define/boundary-conditions/copy-bc';
    
    for clot in clot_names:
        
        line += ' ' + clot;
        
    text.append(line);
    text.append('');
    
# Solution Methods/ Solution Controls (URFs) 
text.append('solve/set/discretization-scheme/pressure 14') # PRESTO Scheme

for num in range(NUM_SCALAR):
    
    text.append('solve/set/discretization-scheme/uds-'+ str(num) + ' 1') # 2nd order Scheme
    text.append('solve/set/under-relaxation/uds-'+ str(num) + ' 0.7') # Under relaxation factor
    
# Residuals
text.append('solve/monitors/residual/convergence-criteria ' + '1E-5 '*(NUM_SCALAR + 4))
    
# Solution Initialisation
text.append('define/user-defined/execute-on-demand "' + ondemandUDF + '::' + library_name + '"')
text.append('solve/initialize/set-defaults/uds-2 ' +str(concArray[2]));
text.append('/solve/initialize/initialize-flow ok');
    
# Calculation Activities
text.append('file/auto-save/data-frequency ' + str(save_interval));
text.append('file/auto-save/append-file-name-with flow-time 6');
text.append('file/auto-save/root-name ' + dir_name);

# Checking if scalars are required
if scalarNeed == 0:
    for it in range(NUM_SCALAR):
        text.append('solve/set/equations/uds-'+ str(it) + ' no')

# Saving Case and File
text.append('file/write-case-data ' + startName);
    
# Run calculation
#text.append('solve/dual-time-iterate ' + str(N_dt) + ' ' + str(max_ts));

# ------------------------------------------------------------------------------
# WRITING FILE
# ------------------------------------------------------------------------------

text_file = open("Output.txt", "w");

for line in text:
    text_file.write(line+'\n');
    
    
text_file.close()