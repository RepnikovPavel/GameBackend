cmake_minimum_required(VERSION 3.24.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)
set(PROJECT_NAME AppsConnectionWithBoost)
project(${PROJECT_NAME})

set(SERVER_EXECUTABLE_NAME server_executable_program)
set(CLIENT_EXECUTABLE_NAME client_executable_program)

include(C:/CmakeProjects/sockets_boost/build/conanbuildinfo.cmake)
conan_basic_setup()

file (GLOB SERVER_APP_FILES
	server_source/http_session.cpp
	server_source/http_session.h
	server_source/Listener.cpp
	server_source/Listener.h
	server_source/main.cpp
	server_source/SharedState.cpp
	server_source/SharedState.h
	server_source/websocket_session.cpp
	server_source/websocket_session.h
	server_source/PlotLib.cpp
	server_source/PlotLib.h
)

add_executable(${SERVER_EXECUTABLE_NAME} ${SERVER_APP_FILES})
target_link_libraries(${SERVER_EXECUTABLE_NAME} ${CONAN_LIBS})


add_executable(${CLIENT_EXECUTABLE_NAME} client_source/main.cpp)
target_link_libraries(${CLIENT_EXECUTABLE_NAME} ${CONAN_LIBS})


