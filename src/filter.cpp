#ifndef __FILTER_H_
#define __FILTER_H_

#include <vector>
#include <utility>
#include <aruco/aruco.h>

class MarkerFilter {
 public:
  virtual ~MarkerFilter() {}

  virtual void Filter(std::vector<aruco::Marker>* m) = 0;
};

class BoardIgnoringMarkerFilter : public MarkerFilter {
 protected:
  const aruco::BoardConfiguration& config_;

 public:
  BoardIgnoringMarkerFilter(const aruco::BoardConfiguration& config);

  virtual void Filter(std::vector<aruco::Marker> m);
};

class SmoothingMarkerFilter : public MarkerFilter {
 protected:
  const int memory_length_;
  std::vector<std::pair<int, int>> markers_;

 public:
  SmoothingMarkerFilter(int memory_length = 2);

  virtual void Filter(std::vector<aruco::Marker> m);
};

#endif  // __FILTER_H_
