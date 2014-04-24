#include "filter.h"

// BoardIgnoringMarkerFilter

BoardIgnoringMarkerFilter::BoardIgnoringMarkerFilter(
    const aruco::BoardConfiguration& config) {
	// Create a hashset from the board markers for faster lookup later.
	std::vector<int> ids;
	config.getIdList(ids, false);
	for (std::vector<int>::iterator it=ids.begin(); it != ids.end(); ++it) {
		board_markers_.insert(*it);
	}
}

void BoardIgnoringMarkerFilter::Filter(std::vector<aruco::Marker> *m) {
	// Iterate backwards so we can remove ids as we go.
	for (std::vector<aruco::Marker>::reverse_iterator it = m->rbegin(); it != m->rend(); ++it) {
		if (board_markers_.find(it->id) != board_markers_.end()) {
			m->erase( --(it.base()) );
		}
	}
}

// SmoothingMarkerFilter

SmoothingMarkerFilter::SmoothingMarkerFilter(int memory_length)
    : memory_length_(memory_length) {}

void SmoothingMarkerFilter::Filter(std::vector<aruco::Marker> *m) {
	// TODO.
}
