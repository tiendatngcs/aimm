###########################################
######### time series plot ################
###########################################
#python <plotting script> <dest folder> <graph name> <legends> <graph title> <labels>  [<series1.csv> <series2.csv> ...<seriesn.csv>]
python series_plot.py ../../../stats/spmv_learning hop_timeline.pdf "baseline,aimmu" "hop-average timeline"
"Epoch,hop-average" ../../../stats/spmv_baseline/hop_avg_50-44-12-15-11-2020.csv ../../../stats/spmv_learning/hop_avg_29-17-04-15-11-2020.csv


###########################################
######## normalize to base ################
###########################################
#python <plotting script> <dest folder> <graph name> <legends> <graph title> <labels>  [<stats.out> <stats.out>...<stats.out>]
python stats_plot_norm_bar.py trial trial.pdf "baseline,aimmu" "Hop Average" "Benchmarks,hop-average" \ 
../../../stats/rbm_baseline/stats.out ../../../stats/rbm_learning/stats.out ../../../stats/spmv_baseline/stats.out \
../../../stats/spmv_learning/stats.out ../../../stats/backprop_baseline/stats.out \ 
../../../stats/backprop_learning/stats.out ../../../stats/pagerank_baseline/stats.out ../../../stats/pagerank_learrning/stats.out

############################################
####### single bar #########################
############################################
python stats_plot_single_bar.py ../../../stats/results perc_mig.pdf "baseline,aimmu" "Migration Average " "Benchmarks,% of
migration"  ../../../stats/rbm_learning/stats.out  ../../../stats/spmv_learning/stats.out
../../../stats/backprop_learning/stats.out ../../../stats/pagerank_learrning/stats.out

python stats_plot_single_bar.py ../../../stats/results perc_mig_acc.pdf "Percentage of Migrated Page Ac 
cess" "Benchmarks,% of migrated page access"  ../../../stats/rbm_learning/stats.out
../../../stats/spmv_learning/stats.out  ../../../stats/backprop_learning/stats.out
../../../stats/pagerank_learrning/stats. 
out     


##################################################
####### stacked bar  #############################
##################################################
python stats_plot_stack_bar.py ../../../stats/results_21_11_2020_12_35_pm_0x2_rolloverrs latency_breakd own.pdf "Latency
Breakdown" "Latency Breakdown" "Benchmarks,Latency"  
../../../stats/rbm_learning/stats.out
../../../stats/spmv_learning/stats.out  
../../../stats/backprop_learning/stats.out 
../../../stats/pagerank_learrning/stats.out

python stats_plot_stack_bar.py ../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/graphs
latency_breakdown.pdf "Latency Breakdown" "Benchmarks,Latency"
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/spmv_baseline/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_roll overs_no_migr_sim/spmv_learning/stats.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/spmv_learning/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/pageran k_baseline/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/pagerank_learning/stats.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/pagerank_learning/stats. out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/lud_baseline/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/lud_learning/stats.out 
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/lud_learning/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/mac_baseline/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_ rollovers_no_migr_sim/mac_learning/stats.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/mac_learning/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/reduc e_baseline/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/reduce_learning/stats.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/reduce_learning/stats.out



#############################################################
####### normal bar with any number of techniques ############
#############################################################
python stats_plot_norm_bar_anynum.py ../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/ graphs speedup.pdf
"baseline,aimmu_no_migr_sim,aimmu_migr_sim" "Speedup" "Benchmarks,Speedup" 
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/spmv_baseline/stats.out 
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/spmv_learning/stats.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/spmv_learning/stats.out
../../../stats/results_01_12_2020_08_11_pm_0 x2_rollovers_no_migr_sim/pagerank_baseline/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/pagerank_learning/stats.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_m igr_sim/pagerank_learning/stats.out

python stats_plot_norm_bar_anynum.py ../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/ graphs
pim_op_latency.pdf "baseline,aimmu_no_migr_sim,aimmu_migr_sim" "Pim-Op Latency" "Benchmarks,Latency" 
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/spmv_baseline/stats.out 
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/spmv_learning/stats.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/spmv_learning/stats.out 
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/pagerank_baseline/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/pagerank_learning/stats.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/pagerank_learning/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/lud_baseline/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_ sim/lud_learning/stats.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/lud_learning/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/mac_baseline/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/mac_learning/stats.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/mac_learning/stats.out 
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/reduce_baseline/stats.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/reduce_learning/stats.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/reduce_learning/stats.out



#######################################################
####### stacked bar [VEC] #############################
#######################################################

python stats_vector_plot_stack_bar.py ../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/graphs
pim_tab_full_hist.pdf "Pim Table Full Hist" "Benchmarks,Count"
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/spmv_baseline/stats_vec.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/spmv_learning/stats_vec.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/spmv_learning/stats_vec.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_ no_migr_sim/pagerank_baseline/stats_vec.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/pagerank_learning/stats_vec.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_s im/pagerank_learning/stats_vec.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/lud_baseline/stats_vec.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/lud_lear ning/stats_vec.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/lud_learning/stats_vec.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/mac_baseline/stats_vec.out 
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/mac_learning/stats_vec.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/mac_learning/stats_vec.out 
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/reduce_baseline/stats_vec.out
../../../stats/results_01_12_2020_08_11_pm_0x2_rollovers_no_migr_sim/reduce_learning/stats_vec.out
../../../stats/results_03_12_2020_12_13_pm_0x2_rollovers_migr_sim/reduce_learning/stats_vec.out
