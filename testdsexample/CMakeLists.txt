cmake_minimum_required (VERSION 3.14)
project (testexample C CXX)
set (CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLINUX -g -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX -g -fPIC")



set (DSINCLUDE 
/global/cscratch1/sd/zw241/ds_install/include
/opt/cray/pe/pmi/5.0.14/include 
/opt/cray/alps/6.6.56-7.0.1.1_4.30__g2e60a7e4.ari/include 
/opt/cray/wlm_detect/1.3.3-7.0.1.1_4.9__g7109084.ari/include 
/opt/cray/ugni/6.0.14.0-7.0.1.1_7.19__ge78e5b0.ari/include 
/opt/cray/gni-headers/5.0.12.0-7.0.1.1_6.17__g3b1768f.ari/include 
/opt/cray/alps/6.6.56-7.0.1.1_4.30__g2e60a7e4.ari/include 
/opt/cray/rca/2.2.20-7.0.1.1_4.25__g8e3fb5b.ari/include 
/opt/cray/krca/2.2.6-7.0.1.1_5.21__gb641b12.ari/include 
/opt/cray-hss-devel/9.0.0/include 
/opt/cray/ugni/6.0.14.0-7.0.1.1_7.19__ge78e5b0.ari/include 
/opt/cray/gni-headers/5.0.12.0-7.0.1.1_6.17__g3b1768f.ari/include 
/opt/cray/rdma-credentials/1.2.25-7.0.1.1_4.28__ga0a409f.ari/include 
/opt/cray/job/2.2.4-7.0.1.1_3.25__g36b56f4.ari/include 
/opt/cray/ugni/6.0.14.0-7.0.1.1_7.19__ge78e5b0.ari/include 
/opt/cray/gni-headers/5.0.12.0-7.0.1.1_6.17__g3b1768f.ari/include
)

set (DSLINKDIR 
/global/cscratch1/sd/zw241/ds_install/lib
/opt/cray/pe/pmi/5.0.14/lib64
/opt/cray/ugni/6.0.14.0-7.0.1.1_7.19__ge78e5b0.ari/lib64
/opt/cray/rdma-credentials/1.2.25-7.0.1.1_4.28__ga0a409f.ari/lib64
/opt/cray/job/2.2.4-7.0.1.1_3.25__g36b56f4.ari/lib64
)

set (DSLINKLIB -ldspaces -ldscommon -ldart -lm -lrt -lpmi -lugni 
-ldrc -ujob_create -ljob -ujobctl -ljobctl -lugni -lpthread -lm -lstdc++)

find_package(MPI REQUIRED)

add_executable(testput
  ./put.c)
  
target_include_directories(testput PUBLIC ${DSINCLUDE})
target_link_directories(testput PUBLIC ${DSLINKDIR})
target_link_libraries(testput MPI::MPI_C ${DSLINKLIB})