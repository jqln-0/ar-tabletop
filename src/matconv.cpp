#include "matconv.h"

using cv::Mat;

bool matToQImage(const Mat &src, QImage *dest) {
  // Convert the Mat to RGB colorspace.
  Mat conv;
  try {
    cv::cvtColor(src, conv, cv::COLOR_BGR2RGB);
  }
  catch (cv::Exception) {
    return false;
  }

  // Construct the QImage.
  *dest = QImage((unsigned char *)conv.data, conv.cols, conv.rows,
                 QImage::Format_RGB888);

  return true;
}
