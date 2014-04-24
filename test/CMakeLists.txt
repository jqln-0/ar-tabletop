message( STATUS ${CMAKE_SOURCE_DIR} )

# Include the main application so that the main library builds.
include_directories( ${CMAKE_SOURCE_DIR}/src )

# List of test files to include.
set( TESTS
  BoardIgnoringMarkerFilterTest.cpp
)

add_executable( AllTests
    AllTests.cpp
    ${TESTS}
)

target_link_libraries( AllTests
    gtest
    ART
)
