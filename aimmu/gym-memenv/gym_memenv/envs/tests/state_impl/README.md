------------
Description:
------------
The module is developed to model a hardware module that can take input from 
PIM trace buffer and record the accesses as long as the relation among the 
accesses. After an epoch (fixed/decided by Neural predictor) those information
in the table is being used to build the page relation graph where the page 
entries are the nodes and the number of accesses along with hop distances 
are used to calculate the weight on the edges. 

Then the graph is fed into node2vec and the represntation of the gaph is being
generated. This is used as a pre-processing layer for the neural network as
a representation learning layer.  

----------------------
run command (example):
----------------------
python fextract.py --disp-raw True --num-ops 1000 --output output/random.out
