#include "calibration.h"
#include "ui_calibrate.h"

using std::shared_ptr;
using std::vector;

CalibrateDialog::CalibrateDialog(shared_ptr<FrameFetcher> source,
                                 QWidget *parent)
    : QDialog(parent),
      ui(new Ui::CalibrateDialog),
      source_(source),
      min_viewpoint_distance_(0.05),
      capturing_(false),
      board_okay_(false) {
  ui->setupUi(this);

  // Set up the graphics view.
  auto view = this->findChild<QGraphicsView *>("graphicsView");
  view->setScene(&scene_);
  view->show();

  // Begin the timer.
  timer_ = new QTimer(this);
  connect(timer_, SIGNAL(timeout()), this, SLOT(ProcessFrame()));
  timer_->start(100);

  // Estimate initial camera parameters.
  cv::Mat frame = source_->GetNextFrame();
  camera_ = GuessCamera(frame.size());
}

CalibrateDialog::~CalibrateDialog() {
  delete ui;
  delete timer_;
}

void CalibrateDialog::ProcessFrame() {
  // Make sure there are frames for us to fetch.
  if (!source_->HasNextFrame()) {
    return;
  }

  // Get the frame.
  cv::Mat frame = source_->GetNextFrame();

  // Perform the calibrations if requested.
  if (capturing_ && board_okay_) {
    CalibrationFrame(frame);
  }

  // Resize the view to fit the frame.
  auto view = this->findChild<QGraphicsView *>("graphicsView");
  view->setFixedSize(frame.size().width, frame.size().height);

  // Draw the frame to the screen.
  scene_.clear();
  scene_.addPixmap(MatToQPixmap(frame));
}

void CalibrateDialog::OpenBoardDialog() {
  // Get the board file.
  QFileDialog dialog(this, "Open board file.");
  dialog.setNameFilter("Board files (*.board)");
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (!dialog.exec()) {
    return;
  }

  try {
    board_.readFromFile(dialog.selectedFiles()[0].toStdString());
  }
  catch (cv::Exception e) {
    QMessageBox msg(this);
    msg.setText("Error reading from board file.");
    msg.exec();
    return;
  }

  board_okay_ = true;
  detector_.setParams(board_);
}

void CalibrateDialog::OpenSaveDialog() {
  // Make sure we have enough viewpoints to calibrate.
  if (this->findChild<QLCDNumber *>("viewpointCount")->intValue() < 3) {
    QMessageBox msg(this);
    msg.setText("Need >= 3 viewpoints before saving.");
    msg.exec();
    return;
  }

  // Get the board file.
  QFileDialog dialog(this, "Save camera file.");
  dialog.setDefaultSuffix("camera");
  dialog.setNameFilter("Camera files (*.camera)");
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  if (!dialog.exec()) {
    return;
  }

  // Perform final calibration.
  try {
    QProgressDialog progress(this);
    progress.setLabelText("Performing final calibration.");
    progress.setMinimum(0);
    progress.setMaximum(0);
    progress.open();
    cv::calibrateCamera(object_points_, image_points_, camera_.CamSize,
                        camera_.CameraMatrix, camera_.Distorsion, r_vectors_,
                        t_vectors_);
    progress.close();
  }
  catch (cv::Exception e) {
    QMessageBox msg(this);
    msg.setText("Final calibration failed.");
    msg.exec();
  }

  // Save the file.
  camera_.saveToFile(dialog.selectedFiles()[0].toStdString(), true);
}

void CalibrateDialog::ToggleCapture(bool state) {
  // Capturing can only begin once we have a board to look for.
  if (!board_okay_ && state) {
    this->findChild<QPushButton *>("capture")->setChecked(false);
    QMessageBox msg(this);
    msg.setText("Load a board before capturing.");
    msg.exec();
    return;
  }

  // Make sure the board is in the correct scale.
  if (!board_.isExpressedInMeters()) {
    ConvertBoard();
    detector_.setParams(board_);
  }

  capturing_ = state;

  // Make sure the user can't alter options whilst capturing.
  if (capturing_) {
    this->findChild<QSpinBox *>("markerSize")->setDisabled(true);
    this->findChild<QPushButton *>("loadBoard")->setDisabled(true);

    // Reset the calibration information.
    this->findChild<QLCDNumber *>("viewpointCount")->display(0);
    image_points_.clear();
    object_points_.clear();
    r_vectors_.clear();
    t_vectors_.clear();
  } else {
    this->findChild<QSpinBox *>("markerSize")->setDisabled(false);
    this->findChild<QPushButton *>("loadBoard")->setDisabled(false);
  }
}

void CalibrateDialog::CalibrationFrame(cv::Mat &f) {
  // First detect any of the board's markers in the frame and outline them.
  float confidence = detector_.detect(f);

  for (size_t i = 0; i < detector_.getDetectedBoard().size(); ++i) {
    detector_.getDetectedBoard()[i].draw(f, cv::Scalar(0, 0, 255));
  }

  // Only use viewpoints from frames where we're confident a large part of the
  // board is visible.
  if (confidence > 0.2) {
    vector<cv::Point3f> obj_points;
    vector<cv::Point2f> img_points;
    GetPoints(detector_.getDetectedBoard(), &obj_points, &img_points);
    SetViewpoint(obj_points, img_points);
  }
}

aruco::CameraParameters CalibrateDialog::GuessCamera(cv::Size s) {
  aruco::CameraParameters camera;
  cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_32F);
  camera_matrix.at<float>(0, 0) = 500;
  camera_matrix.at<float>(1, 1) = 500;
  camera_matrix.at<float>(0, 2) = s.width / 2;
  camera_matrix.at<float>(1, 2) = s.height / 2;
  camera.setParams(camera_matrix, cv::Mat::zeros(1, 5, CV_32F), s);
  return camera;
}

float CalibrateDialog::GetMinimumDistanceToStoredT(cv::Mat t) {
  float lowest = std::numeric_limits<float>::max();
  for (auto it = t_vectors_.begin(); it != t_vectors_.end(); ++it) {
    float normalised = cv::norm(t - *it);
    if (normalised < lowest) {
      lowest = normalised;
    }
  }
  return lowest;
}

void CalibrateDialog::SetViewpoint(vector<cv::Point3f> &obj_points,
                                   vector<cv::Point2f> &img_points) {
  cv::Mat r_vector, t_vector;

  // Attempt to calculate the current location given the points.
  cv::solvePnP(obj_points, img_points, camera_.CameraMatrix, camera_.Distorsion,
               r_vector, t_vector);

  // Ensure the selected viewpoints are adequately spaces.
  if (GetMinimumDistanceToStoredT(t_vector) < min_viewpoint_distance_) {
    return;
  }

  // Advance the viewpoint count.
  QLCDNumber *counter = this->findChild<QLCDNumber *>("viewpointCount");
  counter->display(counter->intValue() + 1);

  // Record the new points.
  image_points_.push_back(img_points);
  object_points_.push_back(obj_points);
  t_vectors_.push_back(t_vector);
  r_vectors_.push_back(r_vector);

  // Recalibrate the camera for better detection, but only if we have enough
  // points to be accurate (and not so many that it would be too slow).
  if (object_points_.size() < 3 || object_points_.size() > 6) {
    return;
  }

  cv::calibrateCamera(object_points_, image_points_, camera_.CamSize,
                      camera_.CameraMatrix, camera_.Distorsion, r_vectors_,
                      t_vectors_);
}

void CalibrateDialog::GetPoints(aruco::Board &b,
                                std::vector<cv::Point3f> *obj_points,
                                std::vector<cv::Point2f> *img_points) {
  int num_points = b.size() * 4;

  // TODO : What does *any* of this do?
  for (size_t i = 0; i < b.size(); ++i) {
    const aruco::MarkerInfo &info = b.conf.getMarkerInfo(b[i].id);
    for (size_t j = 0; j < 4; ++j) {
      img_points->push_back(b[i][j]);
      obj_points->push_back(info[j]);
    }
  }
}

double CalibrateDialog::GetMarkerSize() {
  QSpinBox *b = this->findChild<QSpinBox *>("markerSize");
  return double(b->value()) / 1000.0;
}

void CalibrateDialog::ConvertBoard() {
  // First get the size of each marker in pixels and determine a scaling
  // factor.
  double marker_size_pixels = cv::norm(board_[0][0] - board_[0][1]);
  double pixel_size = GetMarkerSize() / marker_size_pixels;

  // Now change the board size flag and the size of each marker.
  board_.mInfoType = aruco::BoardConfiguration::METERS;
  for (size_t i = 0; i < board_.size(); ++i) {
    for (int j = 0; j < 4; ++j) {
      board_[i][j] *= pixel_size;
    }
  }
}
