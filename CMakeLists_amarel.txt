cmake_minimum_required (VERSION 3.14)
project (gray-scott C CXX)
set (CMAKE_CXX_STANDARD 14)

# run the ./dspaces_config to get the data space compile flag and the link flag

set (DSINCLUDE /home/zw241/cworkspace/build/build_dataspaces/include)
set (DSLINKDIR /home/zw241/cworkspace/build/build_dataspaces/lib)
set (DSLINKLIB -ldspaces -ldscommon -ldart -lpthread -lm -lrt -libverbs -lrdmacm)


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
find_package (thallium REQUIRED)



add_library(metaclient ./putgetMeta/metaclient.cpp)
target_link_libraries(metaclient thallium)

add_executable(gray-scott
  ./simulation/main.cpp
  ./simulation/gray-scott.cpp
  ./simulation/writer.cpp
  ./setting/settings.cpp)
  
target_include_directories(gray-scott PUBLIC ${gray-scott}/simulation ${DSINCLUDE})
target_link_directories(gray-scott PUBLIC ${DSLINKDIR})
target_link_libraries(gray-scott MPI::MPI_C ${DSLINKLIB} metaclient)

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
target_link_libraries(isosurface MPI::MPI_C ${DSLINKLIB} ${VTK_LIBRARIES} metaclient)
