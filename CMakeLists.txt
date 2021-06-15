project(Ray-it)
cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops -Wall ")
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWIN32")
endif (WIN32)

set(FILES_TO_BE_COMPILED
    src/raytracer.cpp
    src/box.cpp
    src/kdTree.cpp
    src/mesh.cpp
    src/ray.cpp
    src/timer.cpp
    src/triangle.cpp
)

# add the executable
add_executable(Ray-it ${FILES_TO_BE_COMPILED})
