cmake_minimum_required(VERSION 2.8)
project( ARTabletop )

add_definitions( -std=c++11 )

find_package( Boost REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

#set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package( Qt5Widgets REQUIRED )

SET( CMAKE_MODULE_PATH /lib/cmake/ )
find_package( aruco REQUIRED )

add_subdirectory( src )
add_subdirectory( test )
