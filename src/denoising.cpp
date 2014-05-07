#include "denoising.h"

using std::shared_ptr;
using cv::Mat;

DenoisingFrameFetcher::DenoisingFrameFetcher() : wrapped_(nullptr) {}

DenoisingFrameFetcher::DenoisingFrameFetcher(FrameFetcher *w) : wrapped_(w) {}

DenoisingFrameFetcher::~DenoisingFrameFetcher() {}

bool DenoisingFrameFetcher::HasNextFrame() const {
  return wrapped_->HasNextFrame();
}

shared_ptr<FrameFetcher> DenoisingFrameFetcher::wrapped() const {
  return wrapped_;
}

void DenoisingFrameFetcher::set_wrapped(shared_ptr<FrameFetcher> fetcher) {
  wrapped_ = fetcher;
}

GaussianDenoisingFrameFetcher::GaussianDenoisingFrameFetcher(
    FrameFetcher *w, const cv::Size &kernel, const double sigma)
    : DenoisingFrameFetcher(w), kernel_(kernel), sigma_(sigma) {}

GaussianDenoisingFrameFetcher::GaussianDenoisingFrameFetcher(FrameFetcher *w,
                                                             const int kernel,
                                                             const double sigma)
    : DenoisingFrameFetcher(w), kernel_(kernel, kernel), sigma_(sigma) {}

GaussianDenoisingFrameFetcher::~GaussianDenoisingFrameFetcher() {}

Mat GaussianDenoisingFrameFetcher::GetNextFrame() {
  // Fetch the next frame from the wrapped fetcher.
  Mat src = wrapped_->GetNextFrame();

  // Blur the frame in-place.
  cv::GaussianBlur(src, src, kernel_, sigma_, sigma_);

  return src;
}

MedianDenoisingFrameFetcher::MedianDenoisingFrameFetcher(FrameFetcher *w,
                                                         const int size)
    : DenoisingFrameFetcher(w), size_(size) {}

MedianDenoisingFrameFetcher::~MedianDenoisingFrameFetcher() {}

Mat MedianDenoisingFrameFetcher::GetNextFrame() {
  // Fetch the next frame from the wrapped fetcher.
  Mat src = wrapped_->GetNextFrame();

  // Blur the frame in-place.
  cv::medianBlur(src, src, size_);

  return src;
}

BilateralDenoisingFrameFetcher::BilateralDenoisingFrameFetcher(
    FrameFetcher *w, const int d, const double sigma)
    : DenoisingFrameFetcher(w), d_(d), sigma_(sigma) {}

BilateralDenoisingFrameFetcher::~BilateralDenoisingFrameFetcher() {}

Mat BilateralDenoisingFrameFetcher::GetNextFrame() {
  // The bilateral filter doesn't work in-place, so we need to make a second
  // mat.
  Mat src = wrapped_->GetNextFrame();
  Mat dest;

  // Run the filter.
  cv::bilateralFilter(src, dest, d_, sigma_, sigma_);

  return dest;
}

NonLocalMeansDenoisingFrameFetcher::NonLocalMeansDenoisingFrameFetcher(
    FrameFetcher *w)
    : DenoisingFrameFetcher(w) {}

NonLocalMeansDenoisingFrameFetcher::~NonLocalMeansDenoisingFrameFetcher() {}

Mat NonLocalMeansDenoisingFrameFetcher::GetNextFrame() {
  // TODO: Check if this filter will work in-place.
  Mat src = wrapped_->GetNextFrame();
  Mat dest;
  cv::fastNlMeansDenoisingColored(src, dest);

  return dest;
}
