#ifndef __MATCONV_H_
#define __MATCONV_H_

#include <QImage>
#include <aruco/aruco.h>

QImage matToQImage(const cv::Mat &m);

#endif  // __MATCONV_H_
