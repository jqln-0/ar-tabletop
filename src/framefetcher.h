#ifndef __FRAMEFETCHER_H_
#define __FRAMEFETCHER_H_

#include <memory>
#include <aruco/aruco.h>

class FrameFetcher {
 public:
  virtual ~FrameFetcher() {};

  virtual std::unique_ptr<cv::Mat> next_frame() = 0;
  virtual bool has_next_frame() const = 0;
};

class WebcamFrameFetcher : public FrameFetcher {
 protected:
  cv::VideoCapture *video_capturer_;

 public:
  WebcamFrameFetcher(const int device_index = 0);
  ~WebcamFrameFetcher();

  virtual std::unique_ptr<cv::Mat> next_frame();
  virtual bool has_next_frame() const;
};

class VideoFrameFetcher : public FrameFetcher {
 protected:
  cv::VideoCapture *video_capturer_;

 public:
  VideoFrameFetcher(const string &filename);
  ~VideoFrameFetcher();

  virtual std::unique_ptr<cv::Mat> next_frame();
  virtual bool has_next_frame() const;
};

class PhotoFrameFetcher : public FrameFetcher {
 protected:
  cv::Mat *image_;

 public:
  PhotoFrameFetcher(const string &filename);
  ~PhotoFrameFetcher();

  virtual std::unique_ptr<cv::Mat> next_frame();
  virtual bool has_next_frame() const;
};

#endif  // __FRAMEFETCHER_H_
