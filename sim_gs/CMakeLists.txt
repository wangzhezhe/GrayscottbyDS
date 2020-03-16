
find_package(ADIOS2 REQUIRED)

add_executable(gray-scott-adios
  main.cpp
  gray-scott.cpp
  settings.cpp
  writer.cpp)

target_link_libraries(gray-scott-adios adios2::adios2 MPI::MPI_C)


add_executable(isosurface-adios
  isosurface.cpp
  settings.cpp)

target_link_libraries(isosurface-adios adios2::adios2 MPI::MPI_C)