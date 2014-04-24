set(SOURCE
  mainwindow.cpp
  main.cpp
  framefetcher.cpp
  denoising.cpp
	matconv.cpp
  filter.cpp
)

set(HEADERS
  mainwindow.h
  framefetcher.h
  denoising.h
	matconv.h
  filter.h
)

set(FORMS
  gui/window.ui
  gui/filtering.ui
)

qt5_wrap_cpp(HEADERS_MOC ${HEADERS})
qt5_wrap_ui(FORMS_HEADERS ${FORMS})

add_executable( ARTabletop
  ${SOURCE}
  ${HEADERS_MOC}
  ${FORMS_HEADERS}
)

target_link_libraries( ARTabletop
  ${aruco_LIBS}
)

qt5_use_modules(ARTabletop Widgets)

