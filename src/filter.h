#ifndef __FILTER_H_
#define __FILTER_H_

#include <aruco/aruco.h>

#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class MarkerFilter {
 public:
  virtual ~MarkerFilter() {}

  virtual void Filter(std::vector<aruco::Marker> *m) = 0;
};

class BoardIgnoringMarkerFilter : public MarkerFilter {
 protected:
  std::unordered_set<int> board_markers_;

 public:
  BoardIgnoringMarkerFilter(const aruco::BoardConfiguration &config);

  virtual void Filter(std::vector<aruco::Marker> *m);
};

class SmoothingMarkerFilter : public MarkerFilter {
 protected:
  const int memory_length_;
  std::unordered_map<int, int> last_seen_;
  std::unordered_map<int, aruco::Marker> markers_;

 public:
  SmoothingMarkerFilter(const int memory_length = 2);

  virtual void Filter(std::vector<aruco::Marker> *m);
};

#endif  // __FILTER_H_
