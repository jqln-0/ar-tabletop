#ifndef __FILTER_H_
#define __FILTER_H_

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <utility>
#include <aruco/aruco.h>

class MarkerFilter {
 public:
  virtual ~MarkerFilter() {}

  virtual void Filter(std::vector<aruco::Marker> *m) = 0;
};

class BoardIgnoringMarkerFilter : public MarkerFilter {
 protected:
	std::unordered_set<int> board_markers_;

 public:
  BoardIgnoringMarkerFilter(const aruco::BoardConfiguration& config);

  virtual void Filter(std::vector<aruco::Marker> *m);
};

class SmoothingMarkerFilter : public MarkerFilter {
 protected:
  const int memory_length_;
	std::unordered_map<int, pair<aruco::Marker, int>> markers_;

 public:
  SmoothingMarkerFilter(int memory_length = 2);

  virtual void Filter(std::vector<aruco::Marker> *m);
};

#endif  // __FILTER_H_
