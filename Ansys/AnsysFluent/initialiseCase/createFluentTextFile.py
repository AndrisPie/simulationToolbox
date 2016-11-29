# -*- coding: utf-8 -*-
"""
Created on Tue Nov 29 13:30:21 2016

@author: ap4409
"""

# ------------------------------------------------------------------------------
# INPUTS
# ------------------------------------------------------------------------------

# Mesh Details
mesh_name = 'fluentNew.msh';
inlet_name = 'inlet';
outlet_names = ['outlet_mca','outlet_aca'];

# Material Properties
density = 1060;
viscosity = 3.5E-3;

# User Defined Properties
library_name = 'libudf';
C_fileNames = ['flowAt0p1007.c','windkesselWithAvgV2.c'];
UDF_inlet = 'flowAt0p1007.c';
UDF_outlet = 'pressure.c'

# User Defined Scalars
NUM_SCALAR = 8;
scalar_options = ['"mass flow rate" "default"','"none" "default"','"mass flow rate" "default"','"none" "default"','"mass flow rate" "default"','"none" "default"','"none" "default"','"none" "default"']

# Boundary Conditions
concArray = [0.04,0,2.2,0,0,0,0,0]


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

# User defined scalars
line = 'define/user-defined/user-defined-scalars ' + str(NUM_SCALAR) + ' yes no ';

for scalar in scalar_options:
    
    line += ' yes ' + scalar;

text.append(line)

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
    
# Assigning Reactions to Zones
    
# Solution Methods 
    
# Solution Controls
    
# Residuals
    
# Solution Initialisation
    
# Calculation Activities
    
# Run calculation


# ------------------------------------------------------------------------------
# WRITING FILE
# ------------------------------------------------------------------------------

text_file = open("Output.txt", "w");

for line in text:
    text_file.write(line+'\n');
    
    
text_file.close()