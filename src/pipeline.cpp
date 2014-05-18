#include "pipeline.h"

using aruco::Marker;
using cv::Mat;
using std::shared_ptr;
using std::vector;

Pipeline::Pipeline() {}

Pipeline::~Pipeline() {}

shared_ptr<FrameFetcher> Pipeline::fetcher() const { return fetcher_; }

void Pipeline::set_fetcher(shared_ptr<FrameFetcher> f) {
  // Check if either the current fetcher or the given fetcher is really a
  // DenoisingFrameFetcher;
  shared_ptr<DenoisingFrameFetcher> f_cast =
      std::dynamic_pointer_cast<DenoisingFrameFetcher>(f);
  shared_ptr<DenoisingFrameFetcher> fetcher_cast =
      std::dynamic_pointer_cast<DenoisingFrameFetcher>(fetcher_);

  if (fetcher_cast && f_cast) {
    // Both are DenoisingFrameFetchers. Replace the current outer fetcher with
    // the given outer fetcher.
    f_cast->set_wrapped(fetcher_cast->wrapped());
    fetcher_ = f_cast;
  } else if (fetcher_cast && !f_cast) {
    // Only the current fetcher is a DenoisingFrameFetcher. Replace the wrapped
    // fetcher.
    fetcher_cast->set_wrapped(f);
  } else if (!fetcher_cast && f_cast) {
    // Only the given fetcher is a DenoisingFrameFetcher. Wrap the current
    // fetcher.
    f_cast->set_wrapped(fetcher_);
    fetcher_ = f_cast;
  } else if (!fetcher_cast && !f_cast) {
    // Both fetchers are ordinary fetchers. Replace the current fetcher.
    fetcher_ = f;
  }
}

const vector<Marker> &Pipeline::markers() const { return markers_; }

void Pipeline::set_camera(const aruco::CameraParameters &c) { camera_ = c; }

void Pipeline::set_board(const aruco::BoardConfiguration &b) { board_ = b; }

QImage Pipeline::GetFrame(bool markers) const {
  // If markers are desired then we will need to draw them on a copy of the
  // frame.
  Mat out;
  if (markers) {
    out = frame_.clone();
    DrawMarkers(out);
  } else {
    out = frame_;
  }

  return MatToQImage(out);
}

QImage Pipeline::GetThresholdedFrame(bool markers) const {
  // As before, we need a copy of the Mat if we're to draw to it.
  Mat out;
  if (markers) {
    out = detector_.getThresholdedImage().clone();
    DrawMarkers(out);
  } else {
    out = detector_.getThresholdedImage();
  }

  return MatToQImage(out);
}

void Pipeline::DrawMarkers(Mat dest) const {
  for (auto it = markers_.cbegin(); it != markers_.cend(); ++it) {
    it->draw(dest, cv::Scalar(0, 0, 0), 1, true);
  }
}

bool Pipeline::IsReady() const { return bool(fetcher_); }

void Pipeline::ProcessFrame() {
  // First check if it's okay to fetch an image.
  if (!fetcher_ || !fetcher_->HasNextFrame()) {
    return;
  }

  // Fetch the image.
  // TODO: The fetcher should run in a seperate thread so we don't fall behind
  // webcam streams.
  if (!fetcher_->HasNextFrame()) {
    return;
  }
  frame_ = fetcher_->GetNextFrame();

  // Next run our detector and filters.
  // TODO: Use camera, board, marker info.
  detector_.detect(frame_, markers_, camera_, 1.0);
  for (auto it = filters_.begin(); it != filters_.end(); ++it) {
    (*it)->Filter(&markers_);
  }

  // TODO: 3D stuff?
}
