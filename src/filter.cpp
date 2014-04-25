#include "filter.h"

using std::vector;
using aruco::Marker;
using std::unordered_map;

// BoardIgnoringMarkerFilter

BoardIgnoringMarkerFilter::BoardIgnoringMarkerFilter(
    const aruco::BoardConfiguration &config) {
  // Create a hashset from the board markers for faster lookup later.
  vector<int> ids;
  config.getIdList(ids, false);
  for (vector<int>::iterator it = ids.begin(); it != ids.end(); ++it) {
    board_markers_.insert(*it);
  }
}

void BoardIgnoringMarkerFilter::Filter(vector<Marker> *m) {
  // Iterate backwards so we can remove ids as we go.
  for (vector<Marker>::reverse_iterator it = m->rbegin(); it != m->rend();
       ++it) {
    if (board_markers_.find(it->id) != board_markers_.end()) {
      m->erase(--(it.base()));
    }
  }
}

// SmoothingMarkerFilter

SmoothingMarkerFilter::SmoothingMarkerFilter(int memory_length)
    : memory_length_(memory_length) {}

void SmoothingMarkerFilter::Filter(vector<Marker> *m) {
  // Increment the last seen values and forget old markers.
  vector<int> to_delete;
  for (unordered_map<int, int>::iterator it = last_seen_.begin();
       it != last_seen_.end(); ++it) {
    last_seen_[it->first] += 1;
    if (last_seen_[it->first] > memory_length_) {
      to_delete.push_back(it->first);
    }
  }

  // Do the actual forgetting.
  for (vector<int>::iterator it = to_delete.begin(); it != to_delete.end();
       ++it) {
    last_seen_.erase(last_seen_.find(*it));
    markers_.erase(markers_.find(*it));
  }

  // Record markers from the current iteration.
  for (vector<Marker>::iterator it = m->begin(); it != m->end(); ++it) {
    last_seen_[it->id] = 0;
    markers_[it->id] = *it;
  }

  // Any markers with last_seen != 0 are markers which are not in this iteration
  // that we have previous seen and remembered. Add them to the list of markers.
  for (unordered_map<int, int>::iterator it = last_seen_.begin();
       it != last_seen_.end(); ++it) {
    if (it->second != 0) {
      m->push_back(markers_[it->first]);
    }
  }
}
