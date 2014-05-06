# Source files for the application.
set( SOURCES
  mainwindow.cpp
  filterdialog.cpp
  framefetcher.cpp
  denoising.cpp
	matconv.cpp
	filter.cpp
)

# Header files for the application.
set( HEADERS
  mainwindow.h
  filterdialog.h
  framefetcher.h
  denoising.h
	matconv.h
	filter.h
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
  ${SOURCES}
  ${HEADERS}
  ${FORMS_HEADERS}
)

target_link_libraries( ART
  ${aruco_LIBS}
)

qt5_use_modules(ART Widgets)

# Compile the application executable.
add_executable( ARTabletop
  main.cpp
)

target_link_libraries( ARTabletop
  ART
)

qt5_use_modules(ARTabletop Widgets)

