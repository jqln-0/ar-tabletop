# Source files for the application.
set( SOURCES
  mainwindow.cpp
  framefetcher.cpp
  denoising.cpp
	matconv.cpp
)

# Header files for the application.
set( HEADERS
  mainwindow.h
  framefetcher.h
  denoising.h
	matconv.h
)

# Qt *.ui files
set( FORMS
  gui/window.ui
  gui/filtering.ui
)
qt5_wrap_ui(FORMS_HEADERS ${FORMS})

# Compile the bulk of the application as a library for easier linking with the
# testing framework.
add_library( ART
  ${SOURCE}
  ${HEADERS_MOC}
  ${FORMS_HEADERS}
)

qt5_use_modules(ART Widgets)

# Compile the application executable.
add_executable( ARTabletop
  main.cpp
)

target_link_libraries( ARTabletop
  ART
  ${aruco_LIBS}
)

qt5_use_modules(ARTabletop Widgets)

