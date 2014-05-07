#include "framefetcher.h"

using cv::Mat;

// WebcamFrameFetcher
WebcamFrameFetcher::WebcamFrameFetcher(int device_index) {
  // Open the requested webcam device.
  // TODO: Error handling if the device doesn't exist/work.
  video_capturer_.open(device_index);
}

WebcamFrameFetcher::~WebcamFrameFetcher() { video_capturer_.release(); }

bool WebcamFrameFetcher::GetNextFrame(Mat *dest) {
  if (!HasNextFrame()) {
    return false;
  }
  video_capturer_.read(*dest);
  return true;
}

bool WebcamFrameFetcher::HasNextFrame() const {
  // cv::VideoCapture will close on EOF, so this should reliably work.
  return video_capturer_.isOpened();
}

// VideoFrameFetcher

VideoFrameFetcher::VideoFrameFetcher(const string &filename) {
  // Open the requested webcam device.
  // TODO: Error handling if the device doesn't exist/work.
  video_capturer_.open(filename);
}

VideoFrameFetcher::~VideoFrameFetcher() { video_capturer_.release(); }

bool VideoFrameFetcher::GetNextFrame(Mat *dest) {
  if (!HasNextFrame()) {
    return false;
  }
  video_capturer_.read(*dest);
  return true;
}

bool VideoFrameFetcher::HasNextFrame() const {
  // cv::VideoCapture will close on EOF, so this will reliably work.
  return video_capturer_.isOpened();
}

// PhotoFrameFetcher

PhotoFrameFetcher::PhotoFrameFetcher(const string &filename) {
  image_ = cv::imread(filename, cv::IMREAD_COLOR);
}

PhotoFrameFetcher::~PhotoFrameFetcher() {}

bool PhotoFrameFetcher::GetNextFrame(Mat *dest) {
  if (!HasNextFrame()) {
    return false;
  }
  // Give the caller a copy of the stored image.
  *dest = image_;
  return true;
}

bool PhotoFrameFetcher::HasNextFrame() const {
  // Essentially this checks whether or not opening the image was successful.
  return image_.data != NULL;
}
