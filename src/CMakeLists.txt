# Source files for the application.
set( SOURCES
  calibration.cpp
  denoising.cpp
  filter.cpp
  filterdialog.cpp
  framefetcher.cpp
  genmarkerdialog.cpp
  graphics.cpp
  mainwindow.cpp
  pipeline.cpp
)

# Header files for the application.
set( HEADERS
  calibration.h
  denoising.h
  filter.h
  filterdialog.h
  framefetcher.h
  genmarkerdialog.h
  graphics.h
  mainwindow.h
  matconv.h
  pipeline.h
)

# Qt *.ui files
set( FORMS
  gui/calibrate.ui
  gui/filtering.ui
  gui/genboard.ui
  gui/genmarker.ui
  gui/window.ui
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
	${OPENGL_LIBS}
)

qt5_use_modules(ART Core Gui OpenGL Widgets)

# Compile the application executable.
add_executable( ARTabletop
  main.cpp
)

target_link_libraries( ARTabletop
  ART
)
qt5_use_modules(ARTabletop Core Gui OpenGL Widgets)

