#include "matconv.h"

QImage matToQImage(const cv::Mat &m) {
  // Convert the Mat to RGB colorspace.
  cv::Mat conv;
  cv::cvtColor(m, conv, cv::COLOR_BGR2RGB);

  // Construct and return the QImage.
  return QImage((unsigned char *)conv.data, conv.cols, conv.rows,
                QImage::Format_RGB888);
}
