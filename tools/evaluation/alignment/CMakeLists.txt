cmake_minimum_required(VERSION 3.10)
project(alignment)

# Set build flags. 
set(CMAKE_CXX_FLAGS "-std=c++11" )
set(CMAKE_CXX_FLAGS "-fpermissive")
set(CMAKE_C_FLAGS "-std=c99")

set(YAML_CPP_DIR "/usr/local/lib/cmake/yaml-cpp" CACHE PATH "YAML-CPP directory")
set(glog_DIR "/usr/local/lib/cmake/glog")

# 查找 yaml-cpp（必需）
find_package(yaml-cpp REQUIRED)
message(STATUS "yaml-cpp libraries: ${YAML_CPP_LIBRARIES}")

# 查找 glog
find_package(glog REQUIRED CONFIG)  # 强制使用CONFIG模式
if(TARGET glog::glog)
    message(STATUS "Found glog via target")
    get_target_property(GLOG_INCLUDE_DIRS glog::glog INTERFACE_INCLUDE_DIRECTORIES)
    set(GLOG_LIBRARIES glog::glog)  # 统一使用目标形式
else()
    message(FATAL_ERROR "glog::glog target not found")
endif()
message(STATUS "Glog includes: ${GLOG_INCLUDE_DIRS}")

add_subdirectory(../../.. gici.out)

include_directories(
  ${EIGEN3_INCLUDE_DIRS}
  include
)

# Add definitions here to ensure correct memory allocation in RTKLIB
add_definitions(-DENAGLO -DENACMP -DENAGAL -DENAQZS -DENAIRN -DNFREQ=3 -DNEXOBS=3 -DDLL)

# Executables
add_executable(nmea_align_timestamp src/nmea_align_timestamp.cpp
                                    src/nmea_formator.cpp)
target_link_libraries(nmea_align_timestamp
  PRIVATE
  gici
  yaml-cpp
  glog::glog
)

add_executable(nmea_estimate_pose_extrinsics src/nmea_estimate_pose_extrinsics.cpp
                                    src/nmea_formator.cpp)
target_link_libraries(nmea_estimate_pose_extrinsics
  PRIVATE
  gici
  yaml-cpp
  glog::glog
)

add_executable(nmea_pose_to_position src/nmea_pose_to_position.cpp 
                                     src/nmea_formator.cpp)
target_link_libraries(nmea_pose_to_position
  PRIVATE
  gici
  yaml-cpp
  glog::glog
)

add_executable(nmea_pose_to_pose src/nmea_pose_to_pose.cpp 
                      src/nmea_formator.cpp)
target_link_libraries(nmea_pose_to_pose
  PRIVATE
  gici
  yaml-cpp
  glog::glog
)
