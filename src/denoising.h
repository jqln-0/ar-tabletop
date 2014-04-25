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

  virtual bool GetNextFrame(cv::Mat *dest) = 0;
  virtual bool HasNextFrame() const;

  virtual FrameFetcher *wrapped() const { return wrapped_; }
};

class GaussianDenoisingFrameFetcher : public DenoisingFrameFetcher {
 protected:
  const cv::Size kernel_;
  const double sigma_;

 public:
  GaussianDenoisingFrameFetcher(FrameFetcher *w, const cv::Size &kernel,
                                const double sigma);
  virtual ~GaussianDenoisingFrameFetcher();

  virtual bool GetNextFrame(cv::Mat *dest);
};

class MedianDenoisingFrameFetcher : public DenoisingFrameFetcher {
 protected:
  const int size_;

 public:
  MedianDenoisingFrameFetcher(FrameFetcher *w, const int size);
  virtual ~MedianDenoisingFrameFetcher();

  virtual bool GetNextFrame(cv::Mat *dest);
};

class BilateralDenoisingFrameFetcher : public DenoisingFrameFetcher {
 protected:
  const int d_;
  const double sigma_;

 public:
  BilateralDenoisingFrameFetcher(FrameFetcher *w, const int d,
                                 const double sigma);
  virtual ~BilateralDenoisingFrameFetcher();

  virtual bool GetNextFrame(cv::Mat *dest);
};

class NonLocalMeansDenoisingFrameFetcher : public DenoisingFrameFetcher {
 public:
  NonLocalMeansDenoisingFrameFetcher(FrameFetcher *w);
  virtual ~NonLocalMeansDenoisingFrameFetcher();

  virtual bool GetNextFrame(cv::Mat *dest);
};

#endif  // __DENOISING_H_
