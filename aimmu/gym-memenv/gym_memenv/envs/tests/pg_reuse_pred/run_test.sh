#### Example
#./test ../../../../../../trace_folder/benchmarks/naive-art backprop 2 affinity_folder

#cat spmv.pg | awk -F, '{print $2}' | sort | uniq -c


#### Actual Run
./test ../../../../../../trace_folder/benchmarks/naive-art backprop 3354 affinity_folder | tee affinity_folder/backprop.aff
./test ../../../../../../trace_folder/benchmarks/naive-art lud 629 affinity_folder | tee affinity_folder/lud.aff
./test ../../../../../../trace_folder/benchmarks/naive-art pagerank 595 affinity_folder | tee affinity_folder/pagerank.aff
./test ../../../../../../trace_folder/benchmarks/naive-art rbm 1133 affinity_folder | tee affinity_folder/rbm.aff
./test ../../../../../../trace_folder/benchmarks/naive-art spmv 502 affinity_folder | tee affinity_folder/spmv.aff
./test ../../../../../../trace_folder/microbench naive_art_mac 640 affinity_folder | tee affinity_folder/mac.aff
./test ../../../../../../trace_folder/microbench naive_art_reduce 640 affinity_folder | tee affinity_folder/reduce.aff
./test ../../../../../../trace_folder/benchmarks/naive-art kmeans 3000 affinity_folder | tee affinity_folder/kmeans.aff 
./test ../../../../../../trace_folder/benchmarks/naive-art streamcluster 3000 affinity_folder | tee affinity_folder/streamcluster.aff


#./test ../../../../../../trace_folder/benchmarks/naive-art svm 3000 affinity_folder | tee affinity_folder/svm.aff
#./test ../../../../../../trace_folder/benchmarks/naive-art sgemm 3000 affinity_folder | tee affinity_folder/sgemm.aff

#./test ../../../../../../trace_folder/benchmarks/naive-art kmeans 3000 input 
#./test ../../../../../../trace_folder/benchmarks/naive-art sgemm 3000 input 
#./test ../../../../../../trace_folder/benchmarks/naive-art streamcluster 3000 input 
#./test ../../../../../../trace_folder/benchmarks/naive-art svm 3000 input 
