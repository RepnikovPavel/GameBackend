cmake_minimum_required(VERSION 3.24.0 FATAL_ERROR)

set(PROJECT_NAME TestProject)
set(EXECUTABLE_NAME executable_program)
set(CMAKE_CXX_STANDARD 20)
project(${PROJECT_NAME})


include(C:/CmakeProjects/sockets_pr/build/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(${EXECUTABLE_NAME} source/main.cpp)
target_link_libraries(${EXECUTABLE_NAME} ${CONAN_LIBS})
