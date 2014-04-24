#include "filter.h"

// BoardIgnoringMarkerFilter

BoardIgnoringMarkerFilter::BoardIgnoringMarkerFilter(
    const aruco::BoardConfiguration& config)
    : config_(config) {}

void BoardIgnoringMarkerFilter::Filter(std::vector<aruco::Marker>> m) {
  // TODO.
}

// SmoothingMarkerFilter

SmoothingMarkerFilter::SmoothingMarkerFilter(int memory_length)
    : memory_length_(memory_length) {}

void SmoothingMarkerFilter::Filter(std::vector<aruco::Marker>> m) {
  // TODO.
}
