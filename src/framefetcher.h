#ifndef __FRAMEFETCHER_H_
#define __FRAMEFETCHER_H_

#include <memory>
#include <aruco/aruco.h>

class FrameFetcher {
 public:
  virtual ~FrameFetcher() {};

  virtual bool GetNextFrame(cv::Mat *dest) = 0;
  virtual bool HasNextFrame() const = 0;
};

class WebcamFrameFetcher : public FrameFetcher {
 protected:
  cv::VideoCapture video_capturer_;

 public:
  WebcamFrameFetcher(const int device_index = 0);
  ~WebcamFrameFetcher();

  virtual bool GetNextFrame(cv::Mat *dest);
  virtual bool HasNextFrame() const;
};

class VideoFrameFetcher : public FrameFetcher {
 protected:
  cv::VideoCapture video_capturer_;

 public:
  VideoFrameFetcher(const string &filename);
  ~VideoFrameFetcher();

  virtual bool GetNextFrame(cv::Mat *dest);
  virtual bool HasNextFrame() const;
};

class PhotoFrameFetcher : public FrameFetcher {
 protected:
  cv::Mat image_;

 public:
  PhotoFrameFetcher(const string &filename);
  ~PhotoFrameFetcher();

  virtual bool GetNextFrame(cv::Mat *dest);
  virtual bool HasNextFrame() const;
};

#endif  // __FRAMEFETCHER_H_
