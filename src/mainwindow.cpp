#include "mainwindow.h"
#include "ui_window.h"

#include <iostream>

using std::make_shared;
using std::shared_ptr;
using std::static_pointer_cast;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      frame_timer_(nullptr),
      capturing_(true),
      show_threshold_(false),
      show_markers_(false),
      camera_resized_(false) {
  ui->setupUi(this);

  frame_timer_ = new QTimer(this);
  connect(frame_timer_, SIGNAL(timeout()), this, SLOT(ProcessFrame()));
  frame_timer_->start(100);

  auto view = this->findChild<QGraphicsView *>("graphicsView");
  scene_3d_ = make_shared<SceneWidget>(this);
  scene_3d_->hide();
}

MainWindow::~MainWindow() {
  delete frame_timer_;
  delete ui;
}

void MainWindow::ProcessFrame() {
  // Check if we are ready and permitted to process frames.
  if (!capturing_ || !processor_.IsReady()) {
    return;
  }

  // Run through the processing pipeline.
  processor_.ProcessFrame();

  if (!camera_resized_ && camera_.isValid()) {
    camera_.resize(processor_.GetFrameSize());
    processor_.set_camera(camera_);
    scene_3d_->set_camera(camera_);
    camera_resized_ = true;
  }

  // Get the background image.
  QImage backdrop;
  if (show_threshold_) {
    backdrop = processor_.GetThresholdedFrame(show_markers_);
  } else if (show_markers_) {
    backdrop = processor_.GetFrame(show_markers_);
  } else if (current_scene_ && current_scene_->HasBackground()) {
    backdrop = current_scene_->background();
  } else {
    backdrop = processor_.GetFrame(show_markers_);
  }

  // Do nothing if the frame was bad.
  if (processor_.GetFrameSize().area() == 0) {
    return;
  }

  // Inform the SceneWidget of the detected markers.
  scene_3d_->set_markers(processor_.markers());
  if (processor_.HasBoard()) {
    scene_3d_->set_board(processor_.detected());
  }

  // Set the view and scene to the correct size.
  QSize frame_size(processor_.GetFrameSize().width,
                   processor_.GetFrameSize().height);
  auto view = this->findChild<QGraphicsView *>("graphicsView");
  scene_3d_->setFixedSize(frame_size);
  view->setFixedSize(frame_size);

  // Draw image and scene to view.
  scene_2d_.clear();
  scene_2d_.setBackgroundBrush(backdrop);
  if (!show_threshold_ && !show_markers_) {
    scene_2d_.addPixmap(
        scene_3d_->renderPixmap(frame_size.width(), frame_size.height()));
  }
  view->setScene(&scene_2d_);
}

void MainWindow::OpenCalibrateDialog() {
  // We need to have a source of frames to pass to the calibrator.
  shared_ptr<FrameFetcher> fetcher = processor_.fetcher();
  if (!fetcher) {
    QMessageBox msg(this);
    msg.setText("Select a source (webcam or file) before calibrating.");
    msg.exec();
    return;
  }

  // Pause capturing until the calibrator is done since it'll want to use the
  // FrameFetcher.
  capturing_ = false;
  CalibrateDialog calibrator(fetcher, this);
  calibrator.exec();
  capturing_ = true;
}

void MainWindow::OpenFilteringDialog() {
  NoiseFilterDialog dialog(this);
  dialog.setModal(true);
  dialog.exec();
  if (dialog.result() == dialog.Rejected) {
    return;
  }

  shared_ptr<FrameFetcher> fetcher = dialog.MakeFilter();
  if (fetcher) {
    processor_.set_fetcher(fetcher);
  }
}

void MainWindow::OpenGenerateBoardDialog() {
  GenerateBoardDialog dialog(this);
  dialog.setModal(true);
  dialog.exec();
}

void MainWindow::OpenGenerateMarkerDialog() {
  GenerateMarkerDialog dialog(this);
  dialog.setModal(true);
  dialog.exec();
}

void MainWindow::OpenIntrinsicsFile() {
  // Get the user's file.
  QFileDialog dialog(this, "Open camera file.");
  dialog.setNameFilter("Camera files (*.camera)");
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (!dialog.exec()) {
    return;
  }

  // Try to read camera parameters from the file.
  try {
    camera_.readFromXMLFile(dialog.selectedFiles()[0].toStdString());

    // The new camera is likely the wrong size. Make sure we resize it before
    // use.
    processor_.set_camera(aruco::CameraParameters());
    scene_3d_->set_camera(aruco::CameraParameters());
    camera_resized_ = false;
  }
  catch (cv::Exception e) {
    QMessageBox msg(this);
    msg.setText("Failed to read camera parameters from file.");
    msg.exec();
    return;
  }
}

void MainWindow::OpenSceneFile() {
  QFileDialog dialog(this, "Open scene file.");
  dialog.setNameFilter("Scene files (*.scene)");
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (!dialog.exec()) {
    return;
  }

  // Load the given scene file.
  current_scene_ = make_shared<Scene>(dialog.selectedFiles()[0]);

  // Inform the 3D scene renderer.
  scene_3d_->set_scene(current_scene_);

  if (current_scene_->HasBoard()) {
    processor_.set_board(current_scene_->board());
  }
}

void MainWindow::OpenSourceFile() {
  // Get the user's file.
  QFileDialog dialog(this, "Open source file");
  dialog.setNameFilter("Image files (*.png *.jpg);;Video files (*.avi)");
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (!dialog.exec()) {
    return;
  }

  // Try both kinds of possible fetcher, use whichever one is valid.
  shared_ptr<FrameFetcher> fetcher;

  // Try the photo fetcher first.
  fetcher =
      make_shared<PhotoFrameFetcher>(dialog.selectedFiles()[0].toStdString());

  // Alternatively try the video fetcher.
  if (!fetcher->HasNextFrame()) {
    fetcher =
        make_shared<VideoFrameFetcher>(dialog.selectedFiles()[0].toStdString());
  }

  // Check if the fetcher is valid.
  if (!fetcher->HasNextFrame()) {
    QMessageBox msg(this);
    msg.setText("Failed to read file! The format may be incorrect.");
    msg.exec();
    return;
  }

  // Assign the fetcher.
  processor_.set_fetcher(fetcher);
}

void MainWindow::OpenSourceWebcam() {
  // Try to create the appropriate fetcher.
  shared_ptr<FrameFetcher> fetcher = make_shared<WebcamFrameFetcher>();

  // See if we can get frames from it.
  if (!fetcher->HasNextFrame()) {
    QMessageBox msg(this);
    msg.setText("Error: Could not open webcam for reading.");
    msg.exec();
    return;
  }

  // Assign the fetcher to the pipeline.
  processor_.set_fetcher(fetcher);
}

void MainWindow::OpenThresholdDialog() {
  std::cout << "Open threshold dialog.\n";
}

void MainWindow::ToggleShowMarkers() { show_markers_ = !show_markers_; }

void MainWindow::ToggleShowThreshold() { show_threshold_ = !show_threshold_; }
