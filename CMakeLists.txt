cmake_minimum_required(VERSION 2.8.3)
project(crrwm)

find_package(catkin REQUIRED COMPONENTS
  roscpp
)

# MATLAB runtime support
set(MATLAB_VER "v92") # Required version for R2017a
include_directories("/usr/local/MATLAB/MATLAB_Runtime/${MATLAB_VER}/extern/include")
link_directories("lib"
  "/usr/local/MATLAB/MATLAB_Runtime/${MATLAB_VER}/runtime/glnxa64"
  "/usr/local/MATLAB/MATLAB_Runtime/${MATLAB_VER}/bin/glnxa64"
  "/usr/local/MATLAB/MATLAB_Runtime/${MATLAB_VER}/sys/os/glnxa64")
set(MATLAB_LIBRARIES "mwmclmcrrt")


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
message("CMAEK MPD APTH ${CMAKE_MODULE_PATH}")

catkin_package()

include_directories(
  include
  ${catkin_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 14)

add_library(crrwm src/crrwm.cpp)
target_link_libraries(crrwm ${catkin_LIBRARIES} ${MATLAB_LIBRARIES})
add_dependencies(crrwm ${catkin_EXPORTED_TARGETS})

install(TARGETS crrwm
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
