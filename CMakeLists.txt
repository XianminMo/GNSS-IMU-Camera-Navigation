cmake_minimum_required(VERSION 3.10)
project(gici)

# 设置C++标准和编译选项
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

# 设置关键依赖项的显式路径
set(OpenCV_DIR "/usr/local/lib/cmake/opencv4" CACHE PATH "OpenCV directory")
set(YAML_CPP_DIR "/usr/local/lib/cmake/yaml-cpp" CACHE PATH "YAML-CPP directory")
set(glog_DIR "/usr/local/lib/cmake/glog")  # 关键修复：显式指定 glog 的 CMake 配置路径
set(GLOG_ROOT "/usr/local")                # 确保 Ceres 能通过此变量找到 glog

# 设置 RPATH 以便运行时找到库
set(CMAKE_INSTALL_RPATH "/usr/local/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# 处理C++11 ABI兼容性问题
option(USE_CXX11_ABI "Use C++11 ABI" ON)
if(USE_CXX11_ABI)
    add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)
else()
    add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
endif()

# Release 编译优化选项
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  if (IS_ARM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon -march=armv7-a")
  else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmmx -msse -msse2 -msse3 -mssse3")
  endif ()
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -fsee -fomit-frame-pointer -fno-signed-zeros -fno-math-errno -funroll-loops")
endif()

# 查找 Eigen3（必需）
find_package(Eigen3 REQUIRED NO_MODULE)

# 查找 OpenCV（必需）
find_package(OpenCV 4.2.0 REQUIRED COMPONENTS core imgproc video videoio highgui)
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")

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

# 查找 Ceres（必需，依赖 glog）
find_package(Ceres REQUIRED)
message(STATUS "Ceres libraries: ${CERES_LIBRARIES}")

# 添加第三方库
add_subdirectory(third_party/fast)
add_subdirectory(third_party/rpg_vikit/vikit_common)
add_subdirectory(third_party/rpg_svo)

# RTKLIB 配置
add_definitions(-DENAGLO -DENACMP -DENAGAL -DENAQZS -DENAIRN -DNFREQ=3 -DNEXOBS=3 -DDLL)
add_subdirectory(third_party/rtklib)

# 设置包含目录
include_directories(
  ${EIGEN3_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${YAML_CPP_INCLUDE_DIRS}
  ${GLOG_INCLUDE_DIRS}
  ${CERES_INCLUDE_DIRS}
  include 
)

# 收集源文件
aux_source_directory(src/utility DIR_utility)
aux_source_directory(src/stream DIR_stream)
aux_source_directory(src/gnss DIR_gnss)
aux_source_directory(src/imu DIR_imu)
aux_source_directory(src/vision DIR_vision)
aux_source_directory(src/estimate DIR_estimate)
aux_source_directory(src/fusion DIR_fusion)
list(APPEND DIR_ALL 
  ${DIR_utility}
  ${DIR_stream}
  ${DIR_gnss}
  ${DIR_imu}
  ${DIR_vision}
  ${DIR_estimate}
  ${DIR_fusion}
)

# 创建共享库
add_library(${PROJECT_NAME} SHARED ${DIR_ALL})
target_link_libraries(${PROJECT_NAME} 
  PUBLIC
  rtklib
  vikit_common
  svo
  yaml-cpp
  glog::glog
  ${OpenCV_LIBS}
  Ceres::ceres
  fast
)
target_include_directories(${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# 创建可执行文件
add_executable(gici_main src/gici_main.cpp)
target_link_libraries(gici_main 
  ${PROJECT_NAME}
  yaml-cpp
  glog::glog
  Ceres::ceres
)
