cmake_minimum_required (VERSION 3.14)
project (gray-scott C CXX)
set (CMAKE_CXX_STANDARD 14)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLINUX -g -Wno-dev")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX -g -Wno-dev")

# check the following include dir based on the dataspace installation
set (DSINCLUDE 
/global/cscratch1/sd/zw241/ds_install/include
/opt/cray/pe/pmi/5.0.14/include 
/opt/cray/alps/6.6.57-7.0.1.1_5.1__g1b735148.ari/include
/opt/cray/wlm_detect/1.3.3-7.0.1.1_4.11__g7109084.ari/include
/opt/cray/ugni/6.0.14.0-7.0.1.1_7.27__ge78e5b0.ari/include
/opt/cray/gni-headers/5.0.12.0-7.0.1.1_6.24__g3b1768f.ari/include
/opt/cray/alps/6.6.57-7.0.1.1_5.1__g1b735148.ari/include
/opt/cray/rca/2.2.20-7.0.1.1_4.33__g8e3fb5b.ari/include
/opt/cray/krca/2.2.6-7.0.1.1_5.27__gb641b12.ari/include
/opt/cray-hss-devel/9.0.0/include  
/opt/cray/ugni/6.0.14.0-7.0.1.1_7.27__ge78e5b0.ari/include
/opt/cray/gni-headers/5.0.12.0-7.0.1.1_6.24__g3b1768f.ari/include
/opt/cray/rdma-credentials/1.2.25-7.0.1.1_4.35__ga0a409f.ari/include
/opt/cray/job/2.2.4-7.0.1.1_3.29__g36b56f4.ari/include
/opt/cray/ugni/6.0.14.0-7.0.1.1_7.27__ge78e5b0.ari/include
/opt/cray/gni-headers/5.0.12.0-7.0.1.1_6.24__g3b1768f.ari/include
)

set (DSLINKDIR 
/global/cscratch1/sd/zw241/ds_install/lib
/opt/cray/pe/pmi/5.0.14/lib64
/opt/cray/ugni/6.0.14.0-7.0.1.1_7.27__ge78e5b0.ari/lib64
/opt/cray/rdma-credentials/1.2.25-7.0.1.1_4.35__ga0a409f.ari/lib64 
/opt/cray/job/2.2.4-7.0.1.1_3.29__g36b56f4.ari/lib64
/opt/cray/ugni/6.0.14.0-7.0.1.1_7.27__ge78e5b0.ari/lib64
)

set (DSLINKLIB -ldspaces -ldscommon -ldart -lm -lrt -lpmi -lugni 
-ldrc -ujob_create -ljob -ujobctl -ljobctl -lugni -lpthread -lm -lstdc++)

message(STATUS "Enabling profiling timers")
add_definitions(-DENABLE_TIMERS)
find_package(MPI REQUIRED) 


# thallium env
# add our cmake module directory to the path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


# link shared lib with full rpath
set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# setup cache variables for ccmake
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Release
         CACHE STRING "Choose the type of build." FORCE)
    set_property (CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS 
                  "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif ()
set (CMAKE_PREFIX_PATH "" CACHE STRING "External dependencies path")
set (BUILD_SHARED_LIBS "OFF" CACHE BOOL "Build a shared library")

# packages we depend on (load all the necessary depedency by spack)
include (xpkg-import)
find_package (mercury REQUIRED)
include_directories(${MERCURY_INCLUDE_DIR})
xpkg_import_module (argobots REQUIRED argobots)
xpkg_import_module (margo REQUIRED margo)
xpkg_import_module (abtio REQUIRED abt-io)
# find_package (thallium REQUIRED)

add_library(metaclient ./putgetMeta/metaclient.cpp)
target_link_libraries(metaclient)

add_executable(gray-scott
  ./simulation/main.cpp
  ./simulation/gray-scott.cpp
  ./simulation/writer.cpp
  ./setting/settings.cpp)
  
target_include_directories(gray-scott PUBLIC ${gray-scott}/simulation ${DSINCLUDE})
target_link_directories(gray-scott PUBLIC ${DSLINKDIR})
target_link_libraries(gray-scott MPI::MPI_C ${DSLINKLIB})

add_executable(isosurface
   ./analytics/isosurface.cpp
   ./analytics/reader.cpp
   ./setting/settings.cpp)

find_package(VTK COMPONENTS
    FiltersCore
    IOImage
    IOXML
    IOLegacy
)
target_include_directories(isosurface PUBLIC ${gray-scott}/analytics ${DSINCLUDE})
target_link_directories(isosurface PUBLIC ${DSLINKDIR})
target_link_libraries(isosurface MPI::MPI_C ${VTK_LIBRARIES} ${DSLINKLIB})


add_subdirectory(sim_gs)