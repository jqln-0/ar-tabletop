#include "gtest/gtest.h"

#include <vector>
#include <aruco/aruco.h>

#include "filter.h"

class BoardIgnoringMarkerFilterTest : public ::testing::Test
{
protected:
		void AddMarkerToConfig(int id) {
			aruco::MarkerInfo info;
			info.id = id;
			config_.push_back(info);
		}

		void AddMarkerToVector(int id) {
			aruco::Marker marker;
			marker.id = id;
			markers_.push_back(marker);
		}

		aruco::BoardConfiguration config_;
		std::vector<aruco::Marker> markers_;
};

TEST_F(BoardIgnoringMarkerFilterTest, RemovesCenterMarker) {
	AddMarkerToVector(0);
	AddMarkerToVector(1);
	AddMarkerToVector(2);

	AddMarkerToConfig(1);

	BoardIgnoringMarkerFilter filter(config_);
	filter.Filter(&markers_);

	ASSERT_EQ(2, markers_.size());
	EXPECT_EQ(0, markers_[0].id);
	EXPECT_EQ(2, markers_[1].id);
}

TEST_F(BoardIgnoringMarkerFilterTest, RemoveFirstMarker) {
	AddMarkerToVector(0);
	AddMarkerToVector(1);
	AddMarkerToVector(2);

	AddMarkerToConfig(0);

	BoardIgnoringMarkerFilter filter(config_);
	filter.Filter(&markers_);

	ASSERT_EQ(2, markers_.size());
	EXPECT_EQ(1, markers_[0].id);
	EXPECT_EQ(2, markers_[1].id);
}

TEST_F(BoardIgnoringMarkerFilterTest, RemovesLastMarker) {
	AddMarkerToVector(0);
	AddMarkerToVector(1);
	AddMarkerToVector(2);

	AddMarkerToConfig(2);

	BoardIgnoringMarkerFilter filter(config_);
	filter.Filter(&markers_);

	ASSERT_EQ(2, markers_.size());
	EXPECT_EQ(0, markers_[0].id);
	EXPECT_EQ(1, markers_[1].id);
}

TEST_F(BoardIgnoringMarkerFilterTest, NoMarkersToRemove) {
	AddMarkerToVector(0);
	AddMarkerToVector(1);
	AddMarkerToVector(2);

	AddMarkerToConfig(5);

	BoardIgnoringMarkerFilter filter(config_);
	filter.Filter(&markers_);

	ASSERT_EQ(3, markers_.size());
	EXPECT_EQ(0, markers_[0].id);
	EXPECT_EQ(1, markers_[1].id);
	EXPECT_EQ(2, markers_[2].id);
}

TEST_F(BoardIgnoringMarkerFilterTest, AllMarkersToRemove) {
	AddMarkerToVector(0);
	AddMarkerToVector(1);
	AddMarkerToVector(2);

	AddMarkerToConfig(0);
	AddMarkerToConfig(1);
	AddMarkerToConfig(2);

	BoardIgnoringMarkerFilter filter(config_);
	filter.Filter(&markers_);

	ASSERT_EQ(0, markers_.size());
}

TEST_F(BoardIgnoringMarkerFilterTest, NoMarkersGiven) {
	AddMarkerToConfig(0);
	AddMarkerToConfig(1);
	AddMarkerToConfig(2);

	BoardIgnoringMarkerFilter filter(config_);
	filter.Filter(&markers_);

	ASSERT_EQ(0, markers_.size());
}

TEST_F(BoardIgnoringMarkerFilterTest, IgnoresEmptyBoard) {
	AddMarkerToVector(0);
	AddMarkerToVector(1);
	AddMarkerToVector(2);

	BoardIgnoringMarkerFilter filter(config_);
	filter.Filter(&markers_);

	ASSERT_EQ(3, markers_.size());
	EXPECT_EQ(0, markers_[0].id);
	EXPECT_EQ(1, markers_[1].id);
	EXPECT_EQ(2, markers_[2].id);
}
