PRINT_EACH=$1

if [ -z "$PRINT_EACH" ]; then
    echo "Specify print interval"
    echo "Example: ./custom_run.sh 100"
    exit 0
fi

# mkdir -p stats/spmv_baseline
# python sim_no_agent.py --aimmu-config setup/new_spmv_baseline.cfg --print-each $PRINT_EACH | tee stats/spmv_baseline/spmv_baseline.log

# mkdir -p stats/pagerank_baseline
# python sim_no_agent.py --aimmu-config setup/new_pagerank_baseline.cfg --print-each $PRINT_EACH | tee stats/pagerank_baseline/pagerank_baseline.log

# mkdir -p stats/backprop_baseline
# python sim_no_agent.py --aimmu-config setup/new_backprop_baseline.cfg  --print-each $PRINT_EACH | tee stats/backprop_baseline/backprop_baseline.log

# mkdir -p stats/rbm_baseline
# python sim_no_agent.py --aimmu-config setup/new_rbm_baseline.cfg --print-each $PRINT_EACH | tee stats/rbm_baseline/rbm_baseline.log

# mkdir -p stats/lud_baseline
# python sim_no_agent.py --aimmu-config setup/new_lud_baseline.cfg --print-each $PRINT_EACH | tee stats/lud_baseline/lud_baseline.log 

mkdir -p stats/sgemm_baseline
python sim_no_agent.py --aimmu-config setup/new_sgemm_baseline.cfg --print-each $PRINT_EACH | tee stats/sgemm_baseline/sgemm_baseline.log

# mkdir -p stats/svm_baseline
# python sim_no_agent.py --aimmu-config setup/new_svm_baseline.cfg --print-each $PRINT_EACH | tee stats/svm_baseline/svm_baseline.log

# mkdir -p stats/streamcluster_baseline
# python sim_no_agent.py --aimmu-config setup/new_streamcluster_baseline.cfg --print-each $PRINT_EACH | tee stats/streamcluster_baseline/streamcluster_baseline.log

# mkdir -p stats/mac_baseline
# python sim_no_agent.py --aimmu-config setup/new_mac_baseline.cfg --print-each $PRINT_EACH | tee stats/mac_baseline/mac_baseline.log

# mkdir -p stats/reduce_baseline
# python sim_no_agent.py --aimmu-config setup/new_reduce_baseline.cfg --print-each $PRINT_EACH | tee stats/reduce_baseline/reduce_baseline.log

# mkdir -p stats/kmeans_baseline
# python sim_no_agent.py --aimmu-config setup/new_kmeans_baseline.cfg --print-each $PRINT_EACH | tee stats/kmeans_baseline/kmeans_baseline.log
