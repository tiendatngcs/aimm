#./bin/sim_dynamic init/vmap-1.ini init/1GB_4x4_mesh.ini sim_type=$1 
#./sim_dynamic init/vmap-1.ini init/4GB_4x4_mesh.ini dim=100000 stats_file=M4GB_4x4_mesh_100000x1000000 
./bin/rlmmu_pybind.so init/vmap-1.ini init/64GB_40x40_mesh.ini sim_type=$1 N=50 H=16 W=16 C=3 M=2 R=3 S=3 
#./test_dynamic init/vmap-1.ini init/64GB_4x4_mesh.ini dim=2000 stats_file=M64GB_4x4_mesh_2000x20000 &
#./test_dynamic init/vmap-1.ini init/64GB_8x8_mesh.ini dim=100000 stats_file=M64GB_8x8_mesh_100000x100000 &
#./test_dynamic init/vmap-1.ini init/64GB_8x8_mesh.ini dim=1000 stats_file=M64GB_8x8_mesh_1000x1000_dir2 &
#./test_dynamic init/vmap-1.ini init/64GB_16x16_mesh.ini dim=100000 stats_file=M64GB_16x16_mesh_100000x100000 &
#./test_dynamic init/vmap-1.ini init/64GB_16x16_mesh.ini dim=1000 stats_file=M64GB_16x16_mesh_1000x1000_dir2 &
#./test_dynamic init/vmap-1.ini init/64GB_32x32_mesh.ini dim=100000 stats_file=M64GB_32x32_mesh_100000x100000 &
#./test_dynamic init/vmap-1.ini init/64GB_32x32_mesh.ini dim=1000 stats_file=M64GB_32x32_mesh_1000x1000_dir2 &
#./test_dynamic init/vmap-1.ini init/64GB_40x40_mesh.ini dim=1000 stats_file=M64GB_40x40_mesh_1000x1000_dir2 &
#./test_dynamic init/vmap-1.ini init/64GB_40x40_mesh.ini dim=100000 stats_file=M64GB_40x40_mesh_100000x100000 &
#./test_dynamic init/vmap-1.ini init/64GB_singleMem.ini dim=100000 stats_file=M64GB_singleMem_100000x100000 &
