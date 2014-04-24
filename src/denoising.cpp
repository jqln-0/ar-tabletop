#include "denoising.h"

// DenoisingFrameFetcher

DenoisingFrameFetcher::DenoisingFrameFetcher(FrameFetcher *w) : wrapped_(w) {}

DenoisingFrameFetcher::~DenoisingFrameFetcher() {}

bool DenoisingFrameFetcher::has_next_frame() {
  return wrapped_->has_next_frame();
}

// GaussianDenoisingFrameFetcher

GaussianDenoisingFrameFetcher::GaussianDenoisingFrameFetcher(FrameFetcher *w,
                                                             cv::Size kernel,
                                                             double sigma)
    : DenoisingFrameFetcher(w), kernel_(kernel), sigma_(sigma) {}

GaussianDenoisingFrameFetcher::~GaussianDenoisingFrameFetcher() {}

std::unique_ptr<cv::Mat> GaussianDenoisingFrameFetcher::next_frame() {
  // Fetch the next frame from the wrapped fetcher.
  std::unique_ptr<cv::Mat> frame = wrapped_->next_frame();

  // Blur the frame in-place.
  cv::GaussianBlur(*frame, *frame, kernel_, sigma_, sigma_);

  return frame;
}

// MedianDenoisingFrameFetcher

MedianDenoisingFrameFetcher::MedianDenoisingFrameFetcher(FrameFetcher *w,
                                                         int size)
    : DenoisingFrameFetcher(w), size_(size) {}

MedianDenoisingFrameFetcher::~MedianDenoisingFrameFetcher() {}

std::unique_ptr<cv::Mat> MedianDenoisingFrameFetcher::next_frame() {
  // Fetch the next frame from the wrapped fetcher.
  std::unique_ptr<cv::Mat> frame = wrapped_->next_frame();

  // Blur the frame in-place.
  cv::medianBlur(*frame, *frame, size_);

  return frame;
}

// BilateralDenoisingFrameFetcher

BilateralDenoisingFrameFetcher::BilateralDenoisingFrameFetcher(FrameFetcher *w,
                                                               int d,
                                                               double sigma)
    : DenoisingFrameFetcher(w), d_(d), sigma_(sigma) {}

BilateralDenoisingFrameFetcher::~BilateralDenoisingFrameFetcher() {}

std::unique_ptr<cv::Mat> BilateralDenoisingFrameFetcher::next_frame() {
  // Fetch the next frame from the wrapped fetcher.
  std::unique_ptr<cv::Mat> frame = wrapped_->next_frame();

  // The bilateral filter doesn't work in-place, so we need a copy.
  cv::Mat original(*frame);
  cv::bilateralFilter(original, *frame, d_, sigma_, sigma_);

  return frame;
}

// NonLocalMeansDenoisingFrameFetcher

NonLocalMeansDenoisingFrameFetcher::NonLocalMeansDenoisingFrameFetcher(
    FrameFetcher *w)
    : DenoisingFrameFetcher(w) {}

NonLocalMeansDenoisingFrameFetcher::~NonLocalMeansDenoisingFrameFetcher() {}

std::unique_ptr<cv::Mat> NonLocalMeansDenoisingFrameFetcher::next_frame() {
  // Fetch the next frame from the wrapped fetcher.
  std::unique_ptr<cv::Mat> frame = wrapped_->next_frame();

  // TODO: Check if we can actually do this one in-place.
  cv::Mat original(*frame);
  cv::fastNlMeansDenoisingColored(original, *frame);

  return frame;
}
