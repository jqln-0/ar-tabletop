#ifndef __CALIBRATION_H_
#define __CALIBRATION_H_

#include <QDialog>
#include <QFileDialog>
#include <QGraphicsView>
#include <QLCDNumber>
#include <QProgressDialog>
#include <QtDebug>
#include <QTimer>

#include <aruco/aruco.h>

#include <memory>
#include <vector>

#include "boardwrap.h"
#include "framefetcher.h"
#include "matconv.h"
#include "message.h"

namespace Ui {
class CalibrateDialog;
}

class CalibrateDialog : public QDialog {
  Q_OBJECT

 public:
  explicit CalibrateDialog(std::shared_ptr<FrameFetcher> source,
                           QWidget *parent = nullptr);
  ~CalibrateDialog();

 public
slots:
  // Processes the next frame of input.
  void ProcessFrame();

  // Prompts the user to specify the board used for calibration.
  void OpenBoardDialog();

  // Prompts the user for a camera file to save the calibration data into.
  void OpenSaveDialog();

  // Toggles capturing of calibration frames.
  void ToggleCapture(bool);

  // Detects markers in the current frame, checks if the viewpoint is distinct
  // enough to be useful then saves the viewpoint.
  void CalibrationFrame(cv::Mat &f);

  // Generates good default camera parameters to be used for determining the
  // actual camera parameters.
  aruco::CameraParameters GuessCamera(const cv::Size &s);

  // Computes the distance between the given viewport t and the stored
  // viewports.
  float GetMinimumDistanceToStoredT(const cv::Mat &t);

  // Calculates the viewpoint from the given object and image points.
  void SetViewpoint(std::vector<cv::Point3f> &obj_points,
                    std::vector<cv::Point2f> &img_points);

  // Determines the object and image points (points in 3D space which correspond
  // to 2D space) of the board.
  void GetPoints(const aruco::Board &b, std::vector<cv::Point3f> *obj_points,
                 std::vector<cv::Point2f> *img_points);

 protected:
  // Flags whether or not we are currently capturing and processing calibration
  // frames.
  bool capturing_;

  // The source of frames.
  std::shared_ptr<FrameFetcher> source_;

  // Frame timer.
  QTimer *timer_;

  // Scene used to draw
  QGraphicsScene scene_;

  // The board used for calibration.
  Board board_;

  aruco::BoardDetector detector_;

  // Camera to hold calibration data.
  aruco::CameraParameters camera_;

  // Stored image and object points.
  std::vector<std::vector<cv::Point2f>> image_points_;
  std::vector<std::vector<cv::Point3f>> object_points_;

  // Minimum allowed distance between stored viewpoints.
  const double min_viewpoint_distance_;

  // Aruco uses magical, unexplained math to do some calculations. To be honest
  // I have no idea what these represent.
  std::vector<cv::Mat> r_vectors_, t_vectors_;

  double GetMarkerSize();

  // Converts the stored board's representation from pixels to metres.
  void ConvertBoard();

 private:
  Ui::CalibrateDialog *ui;
};

#endif  // __CALIBRATION_H_
