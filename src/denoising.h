#ifndef __DENOISING_H_
#define __DENOISING_H_

#include <memory>
#include <aruco/aruco.h>

#include "framefetcher.h"

class DenoisingFrameFetcher : public FrameFetcher {
 protected:
  FrameFetcher *wrapped_;

 public:
  DenoisingFrameFetcher(FrameFetcher *w);
  virtual ~DenoisingFrameFetcher();

  virtual std::unique_ptr<cv::Mat> next_frame() = 0;
  virtual bool has_next_frame();

  virtual FrameFetcher *wrapped() { return wrapped_; }
};

class GaussianDenoisingFrameFetcher : public DenoisingFrameFetcher {
 protected:
  cv::Size kernel_;
  double sigma_;

 public:
  GaussianDenoisingFrameFetcher(FrameFetcher *w, cv::Size kernel, double sigma);
  virtual ~GaussianDenoisingFrameFetcher();

  virtual std::unique_ptr<cv::Mat> next_frame();
};

class MedianDenoisingFrameFetcher : public DenoisingFrameFetcher {
 protected:
  int size_;

 public:
  MedianDenoisingFrameFetcher(FrameFetcher *w, int size);
  virtual ~MedianDenoisingFrameFetcher();

  virtual std::unique_ptr<cv::Mat> next_frame();
};

class BilateralDenoisingFrameFetcher : public DenoisingFrameFetcher {
 protected:
  int d_;
  double sigma_;

 public:
  BilateralDenoisingFrameFetcher(FrameFetcher *w, int d, double sigma);
  virtual ~BilateralDenoisingFrameFetcher();

  virtual std::unique_ptr<cv::Mat> next_frame();
};

class NonLocalMeansDenoisingFrameFetcher : public DenoisingFrameFetcher {
 public:
  NonLocalMeansDenoisingFrameFetcher(FrameFetcher *w);
  virtual ~NonLocalMeansDenoisingFrameFetcher();

  virtual std::unique_ptr<cv::Mat> next_frame();
};

#endif  // __DENOISING_H_
