#ifndef __FRAMEFETCHER_H_
#define __FRAMEFETCHER_H_

#include <aruco/aruco.h>

class FrameFetcher {
 public:
  virtual ~FrameFetcher() {};

  virtual cv::Mat GetNextFrame() = 0;
  virtual bool HasNextFrame() const = 0;
};

class VideoFrameFetcher : public FrameFetcher {
 protected:
  cv::VideoCapture video_capturer_;
  VideoFrameFetcher();

 public:
  VideoFrameFetcher(const string &filename);
  ~VideoFrameFetcher();

  virtual cv::Mat GetNextFrame();
  virtual bool HasNextFrame() const;
};

class WebcamFrameFetcher : public VideoFrameFetcher {
 public:
  WebcamFrameFetcher(const int device_index = 0);
  ~WebcamFrameFetcher();
};

class PhotoFrameFetcher : public FrameFetcher {
 protected:
  cv::Mat image_;

 public:
  PhotoFrameFetcher(const string &filename);
  ~PhotoFrameFetcher();

  virtual cv::Mat GetNextFrame();
  virtual bool HasNextFrame() const;
};

#endif  // __FRAMEFETCHER_H_
