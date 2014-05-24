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
    processor_.SetCamera(camera_);
    scene_3d_->SetCamera(camera_);
    camera_resized_ = true;
  }

  // Get the background image.
  QImage backdrop;
  if (show_threshold_) {
    backdrop = processor_.GetThresholdedFrame(show_markers_);
  } else if (show_markers_) {
    backdrop = processor_.GetFrame(show_markers_);
  } else if (current_scene_ && !current_scene_->GetBackground().isNull()) {
    backdrop = current_scene_->GetBackground();
  } else {
    backdrop = processor_.GetFrame(show_markers_);
  }

  // Do nothing if the frame was bad.
  if (processor_.GetFrameSize().area() == 0) {
    return;
  }

  // Inform the SceneWidget of the detected markers.
  scene_3d_->SetMarkers(processor_.GetMarkers());

  // If a board was detected, inform the scene.
  if (processor_.GetBoard().HasBoard()) {
    current_scene_->SetBoard(processor_.GetBoard());
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
  shared_ptr<FrameFetcher> fetcher = processor_.GetFetcher();
  if (!fetcher) {
    ShowMessage(this, "Select a source (webcam or file) before calibrating.");
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
    processor_.SetFetcher(fetcher);
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
  QString filename = QFileDialog::getOpenFileName(this, "Open camera file.", "",
                                                  "Camera files (*.camera)");

  if (filename == "") {
    return;
  }

  // Try to read camera parameters from the file.
  try {
    camera_.readFromXMLFile(filename.toStdString());

    // The new camera is likely the wrong size. Make sure we resize it before
    // use.
    processor_.SetCamera(aruco::CameraParameters());
    scene_3d_->SetCamera(aruco::CameraParameters());
    camera_resized_ = false;
  }
  catch (cv::Exception e) {
    ShowMessage(this, "Failed to read camera parameters from file.");
    return;
  }
}

void MainWindow::OpenSceneFile() {
  QString filename = QFileDialog::getOpenFileName(this, "Open scene file.", "",
                                                  "Scene files (*.scene)");

  if (filename == "") {
    return;
  }

  // Load the given scene file.
  current_scene_ = make_shared<Scene>(filename);

  // Inform the 3D scene renderer.
  scene_3d_->SetScene(current_scene_);

  if (current_scene_->GetBoard().HasConfig()) {
    processor_.SetBoard(current_scene_->GetBoard());
  }
}

void MainWindow::OpenSourceFile() {
  QString filename = QFileDialog::getOpenFileName(
      this, "Open source file.", "",
      "Image files (*.png *.jpg);;Video file (*.avi)");

  if (filename == "") {
    return;
  }

  // Try both kinds of possible fetcher, use whichever one is valid.
  shared_ptr<FrameFetcher> fetcher;

  // Try the photo fetcher first.
  fetcher = make_shared<PhotoFrameFetcher>(filename.toStdString());

  // Alternatively try the video fetcher.
  if (!fetcher->HasNextFrame()) {
    fetcher = make_shared<VideoFrameFetcher>(filename.toStdString());
  }

  // Check if the fetcher is valid.
  if (!fetcher->HasNextFrame()) {
    ShowMessage(this, "Failed to read file! The format may be incorrect.");
    return;
  }

  // Assign the fetcher.
  processor_.SetFetcher(fetcher);
}

void MainWindow::OpenSourceWebcam() {
  // Try to create the appropriate fetcher.
  shared_ptr<FrameFetcher> fetcher = make_shared<WebcamFrameFetcher>();

  // See if we can get frames from it.
  if (!fetcher->HasNextFrame()) {
    ShowMessage(this, "Error: Could not open webcam for reading.");
    return;
  }

  // Assign the fetcher to the pipeline.
  processor_.SetFetcher(fetcher);
}

void MainWindow::OpenThresholdDialog() {
  // TODO.
  std::cout << "Open threshold dialog.\n";
}

void MainWindow::ToggleShowMarkers() { show_markers_ = !show_markers_; }

void MainWindow::ToggleShowThreshold() { show_threshold_ = !show_threshold_; }
