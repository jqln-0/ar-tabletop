#ifndef __PIPELINE_H_
#define __PIPELINE_H_

#include <QImage>
#include <QObject>

#include <aruco/aruco.h>

#include <memory>
#include <vector>

#include "boardwrap.h"
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
  Board board_;

  void DrawMarkers(cv::Mat dest) const;

 public:
  Pipeline();
  virtual ~Pipeline();

  std::shared_ptr<FrameFetcher> GetFetcher() const;
  void SetFetcher(std::shared_ptr<FrameFetcher> f);
  void SetFetcher(std::shared_ptr<DenoisingFrameFetcher> f);

  void SetCamera(const aruco::CameraParameters &c);
  void SetBoard(const Board &b);

  const std::vector<aruco::Marker> &GetMarkers() const;

  QImage GetFrame(bool markers = false) const;
  QImage GetThresholdedFrame(bool markers = false) const;

  Board GetBoard() const;

  cv::Size GetFrameSize() const;

  bool IsReady() const;

  void ProcessFrame();
};

#endif  // __PIPELINE_H_
