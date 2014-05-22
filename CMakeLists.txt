# Project information.
cmake_minimum_required(VERSION 2.8.8)
project( ARTabletop )

# Compile flags.
add_definitions( -std=c++11 )

# Boost.
find_package( Boost REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

# Qt.
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package( Qt5Widgets REQUIRED )

# ArUco.
SET( CMAKE_MODULE_PATH /lib/cmake/ )
find_package( aruco REQUIRED )

# OpenGL and GLUT.
find_package(OpenGL)
find_package(GLUT)
set(OPENGL_LIBS ${OPENGL_gl_LIBRARY} ${GLUT_glub_LIBRARY})

# assimp
find_package(assimp)

# Build application.
add_subdirectory( src )

# Build tests.
add_subdirectory( test )
