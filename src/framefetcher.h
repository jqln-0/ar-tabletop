#ifndef __FRAMEFETCHER_H_
#define __FRAMEFETCHER_H_

#include <aruco/aruco.h>

// The FrameFetcher interface is an abstraction of the application's source of
// frames. Frames from a webcam, still image or video file should all be
// handled identically by higher level code. This interface facilitates this.
class FrameFetcher {
 public:
  virtual ~FrameFetcher() {};

  // Fetches the next frame of input from the source and returns it.
  virtual cv::Mat GetNextFrame() = 0;

  // Returns whether or not this fetcher's source of frames still has frames
  // remaining.
  virtual bool HasNextFrame() const = 0;
};

// Adapts the FrameFetcher for a video file source.
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

// Further adapts the video fetcher to get the source video from a webcam
// device.
class WebcamFrameFetcher : public VideoFrameFetcher {
 public:
  WebcamFrameFetcher(const int device_index = 0);
  ~WebcamFrameFetcher();
};

// Adapts a single image file into a source of frames. Provided the inital
// image loading is successful, this fetcher will never run out of frames.
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
