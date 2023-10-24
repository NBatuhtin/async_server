cmake_minimum_required(VERSION 2.6)


project(async_server)

set(CMAKE_CXX_STANDART 11)

set(BOOST_ROOT /boost_1_83_0)

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED)

if(Boost_FOUND)
	include_directories (${Boost_INCLUDE_DIR})
	add_executable(Server src/async_server.cpp)
	add_executable(Client src/client.cpp)
	target_link_libraries (Client ${Boost_LIBRARIES})
	target_link_libraries (Client ${Boost_LIBRARIES})
endif()