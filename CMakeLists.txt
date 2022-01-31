cmake_minimum_required(VERSION 3.1.2)
project(detect)

find_package(OpenCV REQUIRED)
include_directories("/usr/local/include/opencv4/")

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_EXE_LINKER_FLAGS "-static")

file(GLOB SRC_FILES *.cpp)
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${OpenCV_LIBS})




