#ifndef __PIPELINE_H_
#define __PIPELINE_H_

#include <QImage>
#include <vector>

#include <aruco/aruco.h>

#include "denoising.h"
#include "framefetcher.h"
#include "filter.h"
#include "matconv.h"

class Pipeline {
 protected:
  // Frame fetching and processing.
  FrameFetcher *source_fetcher_;
  DenoisingFrameFetcher *wrapping_fetcher_;
  cv::Mat frame_;

  // Marker detection.
  aruco::MarkerDetector *detector_;
  std::vector<MarkerFilter> filters_;
  std::vector<aruco::Marker> markers_;

  // Scene information.
  aruco::CameraParameters camera_;
  aruco::BoardConfiguration board_;

 public:
  Pipeline();
  virtual ~Pipeline();

  FrameFetcher *source_fetcher();
  void set_source_fetcher(FrameFetcher *f);
  DenoisingFrameFetcher *wrapping_fetcher();
  void set_wrapping_fetcher(DenoisingFrameFetcher *f);

  std::vector<aruco::Marker> *markers();

  bool GetFrame(QImage *dest);
  bool GetThresholdedFrame(QImage *dest);
  void DrawMarkers();

  // ProcessFrame fetches a frame and performs marker detection on it.
  void ProcessFrame();
};

#endif  // __PIPELINE_H_
