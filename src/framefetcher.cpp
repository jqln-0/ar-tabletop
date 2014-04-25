#include "framefetcher.h"

using cv::Mat;

// WebcamFrameFetcher
WebcamFrameFetcher::WebcamFrameFetcher(int device_index) {
  // Open the requested webcam device.
  // TODO: Error handling if the device doesn't exist/work.
  video_capturer_.open(device_index);

  // Grab the device so that we can start getting output from it.
  video_capturer_.grab();
}

WebcamFrameFetcher::~WebcamFrameFetcher() {
  // Only release on exit as we don't expect anyone else to be using our device.
  video_capturer_.release();
}

bool WebcamFrameFetcher::GetNextFrame(Mat *dest) {
  if (!HasNextFrame()) {
    return false;
  }
  video_capturer_ >> *dest;
  return true;
}

bool WebcamFrameFetcher::HasNextFrame() const {
  // cv::VideoCapture will close on EOF, so this will reliably work.
  return video_capturer_.isOpened();
}

// VideoFrameFetcher

VideoFrameFetcher::VideoFrameFetcher(const string &filename) {
  // Open the requested webcam device.
  // TODO: Error handling if the device doesn't exist/work.
  video_capturer_.open(filename);

  // Grab the device so that we can start getting output from it.
  video_capturer_.grab();
}

VideoFrameFetcher::~VideoFrameFetcher() {
  // Only release on exit as we don't expect anyone else to be using our device.
  video_capturer_.release();
}

bool VideoFrameFetcher::GetNextFrame(Mat *dest) {
  if (!HasNextFrame()) {
    return false;
  }
  video_capturer_ >> *dest;
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
  // Essentially this checks whether or not openning the image was successful.
  return image_.data == NULL;
}
