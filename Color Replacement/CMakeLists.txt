cmake_minimum_required(VERSION 3.10)
project(main)

# Menggunakan standar C++ terbaru sesuai keinginan Anda (C++20 atau 23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Beritahu CMake lokasi OpenCV di MSYS2
set(CMAKE_PREFIX_PATH "C:\\msys64\\ucrt64\\lib\\opencv4")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(main main.cpp)

# Link library OpenCV ke executable
target_link_libraries(main ${OpenCV_LIBS})