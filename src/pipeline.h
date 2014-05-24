#ifndef __PIPELINE_H_
#define __PIPELINE_H_

#include <QImage>
#include <QObject>

#include <aruco/aruco.h>

#include <memory>
#include <vector>

#include "denoising.h"
#include "framefetcher.h"
#include "filter.h"
#include "matconv.h"

class Pipeline : public QObject {
  Q_OBJECT

 protected:
  // Frame fetching and processing.
  std::shared_ptr<FrameFetcher> fetcher_;
  cv::Mat frame_;

  // Marker detection.
  mutable aruco::MarkerDetector detector_;
  aruco::BoardDetector board_detector_;
  std::vector<std::shared_ptr<MarkerFilter>> filters_;
  std::vector<aruco::Marker> markers_;

  // Scene information.
  aruco::CameraParameters camera_;
  aruco::BoardConfiguration board_;

  void DrawMarkers(cv::Mat dest) const;

 public:
  Pipeline();
  virtual ~Pipeline();

  std::shared_ptr<FrameFetcher> fetcher() const;
  void set_fetcher(std::shared_ptr<FrameFetcher> f);
  void set_fetcher(std::shared_ptr<DenoisingFrameFetcher> f);

  void set_camera(const aruco::CameraParameters &c);
  void set_board(const aruco::BoardConfiguration &b);

  const std::vector<aruco::Marker> &markers() const;

  QImage GetFrame(bool markers = false) const;
  QImage GetThresholdedFrame(bool markers = false) const;

  cv::Size GetFrameSize() const;

  bool IsReady() const;

  // ProcessFrame fetches a frame and performs marker detection on it.
  void ProcessFrame();
};

#endif  // __PIPELINE_H_
