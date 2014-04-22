project( ProjectName )

add_definitions( -std=c++0x )

# Set BOOST_ROOT environment variable to point to your local Boost dir.
find_package( Boost REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

find_package( CGAL REQUIRED )
include( ${CGAL_USE_FILE} )

add_subdirectory( src )
add_subdirectory( test )
