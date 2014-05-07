#include "framefetcher.h"

using cv::Mat;

VideoFrameFetcher::VideoFrameFetcher() {}

VideoFrameFetcher::VideoFrameFetcher(const string &filename) {
  // Open the requested video file.
  video_capturer_.open(filename);
}

VideoFrameFetcher::~VideoFrameFetcher() { video_capturer_.release(); }

cv::Mat VideoFrameFetcher::GetNextFrame() {
  cv::Mat frame;
  video_capturer_.read(frame);
  return frame;
}

bool VideoFrameFetcher::HasNextFrame() const {
  // Will detect both EOF and a fail to open the device.
  return video_capturer_.isOpened();
}

WebcamFrameFetcher::WebcamFrameFetcher(int device_index) {
  // Open the requested webcam device.
  video_capturer_.open(device_index);
}

WebcamFrameFetcher::~WebcamFrameFetcher() {}

PhotoFrameFetcher::PhotoFrameFetcher(const string &filename) {
  image_ = cv::imread(filename, cv::IMREAD_COLOR);
}

PhotoFrameFetcher::~PhotoFrameFetcher() {}

cv::Mat PhotoFrameFetcher::GetNextFrame() {
  // We need to explicitly copy the our image since cv::Mat will share image
  // data.
  return image_.clone();
}

bool PhotoFrameFetcher::HasNextFrame() const {
  // Essentially this checks whether or not opening the image was successful.
  return image_.data != NULL;
}
