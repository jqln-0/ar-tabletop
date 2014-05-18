#ifndef __CALIBRATION_H_
#define __CALIBRATION_H_

#include <QDialog>
#include <QFileDialog>
#include <QGraphicsView>
#include <QLCDNumber>
#include <QMessageBox>
#include <QProgressDialog>
#include <QTimer>
#include <memory>
#include <vector>

#include <aruco/aruco.h>

#include "framefetcher.h"
#include "matconv.h"

namespace Ui {
class CalibrateDialog;
}

class CalibrateDialog : public QDialog {
  Q_OBJECT

 public:
  explicit CalibrateDialog(std::shared_ptr<FrameFetcher> source,
                           QWidget *parent = 0);
  ~CalibrateDialog();

 public
slots:
  void ProcessFrame();
  void OpenBoardDialog();
  void OpenSaveDialog();
  void ToggleCapture(bool);

  // Calibration methods.
  void CalibrationFrame(cv::Mat &f);
  aruco::CameraParameters GuessCamera(cv::Size s);
  float GetMinimumDistanceToStoredT(cv::Mat t);
  void SetViewpoint(std::vector<cv::Point3f> &obj_points,
                    std::vector<cv::Point2f> &img_points);
  void GetPoints(aruco::Board &b, std::vector<cv::Point3f> *obj_points,
                 std::vector<cv::Point2f> *img_points);

 protected:
  // Capture vars.
  bool capturing_;
  std::shared_ptr<FrameFetcher> source_;
  QTimer *timer_;
  QGraphicsScene scene_;

  // Configuration vars.
  bool board_okay_;
  aruco::BoardConfiguration board_;
  aruco::BoardDetector detector_;
  aruco::CameraParameters camera_;
  std::vector<std::vector<cv::Point2f>> image_points_;
  std::vector<std::vector<cv::Point3f>> object_points_;
  double min_viewpoint_distance_;

  // Aruco uses magical, unexplained math to do some calculations. To be honest
  // I have no idea what these are used for.
  std::vector<cv::Mat> r_vectors_, t_vectors_;

  double GetMarkerSize();
  void ConvertBoard();

 private:
  Ui::CalibrateDialog *ui;
};

#endif  // __CALIBRATION_H_
