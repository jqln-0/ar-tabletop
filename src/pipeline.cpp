#include "pipeline.h"

Pipeline::Pipeline()
    : source_fetcher_(nullptr),
      wrapping_fetcher_(nullptr),
      frame_(0, 0, 0),
      detector_(new aruco::MarkerDetector) {}

Pipeline::~Pipeline() {
  delete source_fetcher_;
  delete wrapping_fetcher_;
  delete detector_;
}

FrameFetcher *Pipeline::source_fetcher() { return source_fetcher_; }
void Pipeline::set_source_fetcher(FrameFetcher *f) {
  delete source_fetcher_;
  source_fetcher_ = f;
  if (wrapping_fetcher_ != nullptr) {
    wrapping_fetcher_->set_wrapped(source_fetcher_);
  }
}
DenoisingFrameFetcher *Pipeline::wrapping_fetcher() {
  return wrapping_fetcher_;
}

void Pipeline::set_wrapping_fetcher(DenoisingFrameFetcher *f) {
  delete wrapping_fetcher_;
  wrapping_fetcher_ = f;
  wrapping_fetcher_->set_wrapped(source_fetcher_);
}

std::vector<aruco::Marker> *Pipeline::markers() { return &markers_; }

bool Pipeline::GetFrame(QImage *dest) {
  if (frame_.size() == cv::Size(0, 0)) {
    return false;
  }
  *dest = MatToQImage(frame_);
  return true;
}

bool Pipeline::GetThresholdedFrame(QImage *dest) {
  if (frame_.size() == cv::Size(0, 0)) {
    return false;
  }

  *dest = MatToQImage(detector_->getThresholdedImage());
  return true;
}

void Pipeline::DrawMarkers() {
  for (auto it = markers_.begin(); it != markers_.end(); ++it) {
    it->draw(frame_, cv::Scalar(255, 0, 0), 1, true);
  }
}

void Pipeline::ProcessFrame() {
  // First fetch an image from whichever fetcher is available.
  if (source_fetcher_ == nullptr || !source_fetcher_->HasNextFrame()) {
    return;
  }

  if (wrapping_fetcher_ == nullptr || !wrapping_fetcher_->HasNextFrame()) {
    source_fetcher_->GetNextFrame(&frame_);
  } else {
    wrapping_fetcher_->GetNextFrame(&frame_);
  }

  // Next run our detector and filters.
  detector_->detect(frame_, markers_);
  for (auto it = filters_.begin(); it != filters_.end(); ++it) {
    it->Filter(&markers_);
  }

  // TODO: 3D stuff.
}
