

mkdir -p stats/spmv_baseline
python sim_no_agent.py --aimmu-config setup/new_spmv_baseline.cfg | tee stats/spmv_baseline/spmv_baseline.log & 
mkdir -p stats/pagerank_baseline
python sim_no_agent.py --aimmu-config setup/new_pagerank_baseline.cfg | tee stats/pagerank_baseline/pagerank_baseline.log & 
mkdir -p stats/backprop_baseline
python sim_no_agent.py --aimmu-config setup/new_backprop_baseline.cfg  | tee stats/backprop_baseline/backprop_baseline.log &
mkdir -p stats/rbm_baseline
python sim_no_agent.py --aimmu-config setup/new_rbm_baseline.cfg | tee stats/rbm_baseline/rbm_baseline.log & 
mkdir -p stats/lud_baseline
python sim_no_agent.py --aimmu-config setup/new_lud_baseline.cfg | tee stats/lud_baseline/lud_baseline.log  &
# mkdir -p stats/mac_baseline
# python sim_no_agent.py --aimmu-config setup/new_mac_baseline.cfg | tee stats/mac_baseline/mac_baseline.log &
# mkdir -p stats/reduce_baseline
# python sim_no_agent.py --aimmu-config setup/new_reduce_baseline.cfg | tee stats/reduce_baseline/reduce_baseline.log &
# mkdir -p stats/kmeans_baseline
# python sim_no_agent.py --aimmu-config setup/new_kmeans_baseline.cfg | tee stats/kmeans_baseline/kmeans_baseline.log & 
mkdir -p stats/sgemm_baseline
python sim_no_agent.py --aimmu-config setup/new_sgemm_baseline.cfg | tee stats/sgemm_baseline/sgemm_baseline.log &
# mkdir -p stats/svm_baseline
# python sim_no_agent.py --aimmu-config setup/new_svm_baseline.cfg | tee stats/svm_baseline/svm_baseline.log &
# mkdir -p stats/streamcluster_baseline
# python sim_no_agent.py --aimmu-config setup/new_streamcluster_baseline.cfg | tee stats/streamcluster_baseline/streamcluster_baseline.log & 

#============================================================================================#
#||||||||||||||||||||||||||||||||| BNMP  ||||||||||||||||||||||||||||||||||||||||||||||||||||#
#============================================================================================#

#mkdir stats/spmv_baseline
#python simulate.py --aimmu-config setup/spmv_baseline.cfg | tee stats/spmv_baseline/spmv_baseline.log & 
#mkdir stats/pagerank_baseline
#python simulate.py --aimmu-config setup/pagerank_baseline.cfg | tee stats/pagerank_baseline/pagerank_baseline.log & 
#mkdir stats/backprop_baseline
#python simulate.py --aimmu-config setup/backprop_baseline.cfg  | tee stats/backprop_baseline/backprop_baseline.log &
#mkdir stats/rbm_baseline
#python simulate.py --aimmu-config setup/rbm_baseline.cfg | tee stats/rbm_baseline/rbm_baseline.log & 
#mkdir stats/lud_baseline
#python simulate.py --aimmu-config setup/lud_baseline.cfg | tee stats/lud_baseline/lud_baseline.log  &
#mkdir stats/mac_baseline
#python simulate.py --aimmu-config setup/mac_baseline.cfg | tee stats/mac_baseline/mac_baseline.log &
#mkdir stats/reduce_baseline
#python simulate.py --aimmu-config setup/reduce_baseline.cfg | tee stats/reduce_baseline/reduce_baseline.log &
#mkdir stats/kmeans_baseline
#python simulate.py --aimmu-config setup/kmeans_baseline.cfg | tee stats/kmeans_baseline/kmeans_baseline.log & 
##mkdir stats/sgemm_baseline
##python simulate.py --aimmu-config setup/sgemm_baseline.cfg | tee stats/sgemm_baseline/sgemm_baseline.log &
#mkdir stats/svm_baseline
#python simulate.py --aimmu-config setup/svm_baseline.cfg | tee stats/svm_baseline/svm_baseline.log &
#mkdir stats/streamcluster_baseline
#python simulate.py --aimmu-config setup/streamcluster_baseline.cfg | tee stats/streamcluster_baseline/streamcluster_baseline.log & 
#
#============================================================================================#
#||||||||||||||||||||||||||||||||| BNMP+TOM |||||||||||||||||||||||||||||||||||||||||||||||||#
#============================================================================================#

#mkdir stats/spmv_baseline_tom
#python simulate.py --aimmu-config setup/spmv_baseline_tom.cfg | tee stats/spmv_baseline_tom/spmv_baseline_tom.log &  
#mkdir stats/pagerank_baseline_tom
#python simulate.py --aimmu-config setup/pagerank_baseline_tom.cfg | tee stats/pagerank_baseline_tom/pagerank_baseline_tom.log &
#mkdir stats/backprop_baseline_tom
#python simulate.py --aimmu-config setup/backprop_baseline_tom.cfg  | tee stats/backprop_baseline_tom/backprop_baseline_tom.log 
#mkdir stats/rbm_baseline_tom
#python simulate.py --aimmu-config setup/rbm_baseline_tom.cfg | tee stats/rbm_baseline_tom/rbm_baseline_tom.log & 
#mkdir stats/lud_baseline_tom
#python simulate.py --aimmu-config setup/lud_baseline_tom.cfg | tee stats/lud_baseline_tom/lud_baseline_tom.log  &
#mkdir stats/mac_baseline_tom
#python simulate.py --aimmu-config setup/mac_baseline_tom.cfg | tee stats/mac_baseline_tom/mac_baseline_tom.log &
#mkdir stats/reduce_baseline_tom
#python simulate.py --aimmu-config setup/reduce_baseline_tom.cfg | tee stats/reduce_baseline_tom/reduce_baseline_tom.log &
#mkdir stats/kmeans_baseline_tom
#python simulate.py --aimmu-config setup/kmeans_baseline_tom.cfg | tee stats/kmeans_baseline_tom/kmeans_baseline_tom.log &
#mkdir stats/sgemm_baseline_tom
#python simulate.py --aimmu-config setup/sgemm_baseline_tom.cfg | tee stats/sgemm_baseline_tom/sgemm_baseline_tom.log &
#mkdir stats/svm_baseline_tom
#python simulate.py --aimmu-config setup/svm_baseline_tom.cfg | tee stats/svm_baseline_tom/svm_baseline_tom.log &
#mkdir stats/streamcluster_baseline_tom
#python simulate.py --aimmu-config setup/streamcluster_baseline_tom.cfg | tee stats/streamcluster_baseline_tom/streamcluster_baseline_tom.log &

#============================================================================================#
#||||||||||||||||||||||||||||||||| BNMP+AIMM ||||||||||||||||||||||||||||||||||||||||||||||||#
#============================================================================================#

#mkdir stats/kmeans_learning
#python simulate.py --aimmu-config setup/kmeans_learning.cfg --replay-buff-sz 1000 | tee stats/kmeans_learning/kmeans_learning.log & 
##mkdir stats/sgemm_learning
##python simulate.py --aimmu-config setup/sgemm_learning.cfg --replay-buff-sz 1000 | tee stats/sgemm_learning/sgemm_learning.log &
#mkdir stats/svm_learning
#python simulate.py --aimmu-config setup/svm_learning.cfg --replay-buff-sz 1000 | tee stats/svm_learning/svm_learning.log &
#mkdir stats/streamcluster_learning
#python simulate.py --aimmu-config setup/streamcluster_learning.cfg --replay-buff-sz 1000 | tee stats/streamcluster_learning/streamcluster_learning.log &
#mkdir stats/lud_learning
#python simulate.py --aimmu-config setup/lud_learning.cfg --replay-buff-sz 1000 | tee stats/lud_learning/lud_learning.log & 
#mkdir stats/rbm_learning
#python simulate.py --aimmu-config setup/rbm_learning.cfg --replay-buff-sz 1000 | tee stats/rbm_learning/rbm_learning.log &
#mkdir stats/spmv_learning
#python simulate.py --aimmu-config setup/spmv_learning.cfg --replay-buff-sz 1000 | tee stats/spmv_learning/spmv_learning.log & 
#mkdir stats/mac_learning
#python simulate.py --aimmu-config setup/mac_learning.cfg --replay-buff-sz 1000 | tee stats/mac_learning/mac_learning.log &
#mkdir stats/reduce_learning
#python simulate.py --aimmu-config setup/reduce_learning.cfg --replay-buff-sz 1000 | tee stats/reduce_learning/reduce_learning.log &
#mkdir stats/pagerank_learning
#python simulate.py --aimmu-config setup/pagerank_learning.cfg --replay-buff-sz 1000 | tee stats/pagerank_learning/pagerank_learning.log &
#mkdir stats/backprop_learning
#python simulate.py --aimmu-config setup/backprop_learning.cfg --replay-buff-sz 1000 | tee stats/backprop_learning/backprop_learning.log &




#============================================================================================#
#||||||||||||||||||||||||||||||||| LDB  |||||||||||||||||||||||||||||||||||||||||||||||||||||#
#============================================================================================#

#mkdir stats/backprop_baseline_bal
#python simulate.py --aimmu-config setup/backprop_baseline_bal.cfg  | tee stats/backprop_baseline_bal/backprop_baseline_bal.log &
#mkdir stats/spmv_baseline_bal
#python simulate.py --aimmu-config setup/spmv_baseline_bal.cfg | tee stats/spmv_baseline_bal/spmv_baseline_bal.log & 
#mkdir stats/pagerank_baseline_bal
#python simulate.py --aimmu-config setup/pagerank_baseline_bal.cfg | tee stats/pagerank_baseline_bal/pagerank_baseline_bal.log & 
#mkdir stats/rbm_baseline_bal
#python simulate.py --aimmu-config setup/rbm_baseline_bal.cfg | tee stats/rbm_baseline_bal/rbm_baseline_bal.log & 
#mkdir stats/lud_baseline_bal
#python simulate.py --aimmu-config setup/lud_baseline_bal.cfg | tee stats/lud_baseline_bal/lud_baseline_bal.log  &
#mkdir stats/mac_baseline_bal
#python simulate.py --aimmu-config setup/mac_baseline_bal.cfg | tee stats/mac_baseline_bal/mac_baseline_bal.log & 
#mkdir stats/reduce_baseline_bal
#python simulate.py --aimmu-config setup/reduce_baseline_bal.cfg | tee stats/reduce_baseline_bal/reduce_baseline_bal.log  
#mkdir stats/kmeans_baseline_bal
#python simulate.py --aimmu-config setup/kmeans_baseline_bal.cfg | tee stats/kmeans_baseline_bal/kmeans_baseline_bal.log &
#mkdir stats/sgemm_baseline_bal
#python simulate.py --aimmu-config setup/sgemm_baseline_bal.cfg | tee stats/sgemm_baseline_bal/sgemm_baseline_bal.log 
#mkdir stats/svm_baseline_bal
#python simulate.py --aimmu-config setup/svm_baseline_bal.cfg | tee stats/svm_baseline_bal/svm_baseline_bal.log &
#mkdir stats/streamcluster_baseline_bal
#python simulate.py --aimmu-config setup/streamcluster_baseline_bal.cfg | tee stats/streamcluster_baseline_bal/streamcluster_baseline_bal.log 


#mkdir stats/spmv_baseline_rand_bal
#python simulate.py --aimmu-config setup/spmv_baseline_rand_bal.cfg | tee stats/spmv_baseline_rand_bal/spmv_baseline_rand_bal.log & 
#mkdir stats/pagerank_baseline_rand_bal
#python simulate.py --aimmu-config setup/pagerank_baseline_rand_bal.cfg | tee stats/pagerank_baseline_rand_bal/pagerank_baseline_rand_bal.log  &
#mkdir stats/backprop_baseline_rand_bal
#python simulate.py --aimmu-config setup/backprop_baseline_rand_bal.cfg  | tee stats/backprop_baseline_rand_bal/backprop_baseline_rand_bal.log &
#mkdir stats/rbm_baseline_rand_bal
#python simulate.py --aimmu-config setup/rbm_baseline_rand_bal.cfg | tee stats/rbm_baseline_rand_bal/rbm_baseline_rand_bal.log & 
#mkdir stats/lud_baseline_rand_bal
#python simulate.py --aimmu-config setup/lud_baseline_rand_bal.cfg | tee stats/lud_baseline_rand_bal/lud_baseline_rand_bal.log & 
#mkdir stats/mac_baseline_rand_bal
#python simulate.py --aimmu-config setup/mac_baseline_rand_bal.cfg | tee stats/mac_baseline_rand_bal/mac_baseline_rand_bal.log &
#mkdir stats/reduce_baseline_rand_bal
#python simulate.py --aimmu-config setup/reduce_baseline_rand_bal.cfg | tee stats/reduce_baseline_rand_bal/reduce_baseline_rand_bal.log 

#============================================================================================#
#||||||||||||||||||||||||||||||||| LDB+TOM ||||||||||||||||||||||||||||||||||||||||||||||||||#
#============================================================================================#

#mkdir stats/backprop_baseline_bal_tom
#python simulate.py --aimmu-config setup/backprop_baseline_bal_tom.cfg  | tee stats/backprop_baseline_bal_tom/backprop_baseline_bal_tom.log &
#mkdir stats/spmv_baseline_bal_tom
#python simulate.py --aimmu-config setup/spmv_baseline_bal_tom.cfg | tee stats/spmv_baseline_bal_tom/spmv_baseline_bal_tom.log  
#mkdir stats/pagerank_baseline_bal_tom
#python simulate.py --aimmu-config setup/pagerank_baseline_bal_tom.cfg | tee stats/pagerank_baseline_bal_tom/pagerank_baseline_bal_tom.log & 
#mkdir stats/rbm_baseline_bal_tom
#python simulate.py --aimmu-config setup/rbm_baseline_bal_tom.cfg | tee stats/rbm_baseline_bal_tom/rbm_baseline_bal_tom.log 
#mkdir stats/lud_baseline_bal_tom
#python simulate.py --aimmu-config setup/lud_baseline_bal_tom.cfg | tee stats/lud_baseline_bal_tom/lud_baseline_bal_tom.log  &
#mkdir stats/mac_baseline_bal_tom
#python simulate.py --aimmu-config setup/mac_baseline_bal_tom.cfg | tee stats/mac_baseline_bal_tom/mac_baseline_bal_tom.log 
#mkdir stats/reduce_baseline_bal_tom
#python simulate.py --aimmu-config setup/reduce_baseline_bal_tom.cfg | tee stats/reduce_baseline_bal_tom/reduce_baseline_bal_tom.log  &
#mkdir stats/kmeans_baseline_bal_tom
#python simulate.py --aimmu-config setup/kmeans_baseline_bal_tom.cfg | tee stats/kmeans_baseline_bal_tom/kmeans_baseline_bal_tom.log 
#mkdir stats/sgemm_baseline_bal_tom
#python simulate.py --aimmu-config setup/sgemm_baseline_bal_tom.cfg | tee stats/sgemm_baseline_bal_tom/sgemm_baseline_bal_tom.log &
#mkdir stats/svm_baseline_bal_tom
#python simulate.py --aimmu-config setup/svm_baseline_bal_tom.cfg | tee stats/svm_baseline_bal_tom/svm_baseline_bal_tom.log 
#mkdir stats/streamcluster_baseline_bal_tom
#python simulate.py --aimmu-config setup/streamcluster_baseline_bal_tom.cfg | tee stats/streamcluster_baseline_bal_tom/streamcluster_baseline_bal_tom.log &

#============================================================================================#
#||||||||||||||||||||||||||||||||| LDB+AIMM ||||||||||||||||||||||||||||||||||||||||||||||||||#
#============================================================================================#

# mkdir stats/backprop_learning_bal
# python simulate.py --aimmu-config setup/backprop_learning_bal.cfg  | tee stats/backprop_learning_bal/backprop_learning_bal.log &
# mkdir stats/spmv_learning_bal
# python simulate.py --aimmu-config setup/spmv_learning_bal.cfg | tee stats/spmv_learning_bal/spmv_learning_bal.log &  
# mkdir stats/pagerank_learning_bal
# python simulate.py --aimmu-config setup/pagerank_learning_bal.cfg | tee stats/pagerank_learning_bal/pagerank_learning_bal.log & 
# mkdir stats/rbm_learning_bal
# python simulate.py --aimmu-config setup/rbm_learning_bal.cfg | tee stats/rbm_learning_bal/rbm_learning_bal.log & 
# mkdir stats/lud_learning_bal
# python simulate.py --aimmu-config setup/lud_learning_bal.cfg | tee stats/lud_learning_bal/lud_learning_bal.log  &
# mkdir stats/mac_learning_bal
# python simulate.py --aimmu-config setup/mac_learning_bal.cfg | tee stats/mac_learning_bal/mac_learning_bal.log & 
# mkdir stats/reduce_learning_bal
# python simulate.py --aimmu-config setup/reduce_learning_bal.cfg | tee stats/reduce_learning_bal/reduce_learning_bal.log  &
# mkdir stats/kmeans_learning_bal
# python simulate.py --aimmu-config setup/kmeans_learning_bal.cfg | tee stats/kmeans_learning_bal/kmeans_learning_bal.log & 
# mkdir stats/streamcluster_learning_bal
# python simulate.py --aimmu-config setup/streamcluster_learning_bal.cfg | tee stats/streamcluster_learning_bal/streamcluster_learning_bal.log &
# mkdir stats/sgemm_learning_bal
# python simulate.py --aimmu-config setup/sgemm_learning_bal.cfg | tee stats/sgemm_learning_bal/sgemm_learning_bal.log &
# mkdir stats/svm_learning_bal
# python simulate.py --aimmu-config setup/svm_learning_bal.cfg | tee stats/svm_learning_bal/svm_learning_bal.log &


#============================================================================================#
#||||||||||||||||||||||||||||||||| PEI ||||||||||||||||||||||||||||||||||||||||||||||||||||||#
#============================================================================================#

#mkdir stats/spmv_baseline_pei
#python simulate.py --aimmu-config setup/spmv_baseline_pei.cfg | tee stats/spmv_baseline_pei/spmv_baseline_pei.log &  
#mkdir stats/pagerank_baseline_pei
#python simulate.py --aimmu-config setup/pagerank_baseline_pei.cfg | tee stats/pagerank_baseline_pei/pagerank_baseline_pei.log &
#mkdir stats/backprop_baseline_pei
#python simulate.py --aimmu-config setup/backprop_baseline_pei.cfg  | tee stats/backprop_baseline_pei/backprop_baseline_pei.log & 
#mkdir stats/rbm_baseline_pei
#python simulate.py --aimmu-config setup/rbm_baseline_pei.cfg | tee stats/rbm_baseline_pei/rbm_baseline_pei.log & 
#mkdir stats/lud_baseline_pei
#python simulate.py --aimmu-config setup/lud_baseline_pei.cfg | tee stats/lud_baseline_pei/lud_baseline_pei.log  &
#mkdir stats/mac_baseline_pei
#python simulate.py --aimmu-config setup/mac_baseline_pei.cfg | tee stats/mac_baseline_pei/mac_baseline_pei.log &
#mkdir stats/reduce_baseline_pei
#python simulate.py --aimmu-config setup/reduce_baseline_pei.cfg | tee stats/reduce_baseline_pei/reduce_baseline_pei.log &
#mkdir stats/kmeans_baseline_pei
#python simulate.py --aimmu-config setup/kmeans_baseline_pei.cfg | tee stats/kmeans_baseline_pei/kmeans_baseline_pei.log &
#mkdir stats/sgemm_baseline_pei
#python simulate.py --aimmu-config setup/sgemm_baseline_pei.cfg | tee stats/sgemm_baseline_pei/sgemm_baseline_pei.log &
#mkdir stats/svm_baseline_pei
#python simulate.py --aimmu-config setup/svm_baseline_pei.cfg | tee stats/svm_baseline_pei/svm_baseline_pei.log &
#mkdir stats/streamcluster_baseline_pei
#python simulate.py --aimmu-config setup/streamcluster_baseline_pei.cfg | tee stats/streamcluster_baseline_pei/streamcluster_baseline_pei.log &

#============================================================================================#
#||||||||||||||||||||||||||||||||| PEI+TOM ||||||||||||||||||||||||||||||||||||||||||||||||||#
#============================================================================================#

#mkdir stats/spmv_baseline_pei_tom
#python simulate.py --aimmu-config setup/spmv_baseline_pei_tom.cfg | tee stats/spmv_baseline_pei_tom/spmv_baseline_pei_tom.log   
#mkdir stats/pagerank_baseline_pei_tom
#python simulate.py --aimmu-config setup/pagerank_baseline_pei_tom.cfg | tee stats/pagerank_baseline_pei_tom/pagerank_baseline_pei_tom.log &
#mkdir stats/backprop_baseline_pei_tom
#python simulate.py --aimmu-config setup/backprop_baseline_pei_tom.cfg  | tee stats/backprop_baseline_pei_tom/backprop_baseline_pei_tom.log 
#mkdir stats/rbm_baseline_pei_tom
#python simulate.py --aimmu-config setup/rbm_baseline_pei_tom.cfg | tee stats/rbm_baseline_pei_tom/rbm_baseline_pei_tom.log & 
#mkdir stats/lud_baseline_pei_tom
#python simulate.py --aimmu-config setup/lud_baseline_pei_tom.cfg | tee stats/lud_baseline_pei_tom/lud_baseline_pei_tom.log  
#mkdir stats/mac_baseline_pei_tom
#python simulate.py --aimmu-config setup/mac_baseline_pei_tom.cfg | tee stats/mac_baseline_pei_tom/mac_baseline_pei_tom.log &
#mkdir stats/reduce_baseline_pei_tom
#python simulate.py --aimmu-config setup/reduce_baseline_pei_tom.cfg | tee stats/reduce_baseline_pei_tom/reduce_baseline_pei_tom.log 
#mkdir stats/kmeans_baseline_pei_tom
#python simulate.py --aimmu-config setup/kmeans_baseline_pei_tom.cfg | tee stats/kmeans_baseline_pei_tom/kmeans_baseline_pei_tom.log &
#mkdir stats/sgemm_baseline_pei_tom
#python simulate.py --aimmu-config setup/sgemm_baseline_pei_tom.cfg | tee stats/sgemm_baseline_pei_tom/sgemm_baseline_pei_tom.log 
#mkdir stats/svm_baseline_pei_tom
#python simulate.py --aimmu-config setup/svm_baseline_pei_tom.cfg | tee stats/svm_baseline_pei_tom/svm_baseline_pei_tom.log &
#mkdir stats/streamcluster_baseline_pei_tom
#python simulate.py --aimmu-config setup/streamcluster_baseline_pei_tom.cfg | tee stats/streamcluster_baseline_pei_tom/streamcluster_baseline_pei_tom.log 
#

#============================================================================================#
#||||||||||||||||||||||||||||||||| PEI+AIMM ||||||||||||||||||||||||||||||||||||||||||||||||||#
#============================================================================================#

#mkdir stats/spmv_learning_pei
#python simulate.py --aimmu-config setup/spmv_learning_pei.cfg | tee stats/spmv_learning_pei/spmv_learning_pei.log &  
#mkdir stats/pagerank_learning_pei
#python simulate.py --aimmu-config setup/pagerank_learning_pei.cfg | tee stats/pagerank_learning_pei/pagerank_learning_pei.log &
#mkdir stats/backprop_learning_pei
#python simulate.py --aimmu-config setup/backprop_learning_pei.cfg  | tee stats/backprop_learning_pei/backprop_learning_pei.log & 
#mkdir stats/rbm_learning_pei
#python simulate.py --aimmu-config setup/rbm_learning_pei.cfg | tee stats/rbm_learning_pei/rbm_learning_pei.log & 
#mkdir stats/lud_learning_pei
#python simulate.py --aimmu-config setup/lud_learning_pei.cfg | tee stats/lud_learning_pei/lud_learning_pei.log & 
#mkdir stats/mac_learning_pei
#python simulate.py --aimmu-config setup/mac_learning_pei.cfg | tee stats/mac_learning_pei/mac_learning_pei.log &
#mkdir stats/reduce_learning_pei
#python simulate.py --aimmu-config setup/reduce_learning_pei.cfg | tee stats/reduce_learning_pei/reduce_learning_pei.log & 
#mkdir stats/kmeans_learning_pei
#python simulate.py --aimmu-config setup/kmeans_learning_pei.cfg | tee stats/kmeans_learning_pei/kmeans_learning_pei.log &
#mkdir stats/streamcluster_learning_pei
#python simulate.py --aimmu-config setup/streamcluster_learning_pei.cfg | tee stats/streamcluster_learning_pei/streamcluster_learning_pei.log & 

##mkdir stats/sgemm_learning_pei
##python simulate.py --aimmu-config setup/sgemm_learning_pei.cfg | tee stats/sgemm_learning_pei/sgemm_learning_pei.log & 
##mkdir stats/svm_learning_pei
##python simulate.py --aimmu-config setup/svm_learning_pei.cfg | tee stats/svm_learning_pei/svm_learning_pei.log &


################################# MIX OF 2 BENCH (baseline) ###############################################
#mkdir stats/backprop_lud_baseline
#python simulate.py --aimmu-config setup/backprop_lud_baseline.cfg  | tee stats/backprop_lud_baseline/backprop_lud_baseline.log &
#mkdir stats/backprop_mac_baseline
#python simulate.py --aimmu-config setup/backprop_mac_baseline.cfg  | tee stats/backprop_mac_baseline/backprop_mac_baseline.log &
#mkdir stats/backprop_pagerank_baseline
#python simulate.py --aimmu-config setup/backprop_pagerank_baseline.cfg  | tee stats/backprop_pagerank_baseline/backprop_pagerank_baseline.log &
#mkdir stats/backprop_rbm_baseline
#python simulate.py --aimmu-config setup/backprop_rbm_baseline.cfg  | tee stats/backprop_rbm_baseline/backprop_rbm_baseline.log &
#mkdir stats/backprop_reduce_baseline
#python simulate.py --aimmu-config setup/backprop_reduce_baseline.cfg  | tee stats/backprop_reduce_baseline/backprop_reduce_baseline.log &
#mkdir stats/backprop_spmv_baseline
#python simulate.py --aimmu-config setup/backprop_spmv_baseline.cfg  | tee stats/backprop_spmv_baseline/backprop_spmv_baseline.log 
#
#mkdir stats/lud_mac_baseline
#python simulate.py --aimmu-config setup/lud_mac_baseline.cfg | tee stats/lud_mac_baseline/lud_mac_baseline.log  
#mkdir stats/lud_pagerank_baseline
#python simulate.py --aimmu-config setup/lud_pagerank_baseline.cfg | tee stats/lud_pagerank_baseline/lud_pagerank_baseline.log  &
#mkdir stats/lud_rbm_baseline
#python simulate.py --aimmu-config setup/lud_rbm_baseline.cfg | tee stats/lud_rbm_baseline/lud_rbm_baseline.log  &
#mkdir stats/lud_reduce_baseline
#python simulate.py --aimmu-config setup/lud_reduce_baseline.cfg | tee stats/lud_reduce_baseline/lud_reduce_baseline.log  &
#mkdir stats/lud_spmv_baseline
#python simulate.py --aimmu-config setup/lud_spmv_baseline.cfg | tee stats/lud_spmv_baseline/lud_spmv_baseline.log  
#
#mkdir stats/mac_pagerank_baseline
#python simulate.py --aimmu-config setup/mac_pagerank_baseline.cfg | tee stats/mac_pagerank_baseline/mac_pagerank_baseline.log 
#mkdir stats/mac_rbm_baseline
#python simulate.py --aimmu-config setup/mac_rbm_baseline.cfg | tee stats/mac_rbm_baseline/mac_rbm_baseline.log &
#mkdir stats/mac_reduce_baseline
#python simulate.py --aimmu-config setup/mac_reduce_baseline.cfg | tee stats/mac_reduce_baseline/mac_reduce_baseline.log &
#mkdir stats/mac_spmv_baseline
#python simulate.py --aimmu-config setup/mac_spmv_baseline.cfg | tee stats/mac_spmv_baseline/mac_spmv_baseline.log &
#
#mkdir stats/pagerank_rbm_baseline
#python simulate.py --aimmu-config setup/pagerank_rbm_baseline.cfg | tee stats/pagerank_rbm_baseline/pagerank_rbm_baseline.log & 
#mkdir stats/pagerank_reduce_baseline
#python simulate.py --aimmu-config setup/pagerank_reduce_baseline.cfg | tee stats/pagerank_reduce_baseline/pagerank_reduce_baseline.log  
#mkdir stats/pagerank_spmv_baseline
#python simulate.py --aimmu-config setup/pagerank_spmv_baseline.cfg | tee stats/pagerank_spmv_baseline/pagerank_spmv_baseline.log & 
#
#mkdir stats/rbm_reduce_baseline
#python simulate.py --aimmu-config setup/rbm_reduce_baseline.cfg | tee stats/rbm_reduce_baseline/rbm_reduce_baseline.log & 
#mkdir stats/rbm_spmv_baseline
#python simulate.py --aimmu-config setup/rbm_spmv_baseline.cfg | tee stats/rbm_spmv_baseline/rbm_spmv_baseline.log & 
#
#mkdir stats/reduce_spmv_baseline
#python simulate.py --aimmu-config setup/reduce_spmv_baseline.cfg | tee stats/reduce_spmv_baseline/reduce_spmv_baseline.log 


#============================================================================================#
#||||||||||||||||||||||||||||||||| RANDOM ALLOC |||||||||||||||||||||||||||||||||||||||||||||#
#============================================================================================#

#mkdir stats/spmv_baseline_rand
#python simulate.py --aimmu-config setup/spmv_baseline_rand.cfg | tee stats/spmv_baseline_rand/spmv_baseline_rand.log & 
#mkdir stats/pagerank_baseline_rand
#python simulate.py --aimmu-config setup/pagerank_baseline_rand.cfg | tee stats/pagerank_baseline_rand/pagerank_baseline_rand.log  &
#mkdir stats/backprop_baseline_rand
#python simulate.py --aimmu-config setup/backprop_baseline_rand.cfg  | tee stats/backprop_baseline_rand/backprop_baseline_rand.log &
#mkdir stats/rbm_baseline_rand
#python simulate.py --aimmu-config setup/rbm_baseline_rand.cfg | tee stats/rbm_baseline_rand/rbm_baseline_rand.log & 
#mkdir stats/lud_baseline_rand
#python simulate.py --aimmu-config setup/lud_baseline_rand.cfg | tee stats/lud_baseline_rand/lud_baseline_rand.log 
#mkdir stats/mac_baseline_rand
#python simulate.py --aimmu-config setup/mac_baseline_rand.cfg | tee stats/mac_baseline_rand/mac_baseline_rand.log &
#mkdir stats/reduce_baseline_rand
#python simulate.py --aimmu-config setup/reduce_baseline_rand.cfg | tee stats/reduce_baseline_rand/reduce_baseline_rand.log 

#============================================================================================#
#||||||||||||||||||||| BNMP+AIMM - Forward Results (Comp Migr) PKT SIM DISABLED |||||||||||||#
#============================================================================================#
#mkdir stats/kmeans_learning_no_fwd_sim
#python simulate.py --aimmu-config setup/kmeans_learning_no_fwd_sim.cfg | tee stats/kmeans_learning_no_fwd_sim/kmeans_learning_no_fwd_sim.log & 
##mkdir stats/sgemm_learning_no_fwd_sim
##python simulate.py --aimmu-config setup/sgemm_learning_no_fwd_sim.cfg | tee stats/sgemm_learning_no_fwd_sim/sgemm_learning_no_fwd_sim.log &
#mkdir stats/svm_learning_no_fwd_sim
#python simulate.py --aimmu-config setup/svm_learning_no_fwd_sim.cfg | tee stats/svm_learning_no_fwd_sim/svm_learning_no_fwd_sim.log &
#mkdir stats/streamcluster_learning_no_fwd_sim
#python simulate.py --aimmu-config setup/streamcluster_learning_no_fwd_sim.cfg | tee stats/streamcluster_learning_no_fwd_sim/streamcluster_learning_no_fwd_sim.log &
#mkdir stats/lud_learning_no_fwd_sim
#python simulate.py --aimmu-config setup/lud_learning_no_fwd_sim.cfg --graph-folder stats/lud_learning_no_fwd_sim  | tee stats/lud_learning_no_fwd_sim/lud_learning_no_fwd_sim.log & 
#mkdir stats/rbm_learning_no_fwd_sim
#python simulate.py --aimmu-config setup/rbm_learning_no_fwd_sim.cfg  --graph-folder  stats/rbm_learning_no_fwd_sim | tee stats/rbm_learning_no_fwd_sim/rbm_learning_no_fwd_sim.log &
#mkdir stats/spmv_learning_no_fwd_sim
#python simulate.py --aimmu-config setup/spmv_learning_no_fwd_sim.cfg --graph-folder stats/spmv_learning_no_fwd_sim  | tee stats/spmv_learning_no_fwd_sim/spmv_learning_no_fwd_sim.log & 
#mkdir stats/mac_learning_no_fwd_sim
#python simulate.py --aimmu-config setup/mac_learning_no_fwd_sim.cfg --graph-folder stats/mac_learning_no_fwd_sim | tee stats/mac_learning_no_fwd_sim/mac_learning_no_fwd_sim.log &
#mkdir stats/reduce_learning_no_fwd_sim
#python simulate.py --aimmu-config setup/reduce_learning_no_fwd_sim.cfg --graph-folder stats/reduce_learning_no_fwd_sim | tee stats/reduce_learning_no_fwd_sim/reduce_learning_no_fwd_sim.log &
#mkdir stats/pagerank_learning_no_fwd_sim
#python simulate.py --aimmu-config setup/pagerank_learning_no_fwd_sim.cfg --graph-folder stats/pagerank_learning_no_fwd_sim  | tee stats/pagerank_learning_no_fwd_sim/pagerank_learning_no_fwd_sim.log &
#mkdir stats/backprop_learning_no_fwd_sim
#python simulate.py --aimmu-config setup/backprop_learning_no_fwd_sim.cfg --graph-folder stats/backprop_learning_no_fwd_sim | tee stats/backprop_learning_no_fwd_sim/backprop_learning_no_fwd_sim.log &

