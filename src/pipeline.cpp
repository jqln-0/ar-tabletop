#include "pipeline.h"

using aruco::Marker;
using cv::Mat;
using std::dynamic_pointer_cast;
using std::shared_ptr;
using std::vector;

Pipeline::Pipeline() {
  filters_.push_back(
      std::make_shared<SmoothingMarkerFilter>(SmoothingMarkerFilter(3)));
}

Pipeline::~Pipeline() {}

shared_ptr<FrameFetcher> Pipeline::GetFetcher() const { return fetcher_; }

void Pipeline::SetFetcher(shared_ptr<FrameFetcher> f) {
  // Check if either the current fetcher or the given fetcher is really a
  // DenoisingFrameFetcher;
  auto f_cast = dynamic_pointer_cast<DenoisingFrameFetcher>(f);
  auto fetcher_cast = dynamic_pointer_cast<DenoisingFrameFetcher>(fetcher_);

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

const vector<Marker> &Pipeline::GetMarkers() const { return markers_; }

void Pipeline::SetCamera(const aruco::CameraParameters &c) { camera_ = c; }

void Pipeline::SetBoard(const Board &b) { board_ = b; }

Board Pipeline::GetBoard() const { return board_; }

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

cv::Size Pipeline::GetFrameSize() const { return frame_.size(); }

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
  if (!fetcher_->HasNextFrame()) {
    return;
  }

  if (camera_.isValid()) {
    cv::Mat raw_frame = fetcher_->GetNextFrame();
    cv::undistort(raw_frame, frame_, camera_.CameraMatrix, camera_.Distorsion);
  } else {
    frame_ = fetcher_->GetNextFrame();
  }

  // Next run our marker detector and filters.
  detector_.detect(frame_, markers_, camera_, 1.0);
  for (auto it = filters_.begin(); it != filters_.end(); ++it) {
    (*it)->Filter(&markers_);
  }

  // Detect the given board if we have enough data.
  if (camera_.isValid() && board_.HasConfig()) {
    aruco::Board detected_board;
    board_detector_.detect(markers_, board_.GetConfig(), detected_board,
                           camera_.CameraMatrix, camera_.Distorsion, 1);
    board_.SetBoard(detected_board);
  }
}
