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
      capturing_(false) {
  ui->setupUi(this);

  // Set up the graphics view.
  auto view = this->findChild<QGraphicsView *>("graphicsView");
  view->setScene(&scene_);
  view->show();

  // Begin the frame capture timer.
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
  // Make sure there is a frame for us to fetch.
  if (!source_->HasNextFrame()) {
    return;
  }

  // Get the frame.
  cv::Mat frame = source_->GetNextFrame();

  // Perform the calibration if we need to.
  if (capturing_) {
    CalibrationFrame(frame);
  }

  // Resize the view to fit the captured frame.
  auto view = this->findChild<QGraphicsView *>("graphicsView");
  view->setFixedSize(frame.size().width, frame.size().height);

  // Draw the frame to the screen.
  scene_.clear();
  scene_.addPixmap(MatToQPixmap(frame));
}

void CalibrateDialog::OpenBoardDialog() {
  // Get the board filename.
  QString filename = QFileDialog::getOpenFileName(this, "Open board file.", "",
                                                  "Board files (*.board)");

  // Check if the user hit cancel.
  if (filename == "") {
    return;
  }

  // Try to load the configuration from the given file.
  if (!board_.LoadConfig(filename)) {
    ShowMessage(this, "Error reading from board file.");
    return;
  }

  // Set the board the detector is using.
  detector_.setParams(board_.GetConfig());
}

void CalibrateDialog::OpenSaveDialog() {
  // Make sure we have enough viewpoints to calibrate.
  auto counter = this->findChild<QLCDNumber *>("viewpointCount");
  if (counter->intValue() < 3) {
    ShowMessage(this, "Need to capture at least 3 viewpoints before saving.");
    return;
  }

  // Prompt the user for a destination filename.
  QString filename = QFileDialog::getSaveFileName(this, "Save camera file.", "",
                                                  "Camera files (*.camera)");

  // Check for user cancelling.
  if (filename == "") {
    return;
  }

  // Perform the final calibration.
  try {
    // TODO: Consider doing this twice and removing outliar points.
    cv::calibrateCamera(object_points_, image_points_, camera_.CamSize,
                        camera_.CameraMatrix, camera_.Distorsion, r_vectors_,
                        t_vectors_);
  }
  catch (cv::Exception e) {
    qWarning() << e.what();
    ShowMessage(this, "Final calibration failed.");
    return;
  }

  // Save the file.
  camera_.saveToFile(filename.toStdString(), true);
}

void CalibrateDialog::ToggleCapture(bool state) {
  if (state) {
    // User is trying to begin calibration.

    // Calibration can only begin once we have a board config to look for.
    if (!board_.HasConfig()) {
      ShowMessage(this, "Load a board before beginning calibration.");
      return;
    }

    // Ensure the board is in the correct scale. We do this here rather than on
    // load so that the user can set the marker size after loading a board.
    if (!board_.GetConfig().isExpressedInMeters()) {
      ConvertBoard();
    }

    // Every seems good; calibration can now start!
    capturing_ = true;

    // Reset calibration data from previous runs.
    this->findChild<QLCDNumber *>("viewpointCount")->display(0);
    image_points_.clear();
    object_points_.clear();
    r_vectors_.clear();
    t_vectors_.clear();
  } else {
    // User is trying to end calibration.
    capturing_ = false;
  }

  // Make sure the user can't edit options whilst capturing.
  this->findChild<QSpinBox *>("markerSize")->setDisabled(capturing_);
  this->findChild<QPushButton *>("loadBoard")->setDisabled(capturing_);
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

aruco::CameraParameters CalibrateDialog::GuessCamera(const cv::Size &s) {
  aruco::CameraParameters camera;
  cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_32F);
  camera_matrix.at<float>(0, 0) = 500;
  camera_matrix.at<float>(1, 1) = 500;
  camera_matrix.at<float>(0, 2) = s.width / 2;
  camera_matrix.at<float>(1, 2) = s.height / 2;
  camera.setParams(camera_matrix, cv::Mat::zeros(1, 5, CV_32F), s);
  return camera;
}

float CalibrateDialog::GetMinimumDistanceToStoredT(const cv::Mat &t) {
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

void CalibrateDialog::GetPoints(const aruco::Board &b,
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
  // Convert marker size from millimetres to metres.
  return double(b->value()) / 1000.0;
}

void CalibrateDialog::ConvertBoard() {
  if (!board_.HasConfig()) {
    return;
  }
  aruco::BoardConfiguration config = board_.GetConfig();

  // First get the size of each marker in pixels and determine a scaling
  // factor.
  double marker_size_pixels = cv::norm(config[0][0] - config[0][1]);
  double pixel_size = GetMarkerSize() / marker_size_pixels;

  // Now change the board size flag and the size of each marker.
  config.mInfoType = aruco::BoardConfiguration::METERS;
  for (size_t i = 0; i < config.size(); ++i) {
    for (int j = 0; j < 4; ++j) {
      config[i][j] *= pixel_size;
    }
  }

  board_.SetConfig(config);
  detector_.setParams(config);
}
