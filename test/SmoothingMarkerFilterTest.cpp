#include "gtest/gtest.h"

#include <vector>
#include <aruco/aruco.h>

#include "filter.h"

class SmoothingMarkerFilterTest : public ::testing::Test {
 protected:
  void AddMarkerToVector(int id) {
    aruco::Marker marker;
    marker.id = id;
    markers_.push_back(marker);
  }

  std::vector<aruco::Marker> markers_;
};

TEST_F(SmoothingMarkerFilterTest, NoMissingMarkers) {
  SmoothingMarkerFilter filter(1);

  AddMarkerToVector(0);

  // First pass leaves markers untouched.
  filter.Filter(&markers_);
  ASSERT_EQ(1, markers_.size());
  ASSERT_EQ(0, markers_[0].id);

  // Second pass leaves markers untouched.
  filter.Filter(&markers_);
  ASSERT_EQ(1, markers_.size());
  ASSERT_EQ(0, markers_[0].id);
}

TEST_F(SmoothingMarkerFilterTest, RemembersMarker) {
  SmoothingMarkerFilter filter(1);

  AddMarkerToVector(0);
  filter.Filter(&markers_);

  // First filter should not touch the markers.
  ASSERT_EQ(1, markers_.size());
  ASSERT_EQ(0, markers_[0].id);

  markers_.clear();
  ASSERT_EQ(0, markers_.size());
  filter.Filter(&markers_);

  // Second filter should add the missing marker.
  ASSERT_EQ(1, markers_.size());
  ASSERT_EQ(0, markers_[0].id);
}

TEST_F(SmoothingMarkerFilterTest, ForgetsMarker) {
  SmoothingMarkerFilter filter(1);

  AddMarkerToVector(0);
  filter.Filter(&markers_);

  // First filter should not touch the markers.
  ASSERT_EQ(1, markers_.size());
  ASSERT_EQ(0, markers_[0].id);

  markers_.clear();
  ASSERT_EQ(0, markers_.size());
  filter.Filter(&markers_);

  markers_.clear();
  ASSERT_EQ(0, markers_.size());
  filter.Filter(&markers_);

  // Third filter should have forgotten the marker.
  ASSERT_EQ(0, markers_.size());
}
