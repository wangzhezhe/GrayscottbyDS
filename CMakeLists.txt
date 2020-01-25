cmake_minimum_required (VERSION 3.15)
project (gray-scott C CXX)
set (CMAKE_CXX_STANDARD 14)

# run the ./dspaces_config to get the data space compile flag and the link flag

set (DSINCLUDE /home/zw241/cworkspace/build/build_dataspaces/include)
set (DSLINKDIR /home/zw241/cworkspace/build/build_dataspaces/lib)
set (DSLINKLIB -ldspaces -ldscommon -ldart -lpthread -lm -lrt -libverbs -lrdmacm)

option(USE_TIMERS "Use profiling timers")
#if(USE_TIMERS)
  message(STATUS "Enabling profiling timers")
  add_definitions(-DENABLE_TIMERS)
#endif()


message("build with grpc")

# GRPC and Protocol Buffers libraries location
list(APPEND CMAKE_PREFIX_PATH "/scratch/zw241/cworkspace/build/build_grpc" "/scratch/zw241/cworkspace/build/build_protobuf")

# Cmake find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Protobuf REQUIRED)
find_package(GRPC REQUIRED)

set(PROTOS
    ${CMAKE_CURRENT_LIST_DIR}/protos/metaserver.proto)

set(PROTO_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto-src)
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})
include_directories(${PROTO_SRC_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC_DIR} ${PROTOS})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} ${PROTOS})

find_package(MPI REQUIRED) 

add_executable(gray-scott
  ./simulation/main.cpp
  ./simulation/gray-scott.cpp
  ./simulation/writer.cpp
  ./setting/settings.cpp
  ./putgetMeta/metaclient.cpp
  ${PROTO_SRCS} 
  ${GRPC_SRCS})
  
target_include_directories(gray-scott PUBLIC ${gray-scott}/simulation ${DSINCLUDE})
target_link_directories(gray-scott PUBLIC ${DSLINKDIR})
target_link_libraries(gray-scott MPI::MPI_C ${DSLINKLIB}
gRPC::grpc++_reflection protobuf::libprotobuf)

add_executable(isosurface
   ./analytics/isosurface.cpp
   ./analytics/reader.cpp
   ./setting/settings.cpp
   ./putgetMeta/metaclient.cpp
   ${PROTO_SRCS} 
   ${GRPC_SRCS}
)

find_package(VTK COMPONENTS
    FiltersCore
    IOImage
    IOXML
    IOLegacy
)

target_include_directories(isosurface PUBLIC ${gray-scott}/analytics ${DSINCLUDE})
target_link_directories(isosurface PUBLIC ${DSLINKDIR})
target_link_libraries(isosurface MPI::MPI_C ${DSLINKLIB} ${VTK_LIBRARIES}
gRPC::grpc++_reflection protobuf::libprotobuf)
