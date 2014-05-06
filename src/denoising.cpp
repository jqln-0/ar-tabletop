#include "denoising.h"

using cv::Mat;

// DenoisingFrameFetcher

DenoisingFrameFetcher::DenoisingFrameFetcher() : wrapped_(nullptr) {}

DenoisingFrameFetcher::DenoisingFrameFetcher(FrameFetcher *w) : wrapped_(w) {}

DenoisingFrameFetcher::~DenoisingFrameFetcher() {}

bool DenoisingFrameFetcher::HasNextFrame() const {
  return wrapped_->HasNextFrame();
}

// GaussianDenoisingFrameFetcher

GaussianDenoisingFrameFetcher::GaussianDenoisingFrameFetcher(
    FrameFetcher *w, const cv::Size &kernel, const double sigma)
    : DenoisingFrameFetcher(w), kernel_(kernel), sigma_(sigma) {}

GaussianDenoisingFrameFetcher::GaussianDenoisingFrameFetcher(FrameFetcher *w,
                                                             const int kernel,
                                                             const double sigma)
    : DenoisingFrameFetcher(w), kernel_(kernel, kernel), sigma_(sigma) {}

GaussianDenoisingFrameFetcher::~GaussianDenoisingFrameFetcher() {}

bool GaussianDenoisingFrameFetcher::GetNextFrame(Mat *dest) {
  if (!HasNextFrame()) {
    return false;
  }

  // Fetch the next frame from the wrapped fetcher.
  wrapped_->GetNextFrame(dest);

  // Blur the frame in-place.
  cv::GaussianBlur(*dest, *dest, kernel_, sigma_, sigma_);

  return true;
}

// MedianDenoisingFrameFetcher

MedianDenoisingFrameFetcher::MedianDenoisingFrameFetcher(FrameFetcher *w,
                                                         const int size)
    : DenoisingFrameFetcher(w), size_(size) {}

MedianDenoisingFrameFetcher::~MedianDenoisingFrameFetcher() {}

bool MedianDenoisingFrameFetcher::GetNextFrame(Mat *dest) {
  if (!HasNextFrame()) {
    return false;
  }
  // Fetch the next frame from the wrapped fetcher.
  wrapped_->GetNextFrame(dest);

  // Blur the frame in-place.
  cv::medianBlur(*dest, *dest, size_);

  return true;
}

// BilateralDenoisingFrameFetcher

BilateralDenoisingFrameFetcher::BilateralDenoisingFrameFetcher(
    FrameFetcher *w, const int d, const double sigma)
    : DenoisingFrameFetcher(w), d_(d), sigma_(sigma) {}

BilateralDenoisingFrameFetcher::~BilateralDenoisingFrameFetcher() {}

bool BilateralDenoisingFrameFetcher::GetNextFrame(Mat *dest) {
  if (!HasNextFrame()) {
    return false;
  }

  // The bilateral filter doesn't work in-place, so fetch the next frame
  // locally.
  Mat src;
  wrapped_->GetNextFrame(&src);

  // Run the filter.
  cv::bilateralFilter(src, *dest, d_, sigma_, sigma_);

  return true;
}

// NonLocalMeansDenoisingFrameFetcher

NonLocalMeansDenoisingFrameFetcher::NonLocalMeansDenoisingFrameFetcher(
    FrameFetcher *w)
    : DenoisingFrameFetcher(w) {}

NonLocalMeansDenoisingFrameFetcher::~NonLocalMeansDenoisingFrameFetcher() {}

bool NonLocalMeansDenoisingFrameFetcher::GetNextFrame(Mat *dest) {
  if (!HasNextFrame()) {
    return false;
  }

  // TODO: Check if this filter works in-place.
  Mat src;
  wrapped_->GetNextFrame(&src);

  cv::fastNlMeansDenoisingColored(src, *dest);

  return true;
}
