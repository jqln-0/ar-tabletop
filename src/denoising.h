#ifndef __DENOISING_H_
#define __DENOISING_H_

#include <memory>
#include <aruco/aruco.h>

#include "framefetcher.h"

class DenoisingFrameFetcher : public FrameFetcher {
 protected:
  std::shared_ptr<FrameFetcher> wrapped_;

 public:
  DenoisingFrameFetcher();
  DenoisingFrameFetcher(FrameFetcher *w);
  virtual ~DenoisingFrameFetcher();

  virtual cv::Mat GetNextFrame() = 0;
  virtual bool HasNextFrame() const;

  virtual std::shared_ptr<FrameFetcher> wrapped() const;
  virtual void set_wrapped(std::shared_ptr<FrameFetcher> fetcher);
};

class GaussianDenoisingFrameFetcher : public DenoisingFrameFetcher {
 protected:
  const cv::Size kernel_;
  const double sigma_;

 public:
  GaussianDenoisingFrameFetcher(FrameFetcher *w, const cv::Size &kernel,
                                const double sigma);

  GaussianDenoisingFrameFetcher(FrameFetcher *w, const int kernel,
                                const double sigma);

  virtual ~GaussianDenoisingFrameFetcher();

  virtual cv::Mat GetNextFrame();
};

class MedianDenoisingFrameFetcher : public DenoisingFrameFetcher {
 protected:
  const int size_;

 public:
  MedianDenoisingFrameFetcher(FrameFetcher *w, const int size);
  virtual ~MedianDenoisingFrameFetcher();

  virtual cv::Mat GetNextFrame();
};

class BilateralDenoisingFrameFetcher : public DenoisingFrameFetcher {
 protected:
  const int d_;
  const double sigma_;

 public:
  BilateralDenoisingFrameFetcher(FrameFetcher *w, const int d,
                                 const double sigma);
  virtual ~BilateralDenoisingFrameFetcher();

  virtual cv::Mat GetNextFrame();
};

class NonLocalMeansDenoisingFrameFetcher : public DenoisingFrameFetcher {
 public:
  NonLocalMeansDenoisingFrameFetcher(FrameFetcher *w);
  virtual ~NonLocalMeansDenoisingFrameFetcher();

  virtual cv::Mat GetNextFrame();
};

#endif  // __DENOISING_H_
