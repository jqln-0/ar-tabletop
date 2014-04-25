#ifndef __MATCONV_H_
#define __MATCONV_H_

#include <QImage>
#include <aruco/aruco.h>

bool matToQImage(const cv::Mat &src, QImage *dest);

#endif  // __MATCONV_H_
