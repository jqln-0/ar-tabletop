#include "framefetcher.h"

// WebcamFrameFetcher

WebcamFrameFetcher::WebcamFrameFetcher(int device_index) {
  // Open the requested webcam device.
  // TODO: Error handling if the device doesn't exist/work.
  video_capturer_ = new cv::VideoCapture;
  video_capturer_->open(device_index);

  // Grab the device so that we can start getting output from it.
  video_capturer_->grab();
}

WebcamFrameFetcher::~WebcamFrameFetcher() {
  // Only release on exit as we don't expect anyone else to be using our device.
  video_capturer_->release();
  delete video_capturer_;
}

std::unique_ptr<cv::Mat> WebcamFrameFetcher::next_frame() {
  // Fetch the next frame from the capturer. Use a unique_ptr as we don't want
  // to keep ownership of every frame we generate.
  unique_ptr<cv::Mat> frame(new cv::Mat);
  *video_capturer_ >> *frame;
  return frame;
}

bool WebcamFrameFetcher::has_next_frame() {
  // cv::VideoCapture will close on EOF, so this will reliably work.
  return video_capturer_->isOpened();
}

// VideoFrameFetcher

VideoFrameFetcher::VideoFrameFetcher(const string& filename) {
  // Open the requested webcam device.
  // TODO: Error handling if the device doesn't exist/work.
  video_capturer_ = new cv::VideoCapture;
  video_capturer_->open(filename);

  // Grab the device so that we can start getting output from it.
  video_capturer_->grab();
}

VideoFrameFetcher::~VideoFrameFetcher() {
  // Only release on exit as we don't expect anyone else to be using our device.
  video_capturer_->release();
  delete video_capturer_;
}

std::unique_ptr<cv::Mat> VideoFrameFetcher::next_frame() {
  // Fetch the next frame from the capturer. Use a unique_ptr as we don't want
  // to keep ownership of every frame we generate.
  unique_ptr<cv::Mat> frame(new cv::Mat);
  *video_capturer_ >> *frame;
  return frame;
}

bool VideoFrameFetcher::has_next_frame() {
  // cv::VideoCapture will close on EOF, so this will reliably work.
  return video_capturer_->isOpened();
}

// PhotoFrameFetcher

PhotoFrameFetcher::PhotoFrameFetcher(const string& filename) {
  image_ = new cv::Mat;
  *image_ = cv::imread(filename, cv::IMREAD_COLOR);
}

PhotoFrameFetcher::~PhotoFrameFetcher() { delete image_; }

std::unique_ptr<cv::Mat> PhotoFrameFetcher::next_frame() {
  // Give the caller a copy of the stored image.
  cv::Mat* frame = new cv::Mat(*image_);
  return unique_ptr<cv::Mat>(frame);
}

bool PhotoFrameFetcher::has_next_frame() {
  // Essentially this checks whether or not openning the image was successful.
  return image_->data == NULL;
}
