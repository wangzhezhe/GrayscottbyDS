cmake_minimum_required (VERSION 3.15)
project (gray-scott C CXX)
set (CMAKE_CXX_STANDARD 14)

# run the ./dspaces_config to get the data space compile flag and the link flag

set (DSINCLUDE /home/zw241/cworkspace/build/build_dataspaces/include)
set (DSLINKDIR /home/zw241/cworkspace/build/build_dataspaces/lib)
set (DSLINKLIB -ldspaces -ldscommon -ldart -lpthread -lm -lrt -libverbs -lrdmacm)

find_package(MPI REQUIRED) 

add_executable(gray-scott
  ./simulation/main.cpp
  ./simulation/gray-scott.cpp
  ./simulation/settings.cpp
  ./simulation/writer.cpp
  ./simulation/settings.cpp)
  
target_include_directories(gray-scott PUBLIC ${gray-scott}/simulation ${DSINCLUDE})
target_link_directories(gray-scott PUBLIC ${DSLINKDIR})
target_link_libraries(gray-scott MPI::MPI_C ${DSLINKLIB})


add_executable(isosurface
   ./analytics/isosurface.cpp
   ./analytics/reader.cpp
)

find_package(VTK COMPONENTS
    FiltersCore
    IOImage
    IOXML
    IOLegacy
)

target_include_directories(isosurface PUBLIC ${gray-scott}/analytics ${DSINCLUDE})
target_link_directories(isosurface PUBLIC ${DSLINKDIR})
target_link_libraries(isosurface MPI::MPI_C ${DSLINKLIB} ${VTK_LIBRARIES})
