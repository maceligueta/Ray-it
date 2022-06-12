project(Ray-it)
cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops -Wall ")
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWIN32")
endif (WIN32)

IF(RAY_IT_USE_FLOATS MATCHES ON)
    ADD_DEFINITIONS(-DRAY_IT_USE_FLOATS)
ENDIF(RAY_IT_USE_FLOATS MATCHES ON)

IF(RAY_IT_COMPILATION_TYPE MATCHES Debug)
    ADD_DEFINITIONS(-DRAY_IT_DEBUG)
ENDIF()

set(FILES_TO_BE_COMPILED
    src/ray-it.cpp
    src/box.cpp
    src/kdTree.cpp
    src/mesh.cpp
    src/ray.cpp
    src/triangle.cpp
    src/wave.cpp
    src/tests_manager.cpp
    src/computation.cpp
    src/constants.cpp
    src/antenna.cpp
    src/jones.cpp
    tests/test.cpp
    external_libraries/timer.cpp
)

# add the executable
add_executable(Ray-it ${FILES_TO_BE_COMPILED})
